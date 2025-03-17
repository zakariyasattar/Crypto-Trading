
#include "Order.h"
#include <queue>
#include <memory>
#include <condition_variable>

// OrderQueue
// All operations get placed in Queue, add, remove, modify, delete
// get handled atomically

struct Node {
    Order order;
    std::unique_ptr<Node> next;
};

class LockFreeQueue {
private:
    std::atomic<std::unique_ptr<Node>> mHead;
    std::atomic<std::unique_ptr<Node>> mTail;

public:
    LockFreeQueue() {
       Node dummy { Order(), nullptr };

       mHead.store(std::make_unique<Node>(dummy));
       mTail.store(std::make_unique<Node>(dummy));
    }

    void Push(const Order& order);
    Order Pull();
};