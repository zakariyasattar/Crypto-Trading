#pragma once

#include <stdlib.h>
#include <array>
#include <vector>
#include <atomic>
#include <thread>

// Forward declaration for Node struct from LockFreeQueue
struct Node;

// Each individial hazard pointer will have:
//  a thread its associated with,
//  a pointer thats currently in use
struct HazardPointer {
    std::atomic<std::thread::id> id;
    std::atomic<Node*> ptr;
    std::atomic<Node*> nextPtr;
};

// main functions of hazard pointer owner:
//  a. store pointer
//  b. search all HPs to check if ptr is in use
//  c. retrive hazard_pointer for a thread
//  d. assign an array spot to a thread (on construction)

const int max_threads { 100 };

class HazardPointerOwner {
private:
    std::array<HazardPointer, max_threads> HPList;

    // use vector for cache locality (no pointer chasing)
    std::vector<Node*> retiredNodes;

    // Thread Local static var for hp for each thread
    thread_local static HazardPointer* hp;

public:

    HazardPointerOwner();

    // store pointer
    void Protect(Node* ptr, Node* nextPtr);

    // search all HPs to check if ptr is in use
    bool IsDeleteSafe(Node* ptr);

    // retrieve hazard_pointer for a thread
    HazardPointer* GetHazardPointer();

    // periodically check if threads in RetireList can be deleted
    void TryReclaim();
    
    // remove protection and mark for deletion
    void Retire();
};