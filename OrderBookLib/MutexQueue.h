
#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

#include "Enums.h"
#include "Order.h"

using Operation = Enums::Operation;

struct Node {
    Order order;
    Operation operation;
};

class MutexQueue{
private:
    std::queue<Node> mOrderQueue;
    std::mutex mMtx;
    std::condition_variable mCv;

public:
    void Push(const Order& order, const Operation& operation);
    std::pair<Order, Operation> Pop();

    bool empty() { return mOrderQueue.size() == 0; }

    int size() { return mOrderQueue.size(); }

    void print();
};