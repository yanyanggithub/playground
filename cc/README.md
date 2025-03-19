# Game AI Project

This project implements a Monte Carlo Tree Search (MCTS) based AI for various board games, including Tic Tac Toe and Connect Four.

## Project Structure

```
cc/
├── games/              # Game implementations
│   ├── game.h         # Base game interface
│   ├── tic_tac_toe.h  # Tic Tac Toe game
│   ├── tic_tac_toe.cc
│   ├── connect_four.h # Connect Four game
│   ├── connect_four.cc
│   ├── game_manager.h # Game management
│   └── game_manager.cc
├── mcts/              # MCTS implementation
│   ├── mcts.h        # MCTS interface
│   └── mcts.cc       # MCTS implementation
├── tests/            # Test files
│   ├── tic_tac_toe_test.cc
│   └── mcts_test.cc
├── main.cc           # Main program
└── CMakeLists.txt    # Build configuration
```

## Building the Project

### Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- Google Test framework
- pthread library

### Build Instructions

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

This will create two executables:
- `game_ai`: The main game executable
- `game_ai_tests`: The test executable

## Running Tests

To run the tests:
```bash
cd build
ctest
```

Or directly:
```bash
./game_ai_tests
```

## Features

- Monte Carlo Tree Search (MCTS) implementation with:
  - Parallel simulation support
  - Move ordering optimization
  - Heuristic evaluation
  - Configurable exploration constant
- Support for multiple games:
  - Tic Tac Toe
  - Connect Four
- Comprehensive test suite
- Thread-safe implementation

## Game Controls

### Tic Tac Toe
- Board positions are numbered 0-8 from left to right, top to bottom
- Players take turns placing X and O
- First player to get 3 in a row wins

### Connect Four
- Columns are numbered 0-6 from left to right
- Players take turns dropping pieces
- First player to get 4 in a row wins
