/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <stdlib.h>

#include "OrderBook.h"
#include "Order.h"
#include "DataIngestion.h"

using namespace std;

/*
 * OrderBook will have a vector of bids, a vector of asks
 * 
 *
*/

void OrderBook::initData() {
    DataIngestion di {};
    di.populate(bids, asks);
}

void OrderBook::DisplayOrderBook() {
    for(const Order& ask : asks) {
        cout << ask << endl;
    }

    // for(const Order& bid : bids) {
    //     cout << bid << endl;
    // }
}