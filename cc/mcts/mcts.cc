#include "mcts.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>

MCTS::MCTS(const Config& config) : config_(config) {}

int MCTS::selectAction(Game* game) {
    if (!game) return -1;
    
    auto root = std::make_unique<MCTSNode>(game->clone());
    if (!root) return -1;
    
    // Get valid actions before starting simulation
    auto valid_actions = game->getPossibleActions();
    if (valid_actions.empty()) return -1;
    
    if (config_.num_threads > 1) {
        parallelSimulate(root.get(), config_.num_threads);
    } else {
        for (int i = 0; i < config_.num_simulations; ++i) {
            auto node = select(root.get());
            if (!node) continue;
            
            node = expand(node);
            if (!node) continue;
            
            int reward = simulate(node);
            backpropagate(node, reward);
        }
    }

    // Select best action
    int best_action = -1;
    double best_value = -1e9;
    
    for (const auto& child : root->children) {
        if (!child) continue;
        
        std::lock_guard<std::mutex> lock(child->mutex);
        if (child->stats[0] > 0) {
            double value = child->stats[1] / child->stats[0];
            if (value > best_value) {
                best_value = value;
                best_action = child->parent_action;
            }
        }
    }

    // Validate the selected action
    if (best_action < 0 || std::find(valid_actions.begin(), valid_actions.end(), best_action) == valid_actions.end()) {
        // If no valid action found, select a random valid action
        best_action = valid_actions[rand() % valid_actions.size()];
    }

    return best_action;
}

MCTSNode* MCTS::select(MCTSNode* node) {
    if (!node || !node->game_state) return nullptr;

    // If there are untried actions, return this node
    if (!node->untried_actions.empty()) {
        return node;
    }

    // If no children, return this node
    if (node->children.empty()) {
        return node;
    }

    // Select best child using UCB1
    MCTSNode* best_child = nullptr;
    double best_value = -1e9;
    
    std::lock_guard<std::mutex> lock(node->mutex);
    double parent_visits = node->stats[0];
    if (parent_visits == 0) return node->children[0].get();

    for (const auto& child : node->children) {
        if (!child) continue;
        
        std::lock_guard<std::mutex> child_lock(child->mutex);
        if (child->stats[0] == 0) {
            return child.get();
        }

        double exploitation = child->stats[1] / child->stats[0];
        double exploration = config_.exploration_constant * 
                            std::sqrt(std::log(parent_visits) / child->stats[0]);
        double ucb1 = exploitation + exploration;

        if (ucb1 > best_value) {
            best_value = ucb1;
            best_child = child.get();
        }
    }

    // Recursively select from best child
    return best_child ? select(best_child) : node;
}

MCTSNode* MCTS::expand(MCTSNode* node) {
    if (!node || !node->game_state || node->untried_actions.empty() || node->game_state->isGameOver()) {
        return node;
    }
    
    std::lock_guard<std::mutex> lock(node->mutex);
    
    // Get valid actions
    auto valid_actions = node->game_state->getPossibleActions();
    if (valid_actions.empty()) return node;
    
    // Find a valid action from untried actions
    int action = -1;
    while (!node->untried_actions.empty()) {
        action = node->untried_actions.back();
        if (std::find(valid_actions.begin(), valid_actions.end(), action) != valid_actions.end()) {
            node->untried_actions.pop_back();
            break;
        }
        node->untried_actions.pop_back();
    }
    
    if (action == -1) return node;
    
    auto child_state = node->game_state->clone();
    if (!child_state) return node;
    
    try {
        child_state->makeMove(action);
    } catch (const std::exception&) {
        return node;
    }
    
    auto child = new MCTSNode(std::move(child_state), action, node);
    if (!child) return node;
    
    node->children.push_back(std::unique_ptr<MCTSNode>(child));
    return child;
}

int MCTS::simulate(MCTSNode* node) {
    if (!node || !node->game_state) return 0;
    
    auto simulation = node->game_state->clone();
    if (!simulation) return 0;
    
    while (!simulation->isGameOver()) {
        auto actions = simulation->getPossibleActions();
        if (actions.empty()) break;
        
        if (config_.use_move_ordering) {
            orderActions(actions, simulation.get());
        }
        
        // Try to make a valid move
        bool move_made = false;
        for (int attempts = 0; attempts < actions.size() && !move_made; ++attempts) {
            int action = actions[rand() % actions.size()];
            try {
                simulation->makeMove(action);
                move_made = true;
            } catch (const std::exception&) {
                // Remove invalid action and try another
                actions.erase(std::remove(actions.begin(), actions.end(), action), actions.end());
            }
        }
        
        if (!move_made) break;
    }
    
    if (config_.use_heuristic) {
        return evaluateState(simulation.get());
    }
    
    return simulation->getReward(simulation->getCurrentPlayer());
}

void MCTS::backpropagate(MCTSNode* node, int reward) {
    while (node != nullptr) {
        std::lock_guard<std::mutex> lock(node->mutex);
        node->stats[0] += 1; // Increment visits
        node->stats[1] += reward; // Add reward
        node = node->parent;
    }
}

void MCTS::parallelSimulate(MCTSNode* root, int num_threads) {
    if (!root) return;
    
    std::vector<std::thread> threads;
    int simulations_per_thread = std::max(1, config_.num_simulations / num_threads);
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(&MCTS::workerThread, this, root, simulations_per_thread);
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void MCTS::workerThread(MCTSNode* root, int num_simulations) {
    if (!root) return;
    
    for (int i = 0; i < num_simulations; ++i) {
        auto node = select(root);
        if (!node) continue;
        
        node = expand(node);
        if (!node) continue;
        
        int reward = simulate(node);
        backpropagate(node, reward);
    }
}

double MCTS::evaluateState(const Game* state) const {
    if (!state) return 0.0;
    return state->getReward(state->getCurrentPlayer());
}

void MCTS::orderActions(std::vector<int>& actions, const Game* state) const {
    if (!state) return;
    
    std::sort(actions.begin(), actions.end(),
        [state](int a, int b) {
            return state->isWinningMove(a) > state->isWinningMove(b);
        });
} 