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
    // Ingest data from Alpaca
    DataIngestion di {};
    di.populate(mBids, mAsks);

    DisplayOrderBook();
}

void OrderBook::DisplayOrderBook() {
    for(const auto& ask : mAsks) {
        cout << *ask << endl;
    }
    
    for(const auto& bid : mBids) {
        cout << *bid << endl;
    }
}