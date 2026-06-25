#pragma once

#include "worker.hpp"
#include "task.hpp"

#include <vector>
#include <memory>
#include <concepts>
#include <thread>
#include <atomic>


class Scheduler {
public:
   
    // Allocates and initializes the hardware-bound workers.
    explicit Scheduler(size_t thread_count = std::thread::hardware_concurrency());
    
    ~Scheduler();

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;


    // Wait for child task to finish via working on other local or external tasks
    void sync(Task* parent) noexcept;

    void shutdown() noexcept;

    // Gets total workers available in the system. Used by thieves for randomized victim selection.
    inline size_t total_workers() const noexcept { 
        return workers_.size(); 
    }

    // Get worker's task deque
    inline Deque& get_worker_deque(size_t index) noexcept { 
        return workers_[index]->get_deque(); 
    }

private:
    // Core thread pool tracking storage. 
    // std::unique_ptr ensures stable pointers in memory even if the vector reallocates.
    std::vector<std::unique_ptr<Worker>> workers_;

};

