#include "MutexQueue.h"

void MutexQueue::Push(const Order& order, const Operation& operation) {
    std::lock_guard<std::mutex> lock(mMtx);

    mOrderQueue.push(MtxNode(order, operation));
}

std::pair<Order, Operation> MutexQueue::Pop() {
    std::lock_guard<std::mutex> lock(mMtx);

    if(!mOrderQueue.empty()) {
        MtxNode curr { mOrderQueue.front() };
        mOrderQueue.pop();

        return {curr.order, curr.operation};
    }

    return {};
}

void MutexQueue::print() {
    std::queue<MtxNode> qCopy;
    {
        std::lock_guard<std::mutex> lock(mMtx);
        qCopy = mOrderQueue;
    }

    while(!qCopy.empty()) {
        MtxNode curr { qCopy.front() };
        qCopy.pop();

        std::cout << curr.order << std::endl;
    }
}