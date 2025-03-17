/**
 * @file main.cpp
 * @author Zakariya Sattar
 *
 * Initialize Application
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "OrderBookLib/OrderBook.h"
#include "TradingAlgoLib/TradingAlgo.h"

using namespace std;

int main() {
    mutex mtx{};
    condition_variable cv {};
    bool dataInitialized {};

    OrderBook orderBook {};

    thread wsThread { [&]() {
        // Give OrderBook WebSocket its own separate thread
        orderBook.initData();
        
        { // wrap unique_lock in braces to force scope exit
            std::unique_lock<std::mutex> lock(mtx); // std::unique_lock auto unlocks mutex once it exits scope
            dataInitialized = true;
        }

        cv.notify_one();
    } };

    // Allows thread to run separately outside of main thread
    wsThread.detach();

    {
        std::unique_lock<std::mutex> lock(mtx);

        if(!dataInitialized) {
            cv.wait_for(lock, std::chrono::seconds(2), [&] {
                return dataInitialized;
            });
        }
    }

    TradingAlgo algo { orderBook };
    algo.StartTrading();

    return 0;
}