project(TradingAlgoLib)

set(SOURCE_FILES
    TradingAlgo.cpp
    TradeExecution.cpp
)

# Find CURL (libcurl)
find_package(CURL REQUIRED)

# Manually set cURLpp paths
set(CURLPP_INCLUDE_DIR /opt/homebrew/include)
set(CURLPP_LIBRARY /opt/homebrew/lib/libcurlpp.dylib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CURLPP_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

add_library(TradingAlgoLib STATIC ${SOURCE_FILES})

# Link Libraries
target_link_libraries(TradingAlgoLib ${CURLPP_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
