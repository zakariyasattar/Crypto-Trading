/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "OrderBook.h"
#include "Order.h"
#include "DataIngestion.h"

using namespace std;

// ANSI escape sequence to clear screen and move cursor to top-left
#define CLEAR_SCREEN "\033[2J\033[1;1H"

void OrderBook::initData() {
    // Ingest data from Alpaca

    // Instantiate OrderBook with Data from API
    DataIngestion engine { *this };

    // Connect to webhook and display data
    engine.Connect();
}

void OrderBook::DisplayOrderBook() {
    // Clear the screen
    std::cout << CLEAR_SCREEN;
    
    for(const auto& [k, v]: mAsks) {
        cout << "[Ask] " << k << ": " << v << endl;
    }

    // double bid_ask_spread { (mAsks.begin()->first - mBids.begin()->first) / mAsks.begin()->first };
    // cout << bid_ask_spread << endl;
    
    for(const auto& [k, v]: mBids) {
        cout << "[BID] " << k << ": " << v << endl;
    }
}