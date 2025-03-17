#include <stdlib.h>

class Benchmark {
private:
    struct BenchmarkResults {
        double avg_add_time_us;
        double avg_remove_time_us;
        double avg_match_time_us;
        double throughput_ops_per_sec;
        double latency_p99_us;  // 99th percentile latency
    };
public:
    void RunBenchmark(int load, int iterations);
};