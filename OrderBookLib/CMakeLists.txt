# Define the library project
project(OrderBookLib)

# Set the source files
set(SOURCE_FILES
    OrderBook.cpp
    DataIngestion.cpp
    LockFreeQueue.cpp
    Order.cpp
    HazardPointerOwner
)

# Find CURL (libcurl)
find_package(CURL REQUIRED)

# Manually set cURLpp paths
set(CURLPP_INCLUDE_DIR /opt/homebrew/include)
set(CURLPP_LIBRARY /opt/homebrew/lib/libcurlpp.dylib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CURLPP_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

# Find
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
set(WEBSOCKETPP_INCLUDE_DIR /opt/homebrew/include)

# Include  headers
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${WEBSOCKETPP_INCLUDE_DIR})

# Create a static library
add_library(OrderBookLib STATIC ${SOURCE_FILES})

# Link Libraries
target_link_libraries(OrderBookLib ${CURLPP_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
