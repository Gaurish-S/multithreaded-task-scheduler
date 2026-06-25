#pragma once

#include <atomic>
#include <concepts>
#include <functional>

struct Task {
    // Parent task 
    Task* parent{nullptr};

    // No. of tasks current task is waiting on
    std::atomic<int> join_counter{0};

    std::function<void()> work;

    // Constructor initializing a root task node with no parent
    Task() = default;

    
    ~Task() = default;

    // Safety precaution so that stack frames for tasks don't get invalidated upon moves
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) = delete;
    Task& operator=(Task&&) = delete;

    inline void execute() noexcept {
        if (work) {
            work();
        }

        // 2. Propagate structural tree updates up to the parent context.
        //    When this sub-task finishes, if we have a parent, we must decrement 
        //    its join counter to notify it that one of its dependencies is cleared.
        if (parent) {
        // need to figure out lock free logic here
        }
    }
};
