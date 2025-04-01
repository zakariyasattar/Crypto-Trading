#include "Benchmark.h"
#include "LockFreeQueue.h"
#include "MutexQueue.h"

#include <vector>

using namespace std;

void Benchmark::RunBenchmark(int load, int iterations) {
    vector<double> add_times;
    vector<double> remove_times;
    vector<double> match_times;
    
    while(iterations-- > 0) {

    }
}