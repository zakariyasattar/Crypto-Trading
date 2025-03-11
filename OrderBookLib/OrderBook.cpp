/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <stdlib.h>

#include "OrderBook.h"
#include "Order.h"

using namespace std;

/*
 * OrderBook will have a vector of bids, a vector of asks
 * 
 *
*/


void OrderBook::DisplayOrderBook() {
    for(int i {}; i < 10; i++) {
        bids.emplace_back(Order {1, 2, true});
        asks.emplace_back(Order {1, 3, false});
    }

    cout << asks[asks.size() - 1] << endl;

    // for(const Order& ask : asks) {
    //     cout << ask << endl;
    // }

    // for(const Order& bid : bids) {
    //     cout << bid << endl;
    // }
}