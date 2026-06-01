#pragma once

#include "worker.hpp"
#include "task.hpp"

#include <vector>
#include <memory>
#include <concepts>
#include <thread>
#include <atomic>

/**
 * @brief Global coordinator managing the Worker Thread Pool lifecycle.
 * * In the Blumofe Leiserson , the Scheduler serves as the global registry.
 * It initializes the execution context, maps workers to OS threads, and provides
 * safe discovery paths for thief threads to locate random victims.
 */
class [[nodiscard]] B_l_scheduler {
public:
    /**
     * @brief Allocates and initializes the hardware-bound execution workers.
     * @param thread_count Number of processing cores. Defaults to the system's execution concurrency.
     */
    explicit Scheduler(size_t thread_count = std::thread::hardware_concurrency());
    
    /**
     * @brief Custom destructor ensuring clean, panic-free thread reclamation.
     */
    ~Scheduler();

    // Rule of five 
    // Moving or copying a thread manager mid-execution breaks hardware references. 
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;

    /**
     * @brief Spawns an arbitrary computational payload into the current worker's runtime context.
     * * EXCEPTION RULE: Because this is a templated function, its implementation must live in the header.
     * * @tparam F Any callable type (lambda, functor, function pointer).
     * @param work The computational block to execute.
     * @param parent Optional pointer to a parent task node to construct the Fork-Join DAG.
     */
    template <typename F>
    requires std::invocable<F>
    void spawn(F&& work, Task* parent = nullptr) {
        // 1. Wrap the callable `work` into a concrete Task instance.
        // 2. Increment the parent's `join_counter` (if a parent exists) using memory_order_relaxed.
        // 3. Resolve the calling context using `get_current_thread_worker()`.
        // 4. If called by an internal Worker thread: push to its local Deque via `push_bottom()`.
        // 5. If called by an external thread (e.g., main()): inject into a global lock-free queue
        //    or target Worker 0 to bootstrap the execution tree.
    }

    /**
     * @brief Blocks execution until all tasks nested under a parent node hit a join_counter of zero.
     * * Instead of sleeping, the calling thread actively steals or runs local tasks to help finish the work.
     * This prevents worker starvation during synchronization points.
     */
    void sync(Task* parent) noexcept;

    /**
     * @brief Signals all underlying thread execution loops to gracefully wind down.
     */
    void shutdown() noexcept;

    // --- PERFORMANCE MICRO-GETTERS (Inlined in Header) ---

    /**
     * @brief Queries total workers available in the system. Used by thieves for randomized victim selection.
     */
    [[nodiscard]] inline size_t total_workers() const noexcept { 
        return workers_.size(); 
    }

    /**
     * @brief Provides access to a specific worker's deque.
     * @param index The target victim worker index.
     */
    [[nodiscard]] inline Deque& get_worker_deque(size_t index) noexcept { 
        return workers_[index]->get_deque(); 
    }

private:
    // Core thread pool tracking storage. 
    // std::unique_ptr ensures stable pointers in memory even if the vector reallocates.
    std::vector<std::unique_ptr<Worker>> workers_;
    
    // Explicit tracking flag for state transitions
    std::atomic<bool> is_shutdown_{false};

    /**
     * @brief Identifies if the current OS thread calling the scheduler belongs to one of our Workers.
     * @return Pointer to the active local Worker instance, or nullptr if called from outside the pool.
     */
    [[nodiscard]] Worker* get_current_thread_worker() noexcept;
};

