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
    const string api_key { "PKWTH6N3SM1XALKB870E" };
    const string api_secret { "sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK" };

    DataIngestion di { api_key, api_secret };
    di.populate(mBids, mAsks);

    DisplayOrderBook();
}

void OrderBook::DisplayOrderBook() {
    for(const auto& [k, v]: mAsks) {
        cout << k << ": " << v << endl;
    }
    
    for(const auto& [k, v]: mAsks) {
        cout << k << ": " << v << endl;
    }
}