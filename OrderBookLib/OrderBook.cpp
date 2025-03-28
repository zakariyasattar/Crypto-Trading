/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include <thread>
#include <mutex>

#include "OrderBook.h"
#include "DataIngestion.h"
#include "LockFreeQueue.h"
#include "OrderBookAnalysis.h"
#include "Order.h"
#include "Timer.h"
#include "Enums.h"

using namespace std;

// ANSI escape sequence to clear screen and move cursor to top-left
#define CLEAR_SCREEN "\033[2J\033[1;1H"

void OrderBook::InitData() {
    // Instantiate a thread to init the connection
    std::thread connectionThread { [&]() {
        try {
            // Instantiate OrderBook with Data from API
            DataIngestion engine { *this };

            engine.Connect();
        } 
        catch(const std::exception& e) {
            cerr << "Connection error: " << e.what() << endl;
        }
    }};

    // Begin listening to queue for orders coming in
    std::thread queueListenerThread( [&]() {
        while(true) {
            std::pair<Order, Operation> order { mLockFreeQueue.Pop() };

            // skip iter if order mLockFree.pop() is empty
            if(order.second == Operation::None) {
                continue;
            }

            if(order.second == Operation::Delete) {
                DeletePricePoint(order.first.GetPrice(), order.first.GetSide());
            }
            else if(order.second == Operation::Set) {
                SetPricePoint(order.first.GetPrice(), order.first.GetSize(), order.first.GetSide());
            }

        }

    });

    // detach threads so they run separately from the program
    connectionThread.detach();
    queueListenerThread.detach();
}

void OrderBook::DeletePricePoint(double price, Enums::Side side) {
    if(side == Enums::Side::Buy)
        mBids.erase(price);
    else
        mAsks.erase(price);
}

void OrderBook::SetPricePoint(double price, double size, Enums::Side side) {
    int desiredMapSize { 12 };

    if(side == Enums::Side::Buy) {
        mBids[price] = size;
        Shrink(mBids, desiredMapSize, side);
    }
    else {
        mAsks[price] = size;
        Shrink(mAsks, desiredMapSize, side);
    }

    // Notify cv in main thread if both have at least one entry
    if (!mAsks.empty() && !mBids.empty()) {
        mCv.notify_one();
    }
}

void OrderBook::Shrink(auto& orderMap, int desiredMapSize, Enums::Side side) {
    // keep map at desiredMapSize price points only

    if(side == Enums::Side::Buy) { // delete price point at beginning for bids
        if(orderMap.size() > desiredMapSize) {
            orderMap.erase(orderMap.begin());
        }
    }
    else { // delete price point at end for asks
        if(orderMap.size() > desiredMapSize) {
            orderMap.erase(std::prev(orderMap.end()));
        }
    }
}

double OrderBook::GetCurrentPrice() { 
    Order topAsk { GetTopOrder(mAsks) };
    Order topBid { GetTopOrder(mBids) };

    // return mCurrentPrice.load(std::memory_order_relaxed);
    return (topAsk.GetPrice() + topBid.GetPrice()) / 2;
}

void OrderBook::DisplayOrderBook() {
    // Copy maps to avoid race condition
    // Not an issue (maps r small)
    std::map<double, double> asks = mAsks;
    std::map<double, double, std::greater<double>> bids = mBids;

    // Clear the screen
    std::cout << CLEAR_SCREEN << std::flush;

    // mLockFreeQueue.print();
    
    // Display header
    std::cout << "======== BTC/USD ORDER BOOK ========" << std::endl;
    std::cout << "Updated: " << getCurrentTimestamp() << std::endl << endl;
    cout << "Current Price: " << GetCurrentPrice() << endl; 
    
    // Format column headers
    std::cout << std::left
              << std::setw(10) << "Order Type" << " | "
              << std::right
              << std::setw(12) << "Price" << " | "
              << std::setw(12) << "Size" << std::endl;
    
    std::cout << "---------------------------------------" << std::endl;
    
    auto asksIt { asks.end() };
    asksIt--;

    // Display asks
    for(; asksIt != asks.begin(); asksIt--) {
        auto price { asksIt->first };
        auto size { asksIt->second };

        std::cout << std::left 
                  << std::setw(10) << "[Ask]" << " | "
                  << std::right
                  << std::fixed << std::setprecision(2) << std::setw(12) << price << " | "
                  << std::fixed << std::setprecision(6) << std::setw(12) << size << std::endl;
    }
    
    // Display bids
    for(const auto& [price, size]: bids) {
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

Enums::Analysis OrderBook::AnalyzeOrderBook() {
    OrderBookAnalysis analysisEngine { *this };

    Enums::Analysis analysis { analysisEngine.AnalyzeOrderBook() };
    return analysis;
}