#pragma once

#include <vector>
#include <string>
#include <memory>

class Game {
public:
    virtual ~Game() = default;
    
    // Core game mechanics
    virtual bool isGameOver() const = 0;
    virtual std::vector<int> getPossibleActions() const = 0;
    virtual void makeMove(int action) = 0;
    virtual int getReward(int player) const = 0;
    virtual std::unique_ptr<Game> clone() const = 0;
    virtual void printState() const = 0;
    
    // Game state management
    virtual std::string serialize() const = 0;
    virtual bool deserialize(const std::string& state) = 0;
    
    // Heuristic evaluation
    virtual double evaluatePosition() const = 0;
    virtual bool isWinningMove(int action) const = 0;
    
    // Game-specific information
    virtual int getCurrentPlayer() const = 0;
    virtual int getBoardSize() const = 0;
    virtual std::string getGameName() const = 0;
}; 