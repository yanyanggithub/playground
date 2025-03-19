#include "tictactoe.h"
#include <iostream>

TicTacToe::TicTacToe(int p) : Game(p) {
    board.fill(' ');
}

TicTacToe::TicTacToe(const TicTacToe& other) : Game(other.player) {
    board = other.board;
}

std::unique_ptr<Game> TicTacToe::clone() const {
    return std::make_unique<TicTacToe>(*this);
}

void TicTacToe::makeMove(int position) {
    if (position >= 0 && position < BOARD_SIZE && board[position] == ' ') {
        board[position] = (player == 1) ? 'x' : 'o';
        player = (player == 1) ? 2 : 1;
    }
}

void TicTacToe::printState() const {
    std::cout << "  0 | 1 | 2" << std::endl;
    for (int i = 0; i < BOARD_DIM; ++i) {
        std::cout << i + 1 << " | ";
        for (int j = 0; j < BOARD_DIM; ++j) {
            std::cout << board[i * BOARD_DIM + j] << " | ";
        }
        std::cout << std::endl;
    }
}

bool TicTacToe::isGameOver() const {
    return checkWin('x') || checkWin('o') || isBoardFull();
}

bool TicTacToe::checkWin(char symbol) const {
    // Check rows
    for (int i = 0; i < BOARD_DIM; ++i) {
        if (board[i * BOARD_DIM] == symbol && 
            board[i * BOARD_DIM + 1] == symbol && 
            board[i * BOARD_DIM + 2] == symbol) {
            return true;
        }
    }
    
    // Check columns
    for (int j = 0; j < BOARD_DIM; ++j) {
        if (board[j] == symbol && 
            board[j + BOARD_DIM] == symbol && 
            board[j + 2 * BOARD_DIM] == symbol) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0] == symbol && board[4] == symbol && board[8] == symbol) return true;
    if (board[2] == symbol && board[4] == symbol && board[6] == symbol) return true;
    
    return false;
}

bool TicTacToe::isBoardFull() const {
    for (char cell : board) {
        if (cell == ' ') return false;
    }
    return true;
}

int TicTacToe::getReward(int terminalPlayer) const {
    char symbol = (terminalPlayer == 1) ? 'x' : 'o';
    
    if (checkWin(symbol)) return 1;
    if (isBoardFull()) return 0;
    return -1;
}

std::vector<int> TicTacToe::getPossibleActions() const {
    std::vector<int> actions;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == ' ') {
            actions.push_back(i);
        }
    }
    return actions;
}

bool TicTacToe::isWinningMove(int action) const {
    if (action < 0 || action >= BOARD_SIZE || board[action] != ' ') return false;
    
    // Try the move
    TicTacToe temp(*this);
    temp.makeMove(action);
    
    // Check if it's a winning move
    return temp.checkWin((player == 1) ? 'x' : 'o');
} 