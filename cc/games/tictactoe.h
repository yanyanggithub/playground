#pragma once
#include "game.h"
#include <array>

class TicTacToe : public Game {
public:
    static constexpr int BOARD_SIZE = 9;
    static constexpr int BOARD_DIM = 3;
    
    TicTacToe(int p = 1);
    TicTacToe(const TicTacToe& other);
    
    // Core game interface implementation
    bool isGameOver() const override;
    std::vector<int> getPossibleActions() const override;
    void makeMove(int position) override;
    int getReward(int terminalPlayer) const override;
    std::unique_ptr<Game> clone() const override;
    void printState() const override;
    
    // Optional methods implementation
    bool isWinningMove(int action) const override;
    
    // Helper methods
    bool checkWin(char symbol) const;
    bool isBoardFull() const;
    
private:
    std::array<char, BOARD_SIZE> board;
}; 