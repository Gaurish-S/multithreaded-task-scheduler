#pragma once

#include "deque.hpp"

#include <thread>
#include <atomic>
#include <random>
#include <cstddef>

class Scheduler;

class Worker {
public:
    Worker() = default;

    ~Worker();

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
    Worker(Worker&&) = delete;
    Worker& operator=(Worker&&) = delete;

    void initialize(size_t id, Scheduler* parent_scheduler);

    
    void execute();

    void stop() noexcept;

    inline Deque& get_deque() noexcept { 
        return deque_; 
    }

    inline size_t id() const noexcept { 
        return id_; 
    }

private:
    // Unique identification
    size_t id_{0};
    
    // Pointer back to the parent coordinator allowing discovery of victim deques
    Scheduler* scheduler_{nullptr};
    
    // Execution flag
    std::atomic<bool> running_{false};
    
    std::thread thread_;

    // Rng for stealing work from a random victim thread
    std::mt19937_64 rng_{std::random_device{}()};
};

