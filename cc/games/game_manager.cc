#include "game_manager.h"
#include "connect_four.h"
#include <fstream>
#include <iostream>
#include <algorithm>

GameManager::GameManager(const std::string& game_type, int ai_player)
    : game_type(game_type), ai_player(ai_player) {
    game = createGame(game_type, 1);
    
    // Configure MCTS
    MCTS::Config config;
    config.num_simulations = 1000;
    config.num_threads = std::thread::hardware_concurrency();
    config.use_heuristic = true;
    config.use_move_ordering = true;
    config.exploration_constant = 1.41;
    
    ai = std::make_unique<MCTS>(config);
}

bool GameManager::makeMove(int action) {
    if (!game || game->isGameOver()) return false;
    
    auto actions = game->getPossibleActions();
    if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
        return false;
    }
    
    game->makeMove(action);
    return true;
}

bool GameManager::makeAIMove() {
    if (!game || !ai || game->isGameOver()) return false;
    
    int action = ai->selectAction(game.get());
    if (action < 0) return false;
    
    return makeMove(action);
}

bool GameManager::isGameOver() const {
    return game ? game->isGameOver() : true;
}

void GameManager::printState() const {
    if (game) {
        game->printState();
    }
}

bool GameManager::saveGame(const std::string& filename) const {
    if (!game) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Save game type
    file << game_type << "\n";
    
    // Save game state
    file << game->serialize() << "\n";
    
    return true;
}

bool GameManager::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    // Read game type
    std::string saved_type;
    std::getline(file, saved_type);
    
    // Read game state
    std::string state;
    std::getline(file, state);
    
    // Create new game of the correct type
    game = createGame(saved_type, 1);
    if (!game) return false;
    
    // Load the state
    return game->deserialize(state);
}

int GameManager::getCurrentPlayer() const {
    return game ? game->getCurrentPlayer() : 0;
}

std::vector<int> GameManager::getPossibleActions() const {
    return game ? game->getPossibleActions() : std::vector<int>();
}

std::string GameManager::getGameType() const {
    return game_type;
}

std::unique_ptr<Game> GameManager::createGame(const std::string& type, int starting_player) {
    if (type == "connect_four") {
        return std::make_unique<ConnectFour>(starting_player);
    }
    // Add more game types here
    return nullptr;
} 