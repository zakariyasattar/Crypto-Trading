/**
 * @file main.cpp
 * @author Zakariya Sattar
 *
 * Initialize Application
 */

#include <iostream>
#include <mutex>
#include <thread>
#include <csignal>

#include "OrderBookLib/OrderBook.h"
#include "TradingAlgoLib/TradingAlgo.h"

#include "OrderBookLib/LockFreeQueue.h"
#include "OrderBookLib/HazardPointerOwner.h"

using namespace std;

TradingAlgo* gAlgo = nullptr;

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
        gAlgo = &algo;

        std::signal(SIGINT, [](int) {
            std::cout << "\nShutting down...\n";
            if(gAlgo) gAlgo->~TradingAlgo();

            std::exit(0);
        });

        algo.StartTrading();
    }


    return 0;
}