# Define the library project
project(OrderBookLib)

# Set the source files (only .cpp files should be included here)
set(SOURCE_FILES
    Order.cpp
    OrderBook.cpp
    DataIngestion.cpp
)

# Find CURL (libcurl)
find_package(CURL REQUIRED)

# Manually set cURLpp paths (change if installed elsewhere)
set(CURLPP_INCLUDE_DIR /usr/local/include)
set(CURLPP_LIBRARY /usr/local/lib/libcurlpp.dylib)

# Include directories
include_directories(${CURLPP_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

# Create a static library
add_library(OrderBookLib STATIC ${SOURCE_FILES})

# Ensure the headers are visible to projects that link to this library
target_include_directories(OrderBookLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
