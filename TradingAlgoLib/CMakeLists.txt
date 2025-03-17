project(TradingAlgoLib)

set(SOURCE_FILES
    TradingAlgo.cpp
)

add_library(TradingAlgoLib STATIC ${SOURCE_FILES})

# Link Libraries
target_link_libraries(TradingAlgoLib)
