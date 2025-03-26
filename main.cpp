/**
 * @file main.cpp
 * @author Zakariya Sattar
 *
 * Initialize Application
 */

#include <iostream>
#include <mutex>
#include <thread>

#include "OrderBookLib/OrderBook.h"
#include "TradingAlgoLib/TradingAlgo.h"

#include "OrderBookLib/LockFreeQueue.h"
#include "OrderBookLib/HazardPointerOwner.h"

using namespace std;

int main() {
    // LockFreeQueue lfq;

    // Order order {1.0, 1.0, Side::Buy};

    // lfq.Push(order, Operation::Delete);
    // lfq.Push(order, Operation::Delete);
    // lfq.Push(order, Operation::Delete);

    // lfq.print();

    // lfq.Pop();
    // lfq.Pop();
    // lfq.Pop();

    // lfq.Pop();
    // lfq.Pop();

    // lfq.print();

    // lfq.Push(order, Operation::Delete);
    // lfq.Push(order, Operation::Delete);
    // lfq.Push(order, Operation::Delete);
    // lfq.Push(order, Operation::Delete);

    // lfq.print();
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