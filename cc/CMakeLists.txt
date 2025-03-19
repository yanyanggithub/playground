cmake_minimum_required(VERSION 3.10)
project(GameAI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)

# Add source files
set(SOURCES
    main.cc
    games/game_manager.cc
    games/tic_tac_toe.cc
    games/connect_four.cc
    mcts/mcts.cc
)

# Add test files
set(TEST_SOURCES
    tests/test_main.cc
    tests/tic_tac_toe_test.cc
    tests/mcts_test.cc
    games/tic_tac_toe.cc
    games/connect_four.cc
    mcts/mcts.cc
)

# Create main executable
add_executable(game_ai ${SOURCES})

# Create test executable
add_executable(game_ai_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(game_ai PRIVATE pthread)
target_link_libraries(game_ai_tests PRIVATE 
    pthread
    GTest::GTest
    GTest::Main
)

# Include directories
target_include_directories(game_ai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(game_ai_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add test
add_test(NAME game_ai_tests COMMAND game_ai_tests)
