cmake_minimum_required(VERSION 3.5)
project(ThreadNamingUtils)

add_library(ThreadNamingUtils

    "ThreadNamingUtils/SetThreadName.cpp"
    "ThreadNamingUtils/GetThreadName.cpp"
    
    "ThreadNamingUtils/SetThreadName.h"
    "ThreadNamingUtils/GetThreadName.h"

)

target_include_directories(ThreadNamingUtils
    PUBLIC
        ${PROJECT_SOURCE_DIR}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/ThreadNamingUtils
)