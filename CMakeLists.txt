cmake_minimum_required(VERSION 3.10)

# Project Name
project(AlgorithmicTradingSystem)

# Specify C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Order Book Library
set(ORDER_BOOK_LIBRARY OrderBookLib)
add_subdirectory(${ORDER_BOOK_LIBRARY})
include_directories(${ORDER_BOOK_LIBRARY})  # Ensure headers are accessible

# Add Source Files
file(GLOB_RECURSE SOURCES "main.cpp")

# Create Executable
add_executable(trading_system ${SOURCES})

# Link Order Book Library to Executable
target_link_libraries(trading_system PRIVATE OrderBookLib)
