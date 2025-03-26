
#include <thread>

#include "HazardPointerOwner.h"
#include "LockFreeQueue.h"

// Thread Local static var for hp for each thread
thread_local HazardPointer* HazardPointerOwner::hp = nullptr;

// Construct Hazard Pointer object 
HazardPointerOwner::HazardPointerOwner() {
    std::thread::id id = std::this_thread::get_id();
    size_t i {};

    while(i < HPList.size()) {
        std::thread::id blankID {};

        if(HPList[i].id.compare_exchange_strong(blankID, id)) {
            HPList[i].ptr.store(nullptr);
            HPList[i].nextPtr.store(nullptr);

            hp = &HPList[i];
            break;
        }

        std::this_thread::yield();
    }
}

void HazardPointerOwner::Protect(Node* ptr, Node* nextPtr) {
    hp->ptr.store(ptr);
    hp->nextPtr.store(ptr);
}

bool HazardPointerOwner::IsDeleteSafe(Node* ptr) {
    for(const HazardPointer& node : HPList) {
        if(node.ptr.load(std::memory_order_acquire) == ptr || node.nextPtr.load(std::memory_order_acquire) == ptr) {
            return false;
        }
    }

    return true;
}

HazardPointer* HazardPointerOwner::GetHazardPointer() {
    return hp;
}

void HazardPointerOwner::TryReclaim() {
    auto it { retiredNodes.begin() };

    // Iter through entire list of retired nodes,
    // Check if we can delete
    while(it != retiredNodes.end()) {
        if(IsDeleteSafe(*it)) {
            delete static_cast<Node*>(*it);
            it = retiredNodes.erase(it);
        }
        else it++;
    }
}

void HazardPointerOwner::Retire() {
    Node* ptr { hp->ptr.load() };

    hp->ptr.store(nullptr);
    hp->nextPtr.store(nullptr);

    // Only delete the current node
    retiredNodes.push_back(ptr);
}