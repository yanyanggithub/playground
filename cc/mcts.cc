#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <memory>

// Base class for any game that can be played with MCTS
class Game {
public:
    int player; // Current player number (1 or 2)
    
    Game(int p) : player(p) {}
    virtual ~Game() = default;
    virtual bool isGameOver() const = 0;
    virtual std::vector<int> getPossibleActions() const = 0;
    virtual void makeMove(int action) = 0;
    virtual int getReward(int player) const = 0;
    virtual std::unique_ptr<Game> clone() const = 0;
    virtual void printState() const = 0;
};

// Tic-Tac-Toe implementation
class TicTacToe : public Game {
public:
    char board[9];  // Represents the Tic-Tac-Toe board (9 cells)

    TicTacToe(int p) : Game(p) {
        for (int i = 0; i < 9; ++i) {
            board[i] = ' ';
        }
    }

    TicTacToe(const TicTacToe& other) : Game(other.player) {
        for (int i = 0; i < 9; ++i) {
            board[i] = other.board[i];
        }
    }

    std::unique_ptr<Game> clone() const override {
        return std::make_unique<TicTacToe>(*this);
    }

    void makeMove(int position) override {
        if (position >= 0 && position < 9 && board[position] == ' ') {
            board[position] = (player == 1) ? 'x' : 'o';
            player = (player == 1) ? 2 : 1;
        }
    }

    void printState() const override {
        std::cout << "  0 | 1 | 2" << std::endl;
        for (int i = 0; i < 3; ++i) {
            std::cout << i + 1 << " | ";
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i * 3 + j] << " | ";
            }
            std::cout << std::endl;
        }
    }

    bool isGameOver() const override {
        // Check rows
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] != ' ' && 
                board[i * 3] == board[i * 3 + 1] && 
                board[i * 3] == board[i * 3 + 2]) {
                return true;
            }
        }
        // Check columns
        for (int j = 0; j < 3; ++j) {
            if (board[j] != ' ' && 
                board[j] == board[j + 3] && 
                board[j] == board[j + 6]) {
                return true;
            }
        }
        // Check diagonals
        if (board[0] != ' ' && board[0] == board[4] && board[0] == board[8]) return true;
        if (board[2] != ' ' && board[2] == board[4] && board[2] == board[6]) return true;

        // Check if board is full (draw)
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') return false;
        }

        return true; // Board is full (draw)
    }

    int getReward(int terminalPlayer) const override {
        char symbol = (terminalPlayer == 1) ? 'x' : 'o';
        
        // Check rows
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] == symbol && 
                board[i * 3 + 1] == symbol && 
                board[i * 3 + 2] == symbol) {
                return 1;
            }
        }
        
        // Check columns
        for (int j = 0; j < 3; ++j) {
            if (board[j] == symbol && 
                board[j + 3] == symbol && 
                board[j + 6] == symbol) {
                return 1;
            }
        }
        
        // Check diagonals
        if (board[0] == symbol && board[4] == symbol && board[8] == symbol) return 1;
        if (board[2] == symbol && board[4] == symbol && board[6] == symbol) return 1;

        // Check if board is full (draw)
        bool fullBoard = true;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                fullBoard = false;
                break;
            }
        }

        if (fullBoard) {
            return 0; // Draw
        }

        return -1; // Game not over or player loses
    }

    std::vector<int> getPossibleActions() const override {
        std::vector<int> actions;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                actions.push_back(i);
            }
        }
        return actions;
    }
};

// MCTS Node structure
struct MCTSNode {
    std::vector<double> stats; // [visits, wins, UCB1 value]
    std::vector<int> untried_actions;
    std::vector<std::unique_ptr<MCTSNode>> children;
    std::unique_ptr<Game> game_state;
    int parent_action;
    MCTSNode* parent;

    MCTSNode(std::unique_ptr<Game> state, int action = -1, MCTSNode* p = nullptr)
        : stats(3, 0.0), game_state(std::move(state)), parent_action(action), parent(p) {
        untried_actions = game_state->getPossibleActions();
    }
};

// MCTS Algorithm implementation
class MCTS {
private:
    const double C = 1.41; // UCB1 exploration parameter
    const int num_simulations;

    MCTSNode* select(MCTSNode* node) {
        while (!node->untried_actions.empty() && node->game_state->isGameOver() == false) {
            int action = node->untried_actions.back();
            node->untried_actions.pop_back();
            
            auto child_state = node->game_state->clone();
            child_state->makeMove(action);
            
            auto child = new MCTSNode(std::move(child_state), action, node);
            node->children.push_back(std::unique_ptr<MCTSNode>(child));
            return child;
        }
        return node;
    }

    MCTSNode* expand(MCTSNode* node) {
        if (node->untried_actions.empty()) return node;
        
        int action = node->untried_actions.back();
        node->untried_actions.pop_back();
        
        auto child_state = node->game_state->clone();
        child_state->makeMove(action);
        
        auto child = new MCTSNode(std::move(child_state), action, node);
        node->children.push_back(std::unique_ptr<MCTSNode>(child));
        return child;
    }

    int simulate(MCTSNode* node) {
        auto simulation = node->game_state->clone();
        while (!simulation->isGameOver()) {
            auto actions = simulation->getPossibleActions();
            if (actions.empty()) break;
            int action = actions[rand() % actions.size()];
            simulation->makeMove(action);
        }
        return simulation->getReward(node->game_state->player);
    }

    void backpropagate(MCTSNode* node, int reward) {
        while (node != nullptr) {
            node->stats[0] += 1; // Increment visits
            node->stats[1] += reward; // Add reward
            node = node->parent;
        }
    }

public:
    MCTS(int simulations = 1000) : num_simulations(simulations) {}

    int selectAction(Game* game) {
        auto root = std::make_unique<MCTSNode>(game->clone());
        
        for (int i = 0; i < num_simulations; ++i) {
            auto node = select(root.get());
            node = expand(node);
            int reward = simulate(node);
            backpropagate(node, reward);
        }

        // Select best action
        int best_action = -1;
        double best_value = -1e9;
        
        for (const auto& child : root->children) {
            if (child->stats[0] > 0) {
                double value = child->stats[1] / child->stats[0];
                if (value > best_value) {
                    best_value = value;
                    best_action = child->parent_action;
                }
            }
        }

        return best_action;
    }
};

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    auto game = std::make_unique<TicTacToe>(1);
    MCTS mcts(1000); // Run 1000 simulations per move

    while (!game->isGameOver()) {
        game->printState();
        
        std::vector<int> actions = game->getPossibleActions();
        if (actions.empty()) break;

        int action = mcts.selectAction(game.get());
        if (action >= 0 && action < 9) {
            game->makeMove(action);
        } else {
            break;
        }
    }

    std::cout << "Game Over!" << std::endl;
    game->printState();
    
    int winner = game->getReward(1) == 1 ? 1 : (game->getReward(2) == 1 ? 2 : 0);
    if (winner == 0) {
        std::cout << "It's a draw!" << std::endl;
    } else {
        std::cout << "Player " << winner << " wins!" << std::endl;
    }

    return 0;
}

