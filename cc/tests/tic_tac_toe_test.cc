#include "../games/tic_tac_toe.h"
#include <gtest/gtest.h>
#include <stdexcept>

class TicTacToeTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = std::make_unique<TicTacToe>(1);
    }

    std::unique_ptr<TicTacToe> game;
};

TEST_F(TicTacToeTest, ConstructorTest) {
    EXPECT_EQ(game->getCurrentPlayer(), 1);
    EXPECT_EQ(game->getBoardSize(), 3);
    EXPECT_EQ(game->getGameName(), "Tic Tac Toe");
    
    // Test invalid starting player
    EXPECT_THROW(TicTacToe(0), std::invalid_argument);
    EXPECT_THROW(TicTacToe(3), std::invalid_argument);
}

TEST_F(TicTacToeTest, MakeMoveTest) {
    // Test valid move
    EXPECT_NO_THROW(game->makeMove(0));
    EXPECT_EQ(game->getCurrentPlayer(), 2);
    
    // Test invalid moves
    EXPECT_THROW(game->makeMove(0), std::invalid_argument);  // Already taken
    EXPECT_THROW(game->makeMove(-1), std::invalid_argument); // Out of bounds
    EXPECT_THROW(game->makeMove(9), std::invalid_argument);  // Out of bounds
}

TEST_F(TicTacToeTest, WinConditionsTest) {
    // Test horizontal win
    game->makeMove(0); // X
    game->makeMove(3); // O
    game->makeMove(1); // X
    game->makeMove(4); // O
    game->makeMove(2); // X
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getWinner(), 1);
    
    // Test vertical win
    game = std::make_unique<TicTacToe>(1);
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    game->makeMove(2); // O
    game->makeMove(6); // X
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getWinner(), 1);
    
    // Test diagonal win
    game = std::make_unique<TicTacToe>(1);
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(4); // X
    game->makeMove(2); // O
    game->makeMove(8); // X
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getWinner(), 1);
}

TEST_F(TicTacToeTest, DrawConditionTest) {
    // Create a draw scenario
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(2); // X
    game->makeMove(3); // O
    game->makeMove(4); // X
    game->makeMove(5); // O
    game->makeMove(6); // X
    game->makeMove(7); // O
    game->makeMove(8); // X
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getWinner(), 0);
}

TEST_F(TicTacToeTest, PossibleActionsTest) {
    auto actions = game->getPossibleActions();
    EXPECT_EQ(actions.size(), 9);
    
    game->makeMove(0);
    actions = game->getPossibleActions();
    EXPECT_EQ(actions.size(), 8);
    EXPECT_EQ(std::find(actions.begin(), actions.end(), 0), actions.end());
}

TEST_F(TicTacToeTest, CloneTest) {
    game->makeMove(0);
    auto clone = game->clone();
    EXPECT_EQ(clone->getCurrentPlayer(), game->getCurrentPlayer());
    EXPECT_EQ(clone->getBoardSize(), game->getBoardSize());
    
    // Make a move on clone and verify original is unchanged
    clone->makeMove(1);
    EXPECT_NE(clone->getCurrentPlayer(), game->getCurrentPlayer());
}

TEST_F(TicTacToeTest, SerializationTest) {
    game->makeMove(0);
    std::string state = game->serialize();
    auto new_game = std::make_unique<TicTacToe>(1);
    EXPECT_TRUE(new_game->deserialize(state));
    EXPECT_EQ(new_game->getCurrentPlayer(), game->getCurrentPlayer());
}

TEST_F(TicTacToeTest, WinningMoveTest) {
    // Test winning move detection
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    game->makeMove(2); // O
    EXPECT_TRUE(game->isWinningMove(6)); // X can win with this move
    EXPECT_FALSE(game->isWinningMove(4)); // Not a winning move
}

TEST_F(TicTacToeTest, RewardTest) {
    // Test reward for winning
    game->makeMove(0); // X
    game->makeMove(1); // O
    game->makeMove(3); // X
    game->makeMove(2); // O
    game->makeMove(6); // X wins
    EXPECT_EQ(game->getReward(1), 1);
    EXPECT_EQ(game->getReward(2), -1);
    
    // Test reward for draw
    game = std::make_unique<TicTacToe>(1);
    for (int i = 0; i < 9; ++i) {
        game->makeMove(i);
    }
    EXPECT_EQ(game->getReward(1), 0);
    EXPECT_EQ(game->getReward(2), 0);
} 