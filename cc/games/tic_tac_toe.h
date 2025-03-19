#pragma once

#include "game.h"
#include <vector>
#include <string>

/**
 * TicTacToe game implementation.
 * Board positions are numbered 0-8 as follows:
 *   0 | 1 | 2
 *  ---+---+---
 *   3 | 4 | 5
 *  ---+---+---
 *   6 | 7 | 8
 */
class TicTacToe : public Game {
public:
    explicit TicTacToe(int starting_player = 1);
    
    // Game interface implementation
    void makeMove(int action) override;
    bool isGameOver() const override;
    std::vector<int> getPossibleActions() const override;
    void printState() const override;
    std::string serialize() const override;
    bool deserialize(const std::string& state) override;
    int getReward(int player) const override;
    std::unique_ptr<Game> clone() const override;
    double evaluatePosition() const override;
    bool isWinningMove(int action) const override;
    int getCurrentPlayer() const override;
    int getBoardSize() const override;
    std::string getGameName() const override;
    
private:
    static constexpr int BOARD_SIZE = 3;
    static constexpr int EMPTY_CELL = 0;
    
    std::vector<std::vector<int>> board;
    int current_player;
    
    // Helper methods
    bool checkWin() const;
    bool checkDraw() const;
    bool checkLine(const std::vector<std::vector<int>>& board, int start_row, int start_col, int delta_row, int delta_col) const;
    bool isValidPosition(int row, int col) const;
    int getWinner() const;
    double evaluateLine(int start_row, int start_col, int delta_row, int delta_col) const;
}; 