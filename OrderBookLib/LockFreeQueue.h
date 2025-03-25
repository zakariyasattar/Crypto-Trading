
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

public:
    LockFreeQueue();
    void Push(const Order& order, const Operation& operation);
    std::pair<Order, Operation> Pop();

    ~LockFreeQueue() {
        while(Node* node = mHead.load()) {
            mHead.store(node->next);
            delete node;
        }
    }

    void print();
};