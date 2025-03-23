#include <mutex>
#include <thread>
#include <condition_variable>
#include <stdlib.h>
#include <atomic>
#include <future>
#include <vector>
#include <iostream>
#include <iterator>

std::atomic<bool> found;

std::vector<int>::iterator find(std::vector<int>& vec, int target, int l, int r) {
    auto start { vec.begin() + l };

    for(int i {l}; i <= r; i++) {
        if(found.load() == true) return vec.end();

        if(*start == target) {
            found.store(true, std::memory_order_relaxed);

            return start;
        }
        start++;
    }

    return vec.end();
}

std::vector<int>::iterator parallel_find(std::vector<int>& vec, int target) {
    found.store(false);

    int numThreads { 4 };
    std::vector<std::future<std::vector<int>::iterator>> threads;
    threads.reserve(numThreads);

    int elemsPerThread { static_cast<int>(vec.size()) / numThreads };

    for(int i {}; i < numThreads; i++) {
        int start { i * elemsPerThread };
        int end { std::min(start + elemsPerThread, static_cast<int>(vec.size()) - 1) };

        threads.emplace_back(std::async(std::launch::async, find, std::ref(vec), target, start, end));
    }

    for(auto& future : threads) {
        auto it { future.get() };

        if(it != vec.end()) return it;
    }

    return vec.end();
}

int main() {
    std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8};
    auto it { parallel_find(vec, 8) };

    std::cout << *it << std::endl;
    return 0;
}