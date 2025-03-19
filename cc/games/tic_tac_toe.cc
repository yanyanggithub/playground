#include "tic_tac_toe.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>

TicTacToe::TicTacToe(int starting_player)
    : current_player(starting_player) {
    if (starting_player != 1 && starting_player != 2) {
        throw std::invalid_argument("Starting player must be 1 or 2");
    }
    board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, EMPTY_CELL));
}

void TicTacToe::makeMove(int action) {
    // Validate action bounds
    if (action < 0 || action >= BOARD_SIZE * BOARD_SIZE) {
        throw std::invalid_argument("Invalid move: position out of bounds");
    }
    
    int row = action / BOARD_SIZE;
    int col = action % BOARD_SIZE;
    
    // Validate position
    if (!isValidPosition(row, col)) {
        throw std::invalid_argument("Invalid move: position out of bounds");
    }
    
    // Check if position is already taken
    if (board[row][col] != EMPTY_CELL) {
        throw std::invalid_argument("Invalid move: position already taken");
    }
    
    // Make the move
    board[row][col] = current_player;
    current_player = (current_player == 1) ? 2 : 1;
}

bool TicTacToe::isGameOver() const {
    return checkWin() || checkDraw();
}

std::vector<int> TicTacToe::getPossibleActions() const {
    std::vector<int> actions;
    actions.reserve(BOARD_SIZE * BOARD_SIZE);  // Pre-allocate for efficiency
    
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        int row = i / BOARD_SIZE;
        int col = i % BOARD_SIZE;
        if (isValidPosition(row, col) && board[row][col] == EMPTY_CELL) {
            actions.push_back(i);
        }
    }
    return actions;
}

void TicTacToe::printState() const {
    std::cout << "\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY_CELL) {
                std::cout << " " << (i * BOARD_SIZE + j) << " ";
            } else {
                std::cout << " " << (board[i][j] == 1 ? "X" : "O") << " ";
            }
            if (j < BOARD_SIZE - 1) std::cout << "|";
        }
        std::cout << "\n";
        if (i < BOARD_SIZE - 1) {
            std::cout << "---+---+---\n";
        }
    }
    std::cout << "\n";
}

std::string TicTacToe::serialize() const {
    std::stringstream ss;
    ss << current_player << " ";
    for (const auto& row : board) {
        for (int cell : row) {
            ss << cell << " ";
        }
    }
    return ss.str();
}

bool TicTacToe::deserialize(const std::string& state) {
    try {
        std::stringstream ss(state);
        ss >> current_player;
        
        if (current_player != 1 && current_player != 2) {
            return false;
        }
        
        for (auto& row : board) {
            for (int& cell : row) {
                ss >> cell;
                if (cell != EMPTY_CELL && cell != 1 && cell != 2) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool TicTacToe::checkLine(const std::vector<std::vector<int>>& board, int start_row, int start_col, int delta_row, int delta_col) const {
    // Validate start position
    if (!isValidPosition(start_row, start_col)) {
        return false;
    }
    
    int player = board[start_row][start_col];
    if (player == EMPTY_CELL) return false;
    
    // Check the line
    for (int i = 0; i < BOARD_SIZE; ++i) {
        int row = start_row + i * delta_row;
        int col = start_col + i * delta_col;
        
        // Validate each position in the line
        if (!isValidPosition(row, col) || board[row][col] != player) {
            return false;
        }
    }
    return true;
}

bool TicTacToe::checkWin() const {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (checkLine(board, i, 0, 0, 1)) return true;
    }
    
    // Check columns
    for (int j = 0; j < BOARD_SIZE; ++j) {
        if (checkLine(board, 0, j, 1, 0)) return true;
    }
    
    // Check diagonals
    if (checkLine(board, 0, 0, 1, 1)) return true;
    if (checkLine(board, 0, BOARD_SIZE - 1, 1, -1)) return true;
    
    return false;
}

bool TicTacToe::checkDraw() const {
    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == EMPTY_CELL) return false;
        }
    }
    return true;
}

