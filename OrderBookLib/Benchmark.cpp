#include "Benchmark.h"
#include "LockFreeQueue.h"
#include "MutexQueue.h"
#include "Order.h"

#include <iomanip>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

struct BenchmarkConfig {
    int numThreads;
    double readWriteRatio;
};

// current config:
// one thread pulling in updates, and one thread processing said updates

template <typename T>
double RunBenchmark(T& queue) {
    int numIterations { 100000000 };

    auto start { std::chrono::high_resolution_clock::now() };

    vector<thread> threads;

    for(int i {}; i < 16; i++) {
        threads.emplace_back(thread{[&]() {
            int iter { numIterations };
    
            while(iter-- > 0) {
                Order newOrder { Order{12, 12, Side::None} };
                queue.Push(newOrder, Operation::None);
            }
        }});
    }

    for(int i {}; i < 16; i++) {
        threads.emplace_back(thread{[&, numIterations]() {
            int iter { numIterations };

            while(iter-- > 0) {
                queue.Pop();
            }
        }});
    }

    for(auto& t : threads) {
        t.join();
    }

    auto end { std::chrono::high_resolution_clock::now() };
    
    double durationSeconds = std::chrono::duration<double>(end - start).count();
    return static_cast<double>(numIterations * 32) / durationSeconds; // ops/sec
}

// Goal: measure operations / sec

int main() {
    LockFreeQueue lockFreeQueue;
    MutexQueue mutexQueue;

    std::cout << "=== Queue Performance Benchmark ===" << std::endl;
    std::cout << std::left << std::setw(18) << "Queue Type" 
          << std::setw(18) << "Throughput (ops/sec)" << std::endl;
    
    // Run mutex queue benchmark
    auto mutexResult = RunBenchmark(mutexQueue);
    
    // Run lock-free queue benchmark
    auto lockFreeResult = RunBenchmark(lockFreeQueue);
    
    // Print results
    std::cout << std::left
                << std::setw(18) << "Mutex"
                << std::setw(18) << std::fixed << std::setprecision(2) << mutexResult << std::endl;
                
    std::cout << std::left
                << std::setw(18) << "Lock-Free"
                << std::setw(18) << std::fixed << std::setprecision(2) << lockFreeResult << std::endl;
    
    // Calculate and print improvement
    double throughputImprovement = (lockFreeResult / mutexResult - 1.0) * 100;
    
    std::cout << std::left << std::setw(30) << "Improvement (%)" 
                << std::setw(18) << std::fixed << std::setprecision(2) << throughputImprovement << std::endl;
    
    std::cout << std::string(84, '-') << std::endl;
}