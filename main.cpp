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

#include "OrderBookLib/LockFreeQueue.h"

using namespace std;

int main() {
    LockFreeQueue q {};
    Order order {15.5, 3.0, Side::Ask};
    Order order1 {2.0, 2.0, Side::Ask};

    q.Push(order, Operation::None);
    q.Push(order1, Operation::None);

    q.print();

    cout << q.Pop().first << endl;

    // std::condition_variable cv {};
    // std::mutex mtx {};

    // OrderBook orderBook {mtx, cv};
    // orderBook.InitData();

    // // Wait for OrderBook to InitData before running trading algo
    // {
    //     std::unique_lock<std::mutex> lock (mtx);

    //     // Re-check orderBook.isEmpty() because of spurious wake
    //     cv.wait(lock, [&]() {
    //         return !orderBook.isEmpty();
    //     });

    //     TradingAlgo algo { orderBook };
    //     algo.StartTrading();
    // }


    return 0;
}