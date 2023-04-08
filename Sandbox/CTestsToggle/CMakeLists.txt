project(MyProject)
cmake_minimum_required(VERSION 3.20)


option(ENABLE_TESTS "Enables or disables the unit tests" OFF)

if (${ENABLE_TESTS})
    enable_testing()
endif()

add_subdirectory("Source/Submodule")

add_executable(MyProject Source/Core/Main.cpp)
target_link_libraries(MyProject Submodule)