cmake_minimum_required(VERSION 3.10)
project(mcts_games)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cc
    games/connect_four.cc
    games/tic_tac_toe.cc
    games/game_manager.cc
    mcts/mcts.cc
)

# Header files
set(HEADERS
    games/game.h
    games/connect_four.h
    games/tic_tac_toe.h
    games/game_manager.h
    mcts/mcts.h
)

# Create executable
add_executable(mcts_games ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(mcts_games PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Find and link threading library
find_package(Threads REQUIRED)
target_link_libraries(mcts_games PRIVATE Threads::Threads)
