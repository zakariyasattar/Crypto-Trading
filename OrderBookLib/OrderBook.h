/**
 * @file OrderBook.h
 * @author Zakariya Sattar
 *
 * Store and Display OrderBook
 */

#pragma once

#include "Order.h"
#include <vector>

class OrderBook {

private:
    std::vector<Order> bids;
    std::vector<Order> asks;

public:
    // Print out OrderBook
    void DisplayOrderBook();

};

