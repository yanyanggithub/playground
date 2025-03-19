#pragma once

#include "game.h"
#include <vector>
#include <string>

class ConnectFour : public Game {
public:
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;
    
    ConnectFour(int starting_player = 1);
    
    // Core game mechanics
    bool isGameOver() const override;
    std::vector<int> getPossibleActions() const override;
    void makeMove(int action) override;
    int getReward(int player) const override;
    std::unique_ptr<Game> clone() const override;
    void printState() const override;
    
    // Game state management
    std::string serialize() const override;
    bool deserialize(const std::string& state) override;
    
    // Heuristic evaluation
    double evaluatePosition() const override;
    bool isWinningMove(int action) const override;
    
    // Game-specific information
    int getCurrentPlayer() const override { return current_player; }
    int getBoardSize() const override { return ROWS * COLS; }
    std::string getGameName() const override { return "Connect Four"; }
    
private:
    std::vector<std::vector<int>> board;
    int current_player;
    
    bool checkDirection(int row, int col, int drow, int dcol) const;
    bool checkDirection(int row, int col, int drow, int dcol, const std::vector<std::vector<int>>& board) const;
    int getRowForColumn(int col) const;
    double evaluateLine(const std::vector<int>& line) const;
}; 