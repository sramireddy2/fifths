#pragma once

#include <atomic>
#include <cstddef>
#include <span>
#include <vector>

// Single-producer, single-consumer ring. The audio callback will write,
// the analysis loop will read. No mutex, no allocation after construction.
class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity) : buf_(capacity, 0.0f), cap_(capacity) {}

    // Returns false if the buffer is full. Caller should just drop the sample.
    bool push(float x) {
        const std::size_t w = write_.load(std::memory_order_relaxed);
        const std::size_t next = (w + 1) % cap_;
        if (next == read_.load(std::memory_order_acquire)) {
            return false;
        }
        buf_[w] = x;
        write_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(float& out) {
        const std::size_t r = read_.load(std::memory_order_relaxed);
        if (r == write_.load(std::memory_order_acquire)) {
            return false;
        }
        out = buf_[r];
        read_.store((r + 1) % cap_, std::memory_order_release);
        return true;
    }

    std::size_t write(std::span<const float> in) {
        std::size_t n = 0;
        for (float s : in) {
            if (!push(s)) {
                break;
            }
            ++n;
        }
        return n;
    }

    std::size_t read(std::span<float> out) {
        std::size_t n = 0;
        while (n < out.size() && pop(out[n])) {
            ++n;
        }
        return n;
    }

private:
    std::vector<float> buf_;
    std::size_t cap_;
    std::atomic<std::size_t> write_{0};
    std::atomic<std::size_t> read_{0};
};
