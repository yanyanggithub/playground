#pragma once
#include "../games/game.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

struct MCTSNode {
    std::vector<double> stats; // [visits, wins, UCB1 value]
    std::vector<int> untried_actions;
    std::vector<std::unique_ptr<MCTSNode>> children;
    std::unique_ptr<Game> game_state;
    int parent_action;
    MCTSNode* parent;
    std::mutex mutex; // For thread safety

    MCTSNode(std::unique_ptr<Game> state, int action = -1, MCTSNode* p = nullptr)
        : stats(3, 0.0), game_state(std::move(state)), parent_action(action), parent(p) {
        untried_actions = game_state->getPossibleActions();
    }
};

class MCTS {
public:
    struct Config {
        double exploration_constant;
        int num_simulations;
        int num_threads;
        bool use_heuristic;
        bool use_move_ordering;

        Config() : 
            exploration_constant(1.41),
            num_simulations(1000),
            num_threads(std::thread::hardware_concurrency()),
            use_heuristic(false),
            use_move_ordering(false) {}
    };

    explicit MCTS(const Config& config = Config());
    
    int selectAction(Game* game);
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }

private:
    Config config_;
    
    MCTSNode* select(MCTSNode* node);
    MCTSNode* expand(MCTSNode* node);
    int simulate(MCTSNode* node);
    void backpropagate(MCTSNode* node, int reward);
    
    // Parallel simulation helpers
    void parallelSimulate(MCTSNode* root, int num_threads);
    void workerThread(MCTSNode* root, int num_simulations);
    
    // Heuristic evaluation
    double evaluateState(const Game* state) const;
    
    // Move ordering
    void orderActions(std::vector<int>& actions, const Game* state) const;
}; 