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
#include <memory>

class OrderBook {

private:
    std::vector<std::unique_ptr<Order>> mBids;
    std::vector<std::unique_ptr<Order>> mAsks;

public:

    // Call initData() on class construction
    OrderBook() { initData(); };

    // Pull data from source and populate member vectors
    void initData();

    // Print out OrderBook
    void DisplayOrderBook();

};

