/**
 * @file main.cpp
 * @author Zakariya Sattar
 *
 * Initialize Application
 */

#include <iostream>
#include <mutex>

#include "OrderBookLib/OrderBook.h"
#include "TradingAlgoLib/TradingAlgo.h"

using namespace std;

int main() {
    std::condition_variable cv {};
    std::mutex mtx {};

    OrderBook orderBook {mtx, cv};
    orderBook.InitData();

    // Wait for OrderBook to InitData before running trading algo
    {
        std::unique_lock<std::mutex> lock (mtx);

        // Re-check orderBook.isEmpty() because of spurious wake
        cv.wait(lock, [&]() {
            return !orderBook.isEmpty();
        });

        TradingAlgo algo { orderBook };
        algo.StartTrading();
    }


    return 0;
}