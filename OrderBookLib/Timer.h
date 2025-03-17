#include <chrono>
#include <iostream>
#include <vector>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string label;

public:
    Timer(const std::string& operation_label) : label(operation_label) {
        start_time = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        std::cout << label << ": " << duration << " microseconds" << std::endl;
    }
};