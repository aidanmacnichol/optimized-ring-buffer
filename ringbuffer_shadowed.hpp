#pragma once
#include <atomic>
#include <cstdint>
#include <cstddef>

template <typename T, size_t Size>
class ShadowedRingBuffer
{
    static_assert((Size & (Size - 1)) == 0, "Buffer size must be a power of 2!");

public:
    struct alignas(CACHE_LINE)
    {
        std::atomic<size_t> head{0};
        size_t tail_cached{0};
    } producer;

    struct alignas(CACHE_LINE)
    {
        std::atomic<size_t> tail{0};
        size_t head_cached{0};
    } consumer;

    T data[Size];

    bool push(T val)
    {
        size_t h = producer.head.load(std::memory_order_relaxed);
        size_t next_h = (h + 1) & (Size - 1);

        if (next_h == producer.tail_cached)
        {
            producer.tail_cached = consumer.tail.load(std::memory_order_acquire);

            // Deadass still full???
            if (next_h == producer.tail_cached)
            {
                return false;
            }
        }

        data[h] = val;

        producer.head.store(next_h, std::memory_order_release);
        return true;
    }

    bool pop(T &val)
    {
        size_t t = consumer.tail.load(std::memory_order_relaxed);

        if (t == consumer.head_cached)
        {
            consumer.head_cached = producer.head.load(std::memory_order_acquire);

            if (t == consumer.head_cached)
            {
                return false;
            }
        }

        val = data[t];

        consumer.tail.store((t + 1) & (Size - 1), std::memory_order_release);
        return true;
    }
};