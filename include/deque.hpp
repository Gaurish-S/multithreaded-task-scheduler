#pragma once

#include "task.hpp"

#include <vector>
#include <atomic>
#include <optional>
#include <cstdint>
#include <new>

/**
 * @brief A Lock-Free, Single-Producer, Multi-Consumer (SPMC) Chase-Lev style Deque.
 * * DESIGN INTUITION (Asymmetric Contention):
 * - Owner Thread: Mutates the BOTTOM index via LIFO operations (`push_bottom`, `pop_bottom`).
 * - Thief Threads: Mutate the TOP index via FIFO operations (`steal_top`).
 * * Real synchronization overhead only occurs when a single element remains, and the
 * owner's `pop_bottom` races against a thief's concurrent `steal_top`.
 */
class [[nodiscard]] Deque {
public:
    /**
     * @brief Instantiates the concurrent buffer.
     * @param initial_capacity Must be a power of 2 to optimize circular indexing.
     */
    explicit Deque(size_t initial_capacity = 1024);
    
    /**
     * @brief Destructor. Cleans up internal allocations.
     */
    ~Deque() = default;

    // --- Rule of Five ---
    // Concurrent primitives cannot be copied or shifted mid-execution without breaking invariants.
    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;
    Deque(Deque&&) = delete;
    Deque& operator=(Deque&&) = delete;

    /**
     * @brief Pushes a task pointer onto the bottom of the deque.
     * @note ONLY called by the local worker thread that owns this deque.
     * @param task Pointer to the fork-join child computation node.
     */
    void push_bottom(Task* task) noexcept;

    /**
     * @brief Extracts a task pointer from the bottom of the deque (LIFO).
     * @note ONLY called by the local worker thread that owns this deque.
     * @return A type-safe optional holding a Task pointer, or std::nullopt if dry.
     */
    [[nodiscard]] std::optional<Task*> pop_bottom() noexcept;

    /**
     * @brief Steals a task pointer from the top of the deque (FIFO).
     * @note Called concurrently by ANY idle worker thread acting as a thief.
     * @return A type-safe optional holding a stolen Task pointer, or std::nullopt if the steal failed or was empty.
     */
    [[nodiscard]] std::optional<Task*> steal_top() noexcept;

    /**
     * @brief Diagnostic helper to approximate the number of items remaining.
     */
    [[nodiscard]] inline int64_t approximate_size() const noexcept {
        int64_t b = bottom_.load(std::memory_order_relaxed);
        int64_t t = top_.load(std::memory_order_relaxed);
        return (b > t) ? (b - t) : 0;
    }

private:
    // --- CACHE-LINE FALSER SHARING SHIELD ---
    // We isolate 'top_' and 'bottom_' onto distinct CPU cache lines using hardware padding.
    // This stops active local writes on 'bottom_' from continuously evicting 'top_' out of remote thieves' L1/L2 caches.
    
    alignas(std::hardware_destructive_interference_size) std::atomic<int64_t> top_{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<int64_t> bottom_{0};

    // Circular array of atomic task pointers
    std::vector<std::atomic<Task*>> buffer_;
    
    // Mask helper used to bypass slow modulo instructions: (index % capacity) becomes (index & capacity_mask_)
    int64_t capacity_mask_;

    // Private structural helper handles dynamic array resizing if the buffer fills up.
    // In lock-free programming, dynamic allocation requires a carefully managed fence layout.
    void grow_buffer(int64_t b, int64_t t);
};

