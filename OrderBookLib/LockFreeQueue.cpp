#include <iostream>
#include <thread>

#include "Order.h"
#include "LockFreeQueue.h"
#include "HazardPointerOwner.h"

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
                    mSize.fetch_add(1, memory_order_relaxed);
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
    HazardPointerOwner hazardPointerOwner { HazardPointerOwner() };

    while(true) {
        // head never gets deleted
        Node* head { mHead.load() };

        Node* next { head->next }; // The target node we want to pop

        // If list is empty
        // We check next because head always will point to -1 (to keep list connected)
        // The real 'head' will always be head->next
        if(next == nullptr) {
            mTail.store(mHead); // Point tail back to dummy node

            return make_pair(Order(), Operation::None);
        }
        
        Node* next_next { next->next }; // The node we want to put in its place

        if(head == mHead.load()) {
            // Protect next two nodes, because we need to use it still. Head never gets deleted
            hazardPointerOwner.Protect(next, next_next);

            // if list is full
            if(mHead.load()->next.compare_exchange_weak(next, next_next)) {
                pair res {make_pair(next->order, next->operation)}; 

                hazardPointerOwner.Retire();

                // Try to reclaim all retired nodes
                hazardPointerOwner.TryReclaim();

                mSize.fetch_sub(1, memory_order_relaxed);
                
                return res;
            }
        }

        std::this_thread::yield();
    }
}

void LockFreeQueue::print() {
    Node* curr = mHead.load();

    while (curr != nullptr) {
        if(curr->next == nullptr) std::cout << curr << ": " << curr->order;
        else std::cout << curr << ": " << curr->order << " -> ";
        curr = curr->next;
    }

    cout << endl;
}
