project(MyProject)
cmake_minimum_required(VERSION 3.1)



enable_testing()

add_subdirectory("Source/Submodule")

add_executable(MyProject Source/Core/Main.cpp)
target_link_libraries(MyProject Submodule)