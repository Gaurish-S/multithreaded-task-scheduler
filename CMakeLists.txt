# my_project/CMakeLists.txt
cmake_minimum_required(VERSION 4.3.3)
project(MultithreadedTaskScheduler VERSION 1.0.0)

# Global compilation configurations
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Order matters: Parse the library first so the executable can see the target!
add_subdirectory(libs/scheduler)

# Define the demo executable
add_executable(
    scheduler_demo 
    src/main.cpp
)

# Link against the target name directly
target_link_libraries(
    scheduler_demo 
    PRIVATE scheduler
)
