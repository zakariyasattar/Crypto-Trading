#include "MutexQueue.h"

void MutexQueue::Push(const Order& order, const Operation& operation) {
    std::lock_guard<std::mutex> lock(mMtx);

    mOrderQueue.push(Node(order, operation));
}

std::pair<Order, Operation> MutexQueue::Pop() {
    std::lock_guard<std::mutex> lock(mMtx);

    if(!mOrderQueue.empty()) {
        Node curr { mOrderQueue.front() };
        mOrderQueue.pop();

        return {curr.order, curr.operation};
    }

    return {};
}

void MutexQueue::print() {
    std::queue<Node> qCopy;
    {
        std::lock_guard<std::mutex> lock(mMtx);
        qCopy = mOrderQueue;
    }

    while(!qCopy.empty()) {
        Node curr { qCopy.front() };
        qCopy.pop();

        std::cout << curr.order << std::endl;
    }
}