cmake_minimum_required(VERSION 3.25)
project(WorkStealingScheduler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)

add_library(scheduler_core STATIC
    src/deque.cpp
    src/worker.cpp
    src/scheduler.cpp
)

target_include_directories(scheduler_core 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -Werror
target_compile_options(scheduler_core PRIVATE -Wall -Wextra -Wpedantic)

find_package(Threads REQUIRED)
target_link_libraries(scheduler_core PUBLIC Threads::Threads)

add_executable(fibonacci_test 
    tests/fibonacci_test.cpp
)

target_link_libraries(fibonacci_test PRIVATE scheduler_core)
