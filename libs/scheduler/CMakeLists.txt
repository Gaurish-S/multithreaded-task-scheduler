add_library(
    scheduler
    src/scheduler.cpp
)

target_include_directories(
    scheduler 
    PUBLIC include
)