bool TicTacToe::isValidPosition(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

int TicTacToe::getWinner() const {
    if (checkDraw()) return 0;
    if (!checkWin()) return 0;
    
    // Check each line to find the winner
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (checkLine(board, i, 0, 0, 1)) return board[i][0];
        if (checkLine(board, 0, i, 1, 0)) return board[0][i];
    }
    
    if (checkLine(board, 0, 0, 1, 1)) return board[0][0];
    if (checkLine(board, 0, BOARD_SIZE - 1, 1, -1)) return board[0][BOARD_SIZE - 1];
    
    return 0;
}

int TicTacToe::getReward(int player) const {
    if (!isGameOver()) return 0;
    
    if (checkDraw()) return 0;
    
    int winner = getWinner();
    return (player == winner) ? 1 : -1;
}

std::unique_ptr<Game> TicTacToe::clone() const {
    auto clone = std::make_unique<TicTacToe>(current_player);
    clone->board = board;
    return clone;
}

double TicTacToe::evaluateLine(int start_row, int start_col, int delta_row, int delta_col) const {
    int player_count = 0;
    int opponent_count = 0;
    int empty_count = 0;
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        int row = start_row + i * delta_row;
        int col = start_col + i * delta_col;
        
        if (!isValidPosition(row, col)) continue;
        
        if (board[row][col] == EMPTY_CELL) {
            empty_count++;
        } else if (board[row][col] == current_player) {
            player_count++;
        } else {
            opponent_count++;
        }
    }
    
    // Scoring based on line composition
    if (player_count == 2 && empty_count == 2) return 0.1;    // Two in a row
    if (player_count == 3) return 1.0;                        // Win
    if (opponent_count == 2 && empty_count == 2) return -0.1; // Opponent two in a row
    if (opponent_count == 3) return -1.0;                     // Opponent win
    
    return 0.0;
}

double TicTacToe::evaluatePosition() const {
    if (isGameOver()) return getReward(current_player);
    
    double score = 0.0;
    
    // Evaluate all possible lines
    for (int i = 0; i < BOARD_SIZE; ++i) {
        score += evaluateLine(i, 0, 0, 1);    // Rows
        score += evaluateLine(0, i, 1, 0);    // Columns
    }
    score += evaluateLine(0, 0, 1, 1);        // Main diagonal
    score += evaluateLine(0, BOARD_SIZE - 1, 1, -1);  // Anti-diagonal
    
    return score;
}

bool TicTacToe::isWinningMove(int action) const {
    // Validate action bounds
    if (action < 0 || action >= BOARD_SIZE * BOARD_SIZE) {
        return false;
    }
    
    int row = action / BOARD_SIZE;
    int col = action % BOARD_SIZE;
    
    // Validate position
    if (!isValidPosition(row, col) || board[row][col] != EMPTY_CELL) {
        return false;
    }
    
    // Create a temporary board to test the move
    std::vector<std::vector<int>> temp_board = board;
    temp_board[row][col] = current_player;
    
    // Check if this move would win
    bool is_winning = false;
    
    // Check rows
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (checkLine(temp_board, i, 0, 0, 1)) {
            is_winning = true;
            break;
        }
    }
    
    // Check columns
    if (!is_winning) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (checkLine(temp_board, 0, j, 1, 0)) {
                is_winning = true;
                break;
            }
        }
    }
    
    // Check diagonals
    if (!is_winning) {
        if (checkLine(temp_board, 0, 0, 1, 1)) is_winning = true;
        if (!is_winning && checkLine(temp_board, 0, BOARD_SIZE - 1, 1, -1)) is_winning = true;
    }
    
    return is_winning;
}

int TicTacToe::getCurrentPlayer() const {
    return current_player;
}

int TicTacToe::getBoardSize() const {
    return BOARD_SIZE;
}

std::string TicTacToe::getGameName() const {
    return "Tic Tac Toe";
} 