#pragma once

#include "task.hpp"

#include <vector>
#include <atomic>
#include <optional>
#include <cstdint>

// Lock free chase lev style deque
class Deque {
public:
    // Initial capacity is a power of 2 for easy indexing 
    explicit Deque(size_t initial_capacity = 1024);
    
    ~Deque() = default;

    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;
    Deque(Deque&&) = delete;
    Deque& operator=(Deque&&) = delete;

    
    void push_bottom(Task* task) noexcept;

    std::optional<Task*> pop_bottom() noexcept;

    std::optional<Task*> pop_top() noexcept;

private:
    // Circular array 
    std::vector<std::atomic<void*>> atomic_pointers;
    
    // Bit mask for easy indexing 
    int64_t capacity_mask_;

    void grow_buffer(int64_t b, int64_t t); // gotta be careful about dynamic alloc here
};

