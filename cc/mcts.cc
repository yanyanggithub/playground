#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

class GameState {
public:
    int player; // 1 or 2, representing the current player
    char board[9];  // Represents the Tic-Tac-Toe board (9 cells)

    GameState(int p) : player(p) {
        for (int i = 0; i < 9; ++i) {
            board[i] = ' ';
        }
    }

    GameState(const GameState& other) {
        player = other.player;
        for (int i = 0; i < 9; ++i) {
            board[i] = other.board[i];
        }
    }

    void makeMove(int position) {
        if (position >= 0 && position < 9 && board[position] == ' ') {
            board[position] = (player == 1) ? 'x' : 'o';
            player = (player == 1) ? 2 : 1;
        }
    }

    void printBoard() const {
        std::cout << "  0 | 1 | 2" << std::endl;
        for (int i = 0; i < 3; ++i) {
            std::cout << i + 1 << " | ";
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i * 3 + j] << " | ";
            }
            std::cout << std::endl;
        }
    }

    bool isGameOver() const {
        // Check rows
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] != ' ' && 
                board[i * 3] == board[i * 3 + 1] && 
                board[i * 3] == board[i * 3 + 2]) {
                return true;
            }
        }
        // Check columns
        for (int j = 0; j < 3; ++j) {
            if (board[j] != ' ' && 
                board[j] == board[j + 3] && 
                board[j] == board[j + 6]) {
                return true;
            }
        }
        // Check diagonals
        if (board[0] != ' ' && board[0] == board[4] && board[0] == board[8]) return true;
        if (board[2] != ' ' && board[2] == board[4] && board[2] == board[6]) return true;

        // Check if board is full (draw)
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') return false;
        }

        return true; // Board is full (draw)
    }

    int getReward(int terminalPlayer) const {
        char symbol = (terminalPlayer == 1) ? 'x' : 'o';
        
        // Check rows
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] == symbol && 
                board[i * 3 + 1] == symbol && 
                board[i * 3 + 2] == symbol) {
                return 1;
            }
        }
        
        // Check columns
        for (int j = 0; j < 3; ++j) {
            if (board[j] == symbol && 
                board[j + 3] == symbol && 
                board[j + 6] == symbol) {
                return 1;
            }
        }
        
        // Check diagonals
        if (board[0] == symbol && board[4] == symbol && board[8] == symbol) return 1;
        if (board[2] == symbol && board[4] == symbol && board[6] == symbol) return 1;

        // Check if board is full (draw)
        bool fullBoard = true;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                fullBoard = false;
                break;
            }
        }

        if (fullBoard) {
            return 0; // Draw
        }

        return -1; // Game not over or player loses
    }

    std::vector<int> getPossibleActions() const {
        std::vector<int> actions;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                actions.push_back(i);
            }
        }
        return actions;
    }
};

void select_expand(GameState& state, std::vector<std::vector<double>>& nodes, int &node_index) {
    const double C = 1.41; // UCB1 exploration parameter
    std::vector<int> actions = state.getPossibleActions();
    if (actions.empty()) return;

    // Ensure node_index is valid
    if (node_index < 0 || node_index >= nodes.size()) {
        node_index = 0;
    }

    // If node doesn't exist, create it
    if (nodes[node_index].empty()) {
        nodes[node_index] = std::vector<double>(3, 0.0); // [visits, wins, UCB1 value]
        return;
    }

    // Select best child using UCB1
    double best_value = -1e9;
    int best_action = actions[0];
    
    for (int action : actions) {
        // Ensure action is valid
        if (action < 0 || action >= nodes.size()) continue;
        
        if (nodes[action].empty()) {
            nodes[action] = std::vector<double>(3, 0.0);
            node_index = action;
            return;
        }
        
        // Avoid division by zero
        if (nodes[action][0] == 0) {
            node_index = action;
            return;
        }
        
        double ucb1 = nodes[action][1] / nodes[action][0] + 
                      C * sqrt(log(nodes[node_index][0]) / nodes[action][0]);
        if (ucb1 > best_value) {
            best_value = ucb1;
            best_action = action;
        }
    }
    node_index = best_action;
}

void simulate(GameState& state) {
    GameState simulation = state; // Create a copy for simulation
    
    while (!simulation.isGameOver()) {
        std::vector<int> actions = simulation.getPossibleActions();
        if (actions.empty()) break;

        int action = actions[rand() % actions.size()];
        simulation.makeMove(action);
    }
}

void backpropagate(GameState& state, std::vector<std::vector<double>>& nodes, int node_index, int reward) {
    if (node_index < 0 || node_index >= nodes.size()) return;
    if (nodes[node_index].empty()) {
        nodes[node_index] = std::vector<double>(3, 0.0);
    }
    nodes[node_index][0] += 1; // Increment visits
    nodes[node_index][1] += reward; // Add reward
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    GameState game(1); // Start with player 1 (X)
    game.printBoard();

    const int NUM_NODES = 1000;
    std::vector<std::vector<double>> nodes(NUM_NODES); // [visits, wins, UCB1 value]
    
    while (!game.isGameOver()) {
        std::vector<int> actions = game.getPossibleActions();
        if (actions.empty()) break;

        // Run MCTS simulations
        int best_action = -1;
        double best_value = -1e9;
        for (int i = 0; i < 1000; ++i) { // Run 1000 MCTS iterations
            GameState simulation = game;
            int node_index = 0;
            
            // Selection and Expansion
            select_expand(simulation, nodes, node_index);
            
            // Simulation
            simulate(simulation);
            
            // Backpropagation
            int reward = simulation.getReward(game.player);
            backpropagate(simulation, nodes, node_index, reward);
            
            // Track best action
            if (node_index >= 0 && node_index < nodes.size() && !nodes[node_index].empty()) {
                double value = nodes[node_index][1] / nodes[node_index][0];
                if (value > best_value) {
                    best_value = value;
                    best_action = node_index;
                }
            }
        }

        // Make the best move
        if (best_action >= 0 && best_action < 9) {
            game.makeMove(best_action);
            game.printBoard();
        } else {
            break; // No valid move found
        }
    }

    std::cout << "Game Over!" << std::endl;
    int winner = game.getReward(1) == 1 ? 1 : (game.getReward(2) == 1 ? 2 : 0);
    if (winner == 0) {
        std::cout << "It's a draw!" << std::endl;
    } else {
        std::cout << "Player " << winner << " wins!" << std::endl;
    }

    return 0;
}

