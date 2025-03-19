#pragma once
#include <vector>
#include <memory>

class Game {
public:
    int player; // Current player number (1 or 2)
    
    Game(int p) : player(p) {}
    virtual ~Game() = default;
    
    // Core game interface
    virtual bool isGameOver() const = 0;
    virtual std::vector<int> getPossibleActions() const = 0;
    virtual void makeMove(int action) = 0;
    virtual int getReward(int player) const = 0;
    virtual std::unique_ptr<Game> clone() const = 0;
    virtual void printState() const = 0;
    
    // Optional methods that can be overridden for better performance
    virtual bool isTerminal() const { return isGameOver(); }
    virtual double getStateValue() const { return 0.0; } // For heuristic evaluation
    virtual bool isWinningMove(int action) const { return false; } // For move ordering
}; 