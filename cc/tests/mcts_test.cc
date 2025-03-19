#include "../mcts/mcts.h"
#include "../games/tic_tac_toe.h"
#include <gtest/gtest.h>
#include <memory>

class MCTSTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = std::make_unique<TicTacToe>(1);
        config.num_simulations = 1000;
        config.exploration_constant = 1.414;
        config.use_heuristic = true;
        config.use_move_ordering = true;
        config.num_threads = 1;
        mcts = std::make_unique<MCTS>(config);
    }

    std::unique_ptr<TicTacToe> game;
    std::unique_ptr<MCTS> mcts;
    MCTS::Config config;
};

TEST_F(MCTSTest, SelectActionTest) {
    // Test that MCTS returns a valid action
    int action = mcts->selectAction(game.get());
    EXPECT_GE(action, 0);
    EXPECT_LT(action, 9);
    
    // Test that the action is valid
    auto valid_actions = game->getPossibleActions();
    EXPECT_NE(std::find(valid_actions.begin(), valid_actions.end(), action), valid_actions.end());
}

TEST_F(MCTSTest, WinningMoveTest) {
    // Set up a winning position for X
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    game->makeMove(2); // O
    
    // MCTS should find the winning move
    int action = mcts->selectAction(game.get());
    EXPECT_EQ(action, 6); // The winning move
}

TEST_F(MCTSTest, BlockingMoveTest) {
    // Set up a position where O can win
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(2); // X
    game->makeMove(4); // O
    game->makeMove(8); // X
    
    // MCTS should block O's winning move
    int action = mcts->selectAction(game.get());
    EXPECT_EQ(action, 7); // The blocking move
}

TEST_F(MCTSTest, ParallelSimulationTest) {
    config.num_threads = 4;
    mcts = std::make_unique<MCTS>(config);
    
    int action = mcts->selectAction(game.get());
    EXPECT_GE(action, 0);
    EXPECT_LT(action, 9);
}

TEST_F(MCTSTest, MoveOrderingTest) {
    // Set up a position with multiple possible moves
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    
    // Test that move ordering improves performance
    auto start = std::chrono::high_resolution_clock::now();
    int action1 = mcts->selectAction(game.get());
    auto end1 = std::chrono::high_resolution_clock::now();
    
    config.use_move_ordering = false;
    mcts = std::make_unique<MCTS>(config);
    auto start2 = std::chrono::high_resolution_clock::now();
    int action2 = mcts->selectAction(game.get());
    auto end2 = std::chrono::high_resolution_clock::now();
    
    // Move ordering should be faster
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    EXPECT_LE(duration1.count(), duration2.count());
}

TEST_F(MCTSTest, HeuristicEvaluationTest) {
    // Set up a position where heuristic evaluation should help
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    
    // Test with and without heuristic
    config.use_heuristic = true;
    mcts = std::make_unique<MCTS>(config);
    int action1 = mcts->selectAction(game.get());
    
    config.use_heuristic = false;
    mcts = std::make_unique<MCTS>(config);
    int action2 = mcts->selectAction(game.get());
    
    // Both should be valid moves
    auto valid_actions = game->getPossibleActions();
    EXPECT_NE(std::find(valid_actions.begin(), valid_actions.end(), action1), valid_actions.end());
    EXPECT_NE(std::find(valid_actions.begin(), valid_actions.end(), action2), valid_actions.end());
}

TEST_F(MCTSTest, InvalidGameStateTest) {
    // Test with null game state
    EXPECT_EQ(mcts->selectAction(nullptr), -1);
}

TEST_F(MCTSTest, GameOverTest) {
    // Set up a game over state
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    game->makeMove(2); // O
    game->makeMove(6); // X wins
    
    // MCTS should return -1 for game over state
    EXPECT_EQ(mcts->selectAction(game.get()), -1);
} 