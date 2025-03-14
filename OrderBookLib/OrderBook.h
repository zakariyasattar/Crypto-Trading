/**
 * @file OrderBook.h
 * @author Zakariya Sattar
 *
 * Store and Display OrderBook
 */

#pragma once

#include "Bid.h"
#include "Ask.h"
#include "Order.h"

#include <vector>
#include <map>
#include <memory>

class OrderBook {

private:
    // key: price, value: shares
    std::map<double, double> mBids;
    std::map<double, double> mAsks;

public:

    // Call initData() on class construction
    OrderBook() { initData(); };

    // Pull data from source and populate member vectors
    void initData();

    // Print out OrderBook
    void DisplayOrderBook();

};

