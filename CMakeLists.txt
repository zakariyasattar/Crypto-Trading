cmake_minimum_required(VERSION 3.10)

# Project Name
project(AlgorithmicTradingSystem)

# Specify C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Directories
include_directories(include)

# Source Files
file(GLOB_RECURSE SOURCES "TradingAlgoLib/*.cpp")
file(GLOB_RECURSE SOURCES "OrderBookLib/*.cpp")
file(GLOB_RECURSE SOURCES "main.cpp")

# Executable
add_executable(trading_system ${SOURCES})

# Link Libraries (if any)
# target_link_libraries(trading_system PRIVATE some_library)
