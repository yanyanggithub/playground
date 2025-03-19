cmake_minimum_required(VERSION 3.10)
project(mcts_games)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    main.cc
    games/tictactoe.cc
    mcts/mcts.cc
)

# Add header files
set(HEADERS
    games/game.h
    games/tictactoe.h
    mcts/mcts.h
)

# Create executable
add_executable(mcts_games ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(mcts_games PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Enable threading
find_package(Threads REQUIRED)
target_link_libraries(mcts_games PRIVATE Threads::Threads)
