#include "worker.hpp"
#include "scheduler.hpp"

// Hint: Declare your thread_local Worker* pointer here to back get_current_thread_worker()

Worker::~Worker() {
    // TODO: Ensure threads are terminated cleanly
}

void Worker::initialize(size_t id, Scheduler* parent_scheduler) {
    // TODO: Map identifiers, setup state flags, and launch thread_
}

void Worker::execution_loop() noexcept {
    // TODO: Implement the "Pop local LIFO -> Steal remote random FIFO" loop
}

void Worker::stop() noexcept {
    // TODO: Signal thread loop cooperative cancellation
}
