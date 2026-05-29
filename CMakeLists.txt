cmake_minimum_required(VERSION 4.3.3)
project(MultithreadedTaskScheduler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
	scheduler
	libs/scheduler/src/scheduler.cpp
)

target_include_directories(scheduler PUBLIC libs/scheduler/include)

add_executable(scheduler_demo src/main.cpp)

target_link_libraries(scheduler_demo PRIVATE scheduler)
