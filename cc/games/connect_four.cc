#include "connect_four.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

ConnectFour::ConnectFour(int starting_player) 
    : board(ROWS, std::vector<int>(COLS, 0)), current_player(starting_player) {}

bool ConnectFour::isGameOver() const {
    // Check each cell for a win
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (board[row][col] != 0) {
                // Check all directions
                if (checkDirection(row, col, 0, 1) ||  // Horizontal
                    checkDirection(row, col, 1, 0) ||  // Vertical
                    checkDirection(row, col, 1, 1) ||  // Diagonal (positive slope)
                    checkDirection(row, col, 1, -1)) { // Diagonal (negative slope)
                    return true;
                }
            }
        }
    }
    
    // Check if board is full
    for (int col = 0; col < COLS; ++col) {
        if (board[0][col] == 0) return false;
    }
    return true;
}

std::vector<int> ConnectFour::getPossibleActions() const {
    std::vector<int> actions;
    for (int col = 0; col < COLS; ++col) {
        if (board[0][col] == 0) {
            actions.push_back(col);
        }
    }
    return actions;
}

void ConnectFour::makeMove(int action) {
    if (action < 0 || action >= COLS) return;
    
    int row = getRowForColumn(action);
    if (row >= 0) {
        board[row][action] = current_player;
        current_player = (current_player == 1) ? 2 : 1;
    }
}

int ConnectFour::getReward(int player) const {
    // Check for win
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (board[row][col] == player) {
                // Check all directions
                if (checkDirection(row, col, 0, 1) ||  // Horizontal
                    checkDirection(row, col, 1, 0) ||  // Vertical
                    checkDirection(row, col, 1, 1) ||  // Diagonal (positive slope)
                    checkDirection(row, col, 1, -1)) { // Diagonal (negative slope)
                    return 1;
                }
            }
        }
    }
    
    // Check for opponent win
    int opponent = (player == 1) ? 2 : 1;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (board[row][col] == opponent) {
                // Check all directions
                if (checkDirection(row, col, 0, 1) ||  // Horizontal
                    checkDirection(row, col, 1, 0) ||  // Vertical
                    checkDirection(row, col, 1, 1) ||  // Diagonal (positive slope)
                    checkDirection(row, col, 1, -1)) { // Diagonal (negative slope)
                    return -1;
                }
            }
        }
    }
    
    // Check for draw
    bool is_full = true;
    for (int col = 0; col < COLS; ++col) {
        if (board[0][col] == 0) {
            is_full = false;
            break;
        }
    }
    if (is_full) return 0;
    
    return -1; // Game not over
}

std::unique_ptr<Game> ConnectFour::clone() const {
    auto clone = std::make_unique<ConnectFour>(current_player);
    clone->board = board;
    return clone;
}

void ConnectFour::printState() const {
    std::cout << "  ";
    for (int col = 0; col < COLS; ++col) {
        std::cout << col << " ";
    }
    std::cout << "\n";
    
    for (int row = 0; row < ROWS; ++row) {
        std::cout << row << " ";
        for (int col = 0; col < COLS; ++col) {
            char symbol = board[row][col] == 0 ? '.' : 
                         board[row][col] == 1 ? 'X' : 'O';
            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

std::string ConnectFour::serialize() const {
    std::stringstream ss;
    ss << current_player << " ";
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            ss << board[row][col] << " ";
        }
    }
    return ss.str();
}

bool ConnectFour::deserialize(const std::string& state) {
    std::stringstream ss(state);
    ss >> current_player;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!(ss >> board[row][col])) return false;
        }
    }
    return true;
}

double ConnectFour::evaluatePosition() const {
    double score = 0.0;
    
    // Evaluate horizontal lines
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col <= COLS - 4; ++col) {
            std::vector<int> line;
            for (int i = 0; i < 4; ++i) {
                line.push_back(board[row][col + i]);
            }
            score += evaluateLine(line);
        }
    }
    
    // Evaluate vertical lines
    for (int col = 0; col < COLS; ++col) {
        for (int row = 0; row <= ROWS - 4; ++row) {
            std::vector<int> line;
            for (int i = 0; i < 4; ++i) {
                line.push_back(board[row + i][col]);
            }
            score += evaluateLine(line);
        }
    }
    
    // Evaluate diagonal lines (positive slope)
    for (int row = 3; row < ROWS; ++row) {
        for (int col = 0; col <= COLS - 4; ++col) {
            std::vector<int> line;
            for (int i = 0; i < 4; ++i) {
                line.push_back(board[row - i][col + i]);
            }
            score += evaluateLine(line);
        }
    }
    
    // Evaluate diagonal lines (negative slope)
    for (int row = 0; row <= ROWS - 4; ++row) {
        for (int col = 0; col <= COLS - 4; ++col) {
            std::vector<int> line;
            for (int i = 0; i < 4; ++i) {
                line.push_back(board[row + i][col + i]);
            }
            score += evaluateLine(line);
        }
    }
    
    return score;
}

bool ConnectFour::isWinningMove(int action) const {
    if (action < 0 || action >= COLS) return false;
    
    int row = getRowForColumn(action);
    if (row < 0) return false;
    
    // Create a temporary copy of the board
    std::vector<std::vector<int>> temp_board = board;
    temp_board[row][action] = current_player;
    
    // Check if this move wins
    int player = temp_board[row][action];
    if (player == 0) return false;
    
    // Check all directions
    return checkDirection(row, action, 0, 1, temp_board) ||  // Horizontal
           checkDirection(row, action, 1, 0, temp_board) ||  // Vertical
           checkDirection(row, action, 1, 1, temp_board) ||  // Diagonal (positive slope)
           checkDirection(row, action, 1, -1, temp_board);   // Diagonal (negative slope)
}

bool ConnectFour::checkDirection(int row, int col, int drow, int dcol, const std::vector<std::vector<int>>& board) const {
    int player = board[row][col];
    int count = 1;
    
    // Check in positive direction
    for (int i = 1; i < 4; ++i) {
        int new_row = row + i * drow;
        int new_col = col + i * dcol;
        if (new_row < 0 || new_row >= ROWS || new_col < 0 || new_col >= COLS) break;
        if (board[new_row][new_col] != player) break;
        count++;
    }
    
    // Check in negative direction
    for (int i = 1; i < 4; ++i) {
        int new_row = row - i * drow;
        int new_col = col - i * dcol;
        if (new_row < 0 || new_row >= ROWS || new_col < 0 || new_col >= COLS) break;
        if (board[new_row][new_col] != player) break;
        count++;
    }
    
    return count >= 4;
}

bool ConnectFour::checkDirection(int row, int col, int drow, int dcol) const {
    return checkDirection(row, col, drow, dcol, board);
}

int ConnectFour::getRowForColumn(int col) const {
    for (int row = ROWS - 1; row >= 0; --row) {
        if (board[row][col] == 0) {
            return row;
        }
    }
    return -1;
}

double ConnectFour::evaluateLine(const std::vector<int>& line) const {
    int player1_count = 0;
    int player2_count = 0;
    int empty_count = 0;
    
    for (int cell : line) {
        if (cell == 0) empty_count++;
        else if (cell == 1) player1_count++;
        else player2_count++;
    }
    
    // If line is blocked by opponent, it's worthless
    if (player1_count > 0 && player2_count > 0) return 0.0;
    
    // If line is empty, it's worthless
    if (empty_count == 4) return 0.0;
    
    // Score based on number of pieces in line
    double score = 0.0;
    if (player1_count > 0) {
        score = std::pow(10.0, player1_count);
    } else if (player2_count > 0) {
        score = -std::pow(10.0, player2_count);
    }
    
    return score;
} 