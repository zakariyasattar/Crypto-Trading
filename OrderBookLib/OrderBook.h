/**
 * @file OrderBook.h
 * @author Zakariya Sattar
 *
 * Store and Display OrderBook
 */

#pragma once

#include <vector>
#include <map>
#include <memory>

class OrderBook {

private:
    // key: price, value: shares
    std::map<double, double, std::greater<double>> mBids;
    std::map<double, double> mAsks;

public:

    // Call initData() on class construction
    OrderBook() { initData(); };

    // Pull data from source and populate member vectors
    void initData();

    // Print out OrderBook
    void DisplayOrderBook();

    auto& GetAsks() { return mAsks; }
    auto& GetBids() { return mBids; }

    // return current timestamp
    std::string getCurrentTimestamp();
};

