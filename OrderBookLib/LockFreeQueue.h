
#pragma once

#include "Order.h"
// #include "HazardPointer.h"

#include <queue>
#include <memory>
#include <condition_variable>
#include "Enums.h"

// OrderQueue
// All operations get placed in Queue, add, remove, modify, delete
// get handled atomically

using Operation = Enums::Operation;

struct Node {
    Order order;
    Operation operation;
    std::atomic<Node*> next;
};

class LockFreeQueue {
private:
    std::atomic<Node*> mHead;
    std::atomic<Node*> mTail;

    std::atomic<int> mSize;

public:
    LockFreeQueue();
    void Push(const Order& order, const Operation& operation);
    std::pair<Order, Operation> Pop();

    bool empty() {
        // Does tail point to dummy order
        return mTail.load()->order.GetPrice() == -1;
    }

    int size() {
        return mSize.load();
    }

    ~LockFreeQueue() {
        while(Node* node = mHead.load()) {
            mHead.store(node->next);
            delete node;
        }
    }

    void print();
};