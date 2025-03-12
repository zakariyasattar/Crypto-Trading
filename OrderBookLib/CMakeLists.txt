# Define the library project
project(OrderBookLib)

# Set the source files (only .cpp files should be included here)
set(SOURCE_FILES
    Order.cpp
    OrderBook.cpp
    DataIngestion.cpp
    Ask.cpp
    Bid.cpp
)

# Find CURL (libcurl)
find_package(CURL REQUIRED)

# Manually set cURLpp paths (change if installed elsewhere)
set(CURLPP_INCLUDE_DIR /opt/homebrew/include)  # Adjust path as needed
set(CURLPP_LIBRARY /opt/homebrew/lib/libcurlpp.dylib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CURLPP_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

# Create a static library
add_library(OrderBookLib STATIC ${SOURCE_FILES})

# Link Libraries (Ensure linking of cURL and cURLpp)
target_link_libraries(OrderBookLib ${CURLPP_LIBRARY} ${CURL_LIBRARIES})
