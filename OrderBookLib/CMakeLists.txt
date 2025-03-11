# Define the library project
project(OrderBookLib)

# Set the source files (only .cpp files should be included here)
set(SOURCE_FILES
    Order.cpp
    OrderBook.cpp
)

# Create a static library
add_library(OrderBookLib STATIC ${SOURCE_FILES})

# Ensure the headers are visible to projects that link to this library
target_include_directories(OrderBookLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
