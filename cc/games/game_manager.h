#pragma once

#include "game.h"
#include "../mcts/mcts.h"
#include <string>
#include <fstream>
#include <memory>

class GameManager {
public:
    GameManager(const std::string& game_type, int ai_player = 2);
    
    // Game control
    bool makeMove(int action);
    bool makeAIMove();
    bool isGameOver() const;
    void printState() const;
    
    // Game state management
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    
    // Game information
    int getCurrentPlayer() const;
    std::vector<int> getPossibleActions() const;
    std::string getGameType() const;
    
private:
    std::unique_ptr<Game> game;
    std::unique_ptr<MCTS> ai;
    int ai_player;
    std::string game_type;
    
    std::unique_ptr<Game> createGame(const std::string& type, int starting_player);
}; 