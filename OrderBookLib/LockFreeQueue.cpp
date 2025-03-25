#include <iostream>
#include <thread>

#include "Order.h"
#include "LockFreeQueue.h"
// #include "HazardPointer.h"

using namespace std;

// mutextes lock and unlock entire structure, which is slow
// also could lead to deadlocks
// better to use std::atomic

LockFreeQueue::LockFreeQueue() {
    // Init dummy node
    Node* dummy { new Node{Order(-1, -1, Side::None), Operation::None, nullptr} };

    // Set head and tail to dummy node
    mHead.store(dummy);
    mTail.store(dummy);
}

void LockFreeQueue::Push(const Order& order, const Operation& operation) {
    Node* node { new Node{order, operation, nullptr} };

    while(true) {
        Node* tail { mTail.load() };
        Node* next { tail->next };

        if(tail == mTail.load()) {
            if(next == nullptr) {
                Node* expected = nullptr;

                 // mTail will become node if compare_exchange_weak succeeds
                if(tail->next.compare_exchange_weak(expected, node)) {
                    mTail.compare_exchange_strong(tail, node);
                    break;
                }
            }
        }

        // Allow other threads to run if compare_exchange_weak continues to fail
        // Let OS handle scheduling
        std::this_thread::yield();
    }
}

std::pair<Order, Operation> LockFreeQueue::Pop() {
    Node* node;

    while(true) {
        Node* head { mHead.load() };
        Node* next { head->next };

        if (head == mHead.load()) {
            // Only one elem left
            if(next == nullptr) {
                // extract val, and set initial node to nullptr
                node = head;
                mHead.store(nullptr);

                break;
            }

            // If next is what we think it is, move head to next
            if(next == head->next) {
                mHead.compare_exchange_strong(head, next);

                // Delete old head
                delete head;

                node = mHead.load();
                break;
            }
        }

        std::this_thread::yield();
    }

    std::pair<Order, Operation> res {node->order, node->operation};

    return res;
}

void LockFreeQueue::print() {
    Node* curr = mHead.load();

    while (curr != nullptr) {
        if(curr->next == nullptr) std::cout << curr->order;
        else std::cout << curr->order << " -> ";
        curr = curr->next;
    }

    cout << endl;
}
