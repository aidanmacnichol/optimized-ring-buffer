#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstring>
#include <pthread.h>
#include <iomanip>

// Implementations
#include "ringbuffer_aligned.hpp"
#include "ringbuffer_shadowed.hpp"

// CONFIG
const long long ITERATIONS = 1e8;
const int BUFFER_SIZE = 4096;

void pin_thread(int core_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_t current_thread = pthread_self();
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

template <typename BufferType>
void run_benchmark(const std::string &name)
{
    std::cout << "Running: " << name << "..." << std::endl;

    BufferType buffer;
    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&]()
                         {
        pin_thread(1);
        for (int i = 0; i < ITERATIONS; ++i) {
            while (!buffer.push(i)) {
                // spinlock
            }
        } });

    std::thread consumer([&]()
                         {
        pin_thread(2);
        int val;
        for (int i = 0; i < ITERATIONS; ++i) {
            while (!buffer.pop(val)) {
                // spinlock
            }
        } });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();
    double duration_sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;
    double ops_per_sec = ITERATIONS / duration_sec;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Result for " << name << std::endl;
    std::cout << "Time: " << std::fixed << std::setprecision(4) << duration_sec << " seconds" << std::endl;
    std::cout << "Speed: " << (ops_per_sec / 1e6) << " Million Ops/sec" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cout << "Provide a ring buffer implementation" << std::endl;
        return 1;
    }

    std::string type = argv[1];

    std::cout << "=== RPI5 BENCHMARK (" << ITERATIONS << " ops) ===" << std::endl;

    if (type == "aligned")
    {
        run_benchmark<AlignedRingBuffer<int, BUFFER_SIZE>>("Aligned");
    }
    else if (type == "shadowed")
    {
        run_benchmark<ShadowedRingBuffer<int, BUFFER_SIZE>>("Shadowed");
    }
    else
    {
        std::cout << "Unknown implementation: " << type << std::endl;
        return 1;
    }

    return 0;
}