#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

class GameState {
public:
    int player; // 1 or 2, representing the current player
    char board[3];  // Represents the Tic-Tac-Toe board (0 = empty)

    GameState(int p) : player(p) {
        for (int i = 0; i < 3; ++i) {
            board[i] = ' ';
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
            if (board[i * 3] != ' ') {
                return true;
            }
        }
        // Check columns
        for (int j = 0; j < 3; ++j) {
            if (board[j] != ' ') {
                return true;
            }
        }
        // Check diagonals
        if (board[0] != ' ' && board[4] != ' ' && board[8] == ' ') return false; // Added check for diagonal win condition
        if (board[2] != ' ' && board[4] != ' ' && board[6] == ' ') return false;

        return true; // No winner, game over
    }

    int getReward(int terminalPlayer) const {
        // Check if the player won
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] == terminalPlayer) {
                return 1; // Player wins
            }
        }

        // Check if board is full and no winner
        bool fullBoard = true;
        for (int i = 0; i < 3; ++i) {
            if (board[i] == ' ') {
                fullBoard = false;
                break;
            }
        }

        if (fullBoard) {
            return 0; // Draw
        }

        return -1; // Player loses
    }

    std::vector<int> getPossibleActions() const {
        std::vector<int> actions;
        for (int i = 0; i < 3; ++i) {
            if (board[i * 3] == ' ') {
                actions.push_back(i); // Index represents the cell
            }
        }
        return actions;
    }
};

void select_expand(GameState& state, std::vector<std::vector<bool>>& nodes, int &node_index) {
    while (!nodes[node_index][0]) {
        node_index = rand() % state.getPossibleActions().size();
    }
}

void simulate(GameState& state) {
    // Simple random move for simulation (can be improved)
    std::vector<int> actions = state.getPossibleActions();
    if (!actions.empty()) {
        state.board[actions[rand() % actions.size()]] = (state.player == 1) ? 'x' : 'o';
    }
}

void backpropagate(GameState& state, int reward) {
    // No need for complex backpropagation in this simplified version
}


int main() {
    GameState game(1); // Start with player 1 (X)
    game.printBoard();

    while (!game.isGameOver()) {
        std::vector<int> actions = game.getPossibleActions();
        if (actions.empty()) break;  // No moves left, draw

        int action_index = rand() % actions.size();
        int cell = actions[action_index];
        game.board[cell] = (game.player == 1) ? 'x' : 'o';
        game.printBoard();

        // MCTS Simulation
        std::vector<std::vector<bool>> nodes(1000); // Example: 1000 nodes
        int node_index;
        select_expand(game, nodes, node_index);
        simulate(game);
        backpropagate(game, game.getReward(game.player));

        // Update player turn
        game.player = (game.player == 1) ? 2 : 1;
    }

    std::cout << "Game Over!" << std::endl;
    if (game.isGameOver()) {
        std::cout << "Draw" << std::endl;
    } else {
        std::cout << "Player " << game.player << " wins!" << std::endl;
    }

    return 0;
}

