cmake_minimum_required(VERSION 3.10)

project(MCTS)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # Ensure C++14 is enabled

# Add executable
add_executable(mcts main.cc)
