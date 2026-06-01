#pragma once

#include "deque.hpp"

#include <thread>
#include <atomic>
#include <random>
#include <cstddef>

// Forward declaration of the Scheduler class to avoid a circular include dependency loop.
// Since we only store a pointer to the Scheduler (Scheduler*), the compiler doesn't 
// need to read scheduler.hpp yet.
class Scheduler;

/**
 * @brief Encapsulates a dedicated processing unit executing the Blumofe-Leiserson state machine loop.
 * * DESIGN INTUITION:
 * Each worker maps to exactly one physical thread slot. It operates in two behavioral states:
 * 1. Execution Mode: Continuously executes and forks tasks out of its local Deque (LIFO order).
 * 2. Thief Mode: If its local queue goes completely dry, it shifts to choosing a remote 
 * victim uniformly at random and attempts to steal work from the top of their queue (FIFO order).
 */
class [[nodiscard]] Worker {
public:
    /**
     * @brief Instantiates the worker control blocks.
     */
    Worker() = default;

    /**
     * @brief Custom destructor to handle graceful runtime execution unwinding.
     */
    ~Worker();

    // --- Rule of Five ---
    // Background execution workers own active system threads and private caches. 
    // Allowing them to be copied or shifted across registers breaks hardware invariants.
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
    Worker(Worker&&) = delete;
    Worker& operator=(Worker&&) = delete;

    /**
     * @brief Initializes the structural background loop context and fires up the OS thread.
     * @param id Unique identification index of this worker within the global array.
     * @param parent_scheduler Pointer to the coordinating global registry engine.
     */
    void initialize(size_t id, Scheduler* parent_scheduler);

    /**
     * @brief Main execution state machine loop running the "Pop local -> Steal random" sequence.
     * @note Executed continuously inside the background thread context.
     */
    void execution_loop() noexcept;

    /**
     * @brief Triggers cooperative cancellation to signal that the loop should wind down.
     */
    void stop() noexcept;

    // --- PERFORMANCE INLINE GETTERS ---

    /**
     * @brief Public reference accessor to allow remote thief threads to access our queue.
     */
    [[nodiscard]] inline Deque& get_deque() noexcept { 
        return deque_; 
    }

    /**
     * @brief Retreives the worker's unique pool identity index.
     */
    [[nodiscard]] inline size_t id() const noexcept { 
        return id_; 
    }

private:
    // Unique identification index within the thread pool range [0, P-1]
    size_t id_{0};
    
    // Pointer back to the parent coordinator allowing discovery of victim deques
    Scheduler* scheduler_{nullptr};
    
    // The thread's private lock-free sandbox for holding pending child frames
    Deque deque_;
    
    // Cooperative execution control flag
    std::atomic<bool> running_{false};
    
    // C++20/23 RAII Cooperative Thread Container. 
    // It must be declared LAST in the class layout so that all other member variables (like deque_
    // and running_) are fully initialized BEFORE the OS thread actually fires up and reads them.
    std::jthread thread_;

    // --- RANDOMIZATION INFRASTRUCTURE ---
    // To satisfy Blumofe-Leiserson's mathematical efficiency proofs ($T_1/P + O(T_\infty)$), 
    // victims must be chosen uniformly at random. We store a high-performance pseudo-random engine 
    // directly inside the worker to avoid locking a global random number generator.
    std::mt19937_64 rng_{std::random_device{}()};
};

