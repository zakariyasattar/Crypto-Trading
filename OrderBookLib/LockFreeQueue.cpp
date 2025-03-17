#include <iostream>
#include "Order.h"
#include "LockFreeQueue.h"

using namespace std;

// mutextes lock and unlock entire structure, which is slow
// also could lead to deadlocks
// better to use std::atomic

void LockFreeQueue::Push(const Order& order) {

}