#pragma once
#include <atomic>
#include <cstdint>
#include <cstddef>

constexpr size_t CACHE_LINE = 64;

template <typename T, size_t Size>
class AlignedRingBuffer
{
    static_assert((Size & (Size - 1)) == 0, "Buffer size must be a power of 2!");

public:
    struct alignas(CACHE_LINE)
    {
        std::atomic<size_t> head{0};
    } producer;

    struct alignas(CACHE_LINE)
    {
        std::atomic<size_t> tail{0};
    } consumer;

    T data[Size];

    /*
        Producer (Push into the buffer)
    */
    bool push(T val)
    {
        // where am I (we own the head so we can used relaxed)?
        size_t h = producer.head.load(std::memory_order_relaxed);
        // Where is consumer (make sure you see the latest value)?
        size_t t = consumer.tail.load(std::memory_order_acquire);

        // Buffer is full
        if (((h + 1) & (Size - 1)) == t)
        {
            return false;
        }

        // write value to head
        data[h] = val;

        // make sure the write is fully visible to other cores
        producer.head.store((h + 1) & (Size - 1), std::memory_order_release);
        return true;
    }

    bool pop(T &val)
    {
        size_t t = consumer.tail.load(std::memory_order_relaxed);
        size_t h = producer.head.load(std::memory_order_acquire);

        // buffer empty
        if (h == t)
        {
            return false;
        }

        val = data[t];

        consumer.tail.store((t + 1) & (Size - 1), std::memory_order_release);
        return true;
    }
};