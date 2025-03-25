#pragma once

#include <stdlib.h>
#include <array>
#include <atomic>
#include <thread>

// Each individial hazard pointer will have:
//  a thread its associated with,
//  a pointer thats currently in use
struct HP {
    std::atomic<std::thread::id> id;
    std::atomic<void*> ptr;
};

// main functions of hazard pointer:
//  a. store pointer
//  b. search all HPs to check if ptr is in use
//  c. retrive hazard_pointer for a thread
//  d. assign an array spot to a thread (on construction)

const int max_threads { 100 };

class HazardPointer {
private:
    std::array<HP, max_threads> HPList;

public:

    HazardPointer(std::thread::id);

    // store pointer
    void protect(std::thread::id id, void* ptr);

    // search all HPs to check if ptr is in use

};