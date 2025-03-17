/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "OrderBook.h"
#include "DataIngestion.h"
#include "Timer.h"

using namespace std;

// ANSI escape sequence to clear screen and move cursor to top-left
#define CLEAR_SCREEN "\033[2J\033[1;1H"

void OrderBook::initData() {
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
    
    auto asksIt { mAsks.end() };
    asksIt--;

    // Display asks
    for(; asksIt != mAsks.begin(); asksIt--) {
        auto price { asksIt->first };
        auto size { asksIt->second };

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

    std::cout << std::flush;
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

// function that will analyze OrderBook and will return whether to buy or sell
// and at what price to do so. Also stop-loss price
OrderBook::TradeDecision OrderBook::AnalyzeOrderBook() {
    // 3 strategies:
    //  1. VWAP Deviation
    //  2. Current Price near to top Bid/Ask
    //  3. Order Book Imbalance

    // each strat can return a prob of success,
    // whichever has the highest prob should 
    // be executed (converted to an TradeDecision)

    double VWAPDevProb { CalcVWAPDev() };

    return {Side::Buy, 0.0, 0.0};
}


// Calculates VWAP deviation within order book
// returns probability of trade success
double OrderBook::CalcVWAPDev() {
    // double bidVWAP { CalcVWAP(mBids) };
    // double askVWAP { CalcVWAP(mAsks) };

    // double bidSideDeviation { bidVWAP };

    return 0.0;
}

double OrderBook::CalcVWAP() {
    // for()
    return 0.0;
}