/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "OrderBook.h"
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
    std::cout << CLEAR_SCREEN << std::flush;
    
    // Display header
    std::cout << "======== BTC/USD ORDER BOOK ========" << std::endl;
    std::cout << "Updated: " << getCurrentTimestamp() << std::endl << endl;
    
    // Format column headers
    std::cout << std::left
              << std::setw(10) << "Order Type" << " | "
              << std::right
              << std::setw(12) << "Price" << " | "
              << std::setw(12) << "Size" << std::endl;
    
    std::cout << "---------------------------------------" << std::endl;
    
    // Display asks
    for(const auto& [price, size]: mAsks) {
        std::cout << std::left 
                  << std::setw(10) << "[Ask]" << " | "
                  << std::right
                  << std::fixed << std::setprecision(2) << std::setw(12) << price << " | "
                  << std::fixed << std::setprecision(6) << std::setw(12) << size << std::endl;
    }
    
    // Display bids
    for(const auto& [price, size]: mBids) {
        std::cout << std::left 
                  << std::setw(10) << "[BID]" << " | "
                  << std::right
                  << std::fixed << std::setprecision(2) << std::setw(12) << price << " | "
                  << std::fixed << std::setprecision(6) << std::setw(12) << size << std::endl;
    }
}

// Helper function to get current timestamp
string OrderBook::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time);

    // Remove newline character
    timestamp.pop_back();
    return timestamp;
}