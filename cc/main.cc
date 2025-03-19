#include "games/game_manager.h"
#include <iostream>
#include <string>
#include <random>

int main() {
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::srand(gen());
    
    // Create game manager
    GameManager manager("connect_four", 2);  // AI plays as player 2
    
    std::cout << "Welcome to Connect Four!" << std::endl;
    std::cout << "You are player 1 (X), AI is player 2 (O)" << std::endl;
    std::cout << "Enter column number (0-6) to make a move" << std::endl;
    std::cout << "Enter 's' to save game, 'l' to load game, 'q' to quit" << std::endl << std::endl;
    
    while (!manager.isGameOver()) {
        manager.printState();
        
        std::cout << "Current player: " << manager.getCurrentPlayer() << std::endl;
        
        if (manager.getCurrentPlayer() == 1) {
            // Human player's turn
            std::string input;
            std::cout << "Enter your move: ";
            std::getline(std::cin, input);
            
            if (input == "q") break;
            if (input == "s") {
                std::string filename;
                std::cout << "Enter filename to save: ";
                std::getline(std::cin, filename);
                if (manager.saveGame(filename)) {
                    std::cout << "Game saved successfully!" << std::endl;
                } else {
                    std::cout << "Failed to save game." << std::endl;
                }
                continue;
            }
            if (input == "l") {
                std::string filename;
                std::cout << "Enter filename to load: ";
                std::getline(std::cin, filename);
                if (manager.loadGame(filename)) {
                    std::cout << "Game loaded successfully!" << std::endl;
                } else {
                    std::cout << "Failed to load game." << std::endl;
                }
                continue;
            }
            
            try {
                int action = std::stoi(input);
                if (!manager.makeMove(action)) {
                    std::cout << "Invalid move! Try again." << std::endl;
                    continue;
                }
            } catch (const std::exception& e) {
                std::cout << "Invalid input! Try again." << std::endl;
                continue;
            }
        } else {
            // AI's turn
            std::cout << "AI is thinking..." << std::endl;
            if (!manager.makeAIMove()) {
                std::cout << "AI failed to make a move!" << std::endl;
                break;
            }
        }
    }
    
    // Game over
    std::cout << "\nGame Over!" << std::endl;
    manager.printState();
    
    // Determine winner
    int winner = manager.getCurrentPlayer() == 1 ? 2 : 1;
    if (winner == 1) {
        std::cout << "Congratulations! You won!" << std::endl;
    } else if (winner == 2) {
        std::cout << "AI wins!" << std::endl;
    } else {
        std::cout << "It's a draw!" << std::endl;
    }
    
    return 0;
} 