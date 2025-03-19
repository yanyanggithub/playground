#include "games/tictactoe.h"
#include "mcts/mcts.h"
#include <iostream>
#include <memory>
#include <random>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::srand(gen()); // Seed the global random number generator
    
    // Create game instance
    auto game = std::make_unique<TicTacToe>(1);
    
    // Configure MCTS
    MCTS::Config config;
    config.num_simulations = 1000;
    config.num_threads = std::thread::hardware_concurrency();
    config.use_heuristic = true;
    config.use_move_ordering = true;
    
    MCTS mcts(config);
    
    // Game loop
    while (!game->isGameOver()) {
        game->printState();
        
        std::vector<int> actions = game->getPossibleActions();
        if (actions.empty()) break;

        int action = mcts.selectAction(game.get());
        if (action >= 0 && action < TicTacToe::BOARD_SIZE) {
            game->makeMove(action);
        } else {
            break;
        }
    }

    // Game over
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