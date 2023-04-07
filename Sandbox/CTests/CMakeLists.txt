project(test)
cmake_minimum_required(VERSION 3.1)

add_subdirectory("Source/Submodule")

add_executable(main_executable Source/Core/Main.cpp)