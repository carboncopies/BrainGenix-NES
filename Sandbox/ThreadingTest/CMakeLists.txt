# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BGThreadingTest"
	VERSION "0.0.1"
	LANGUAGES CXX
	)

# Set Dirs
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_INSTALL_PREFIX ${BIN_DIR})

# Add Threads Lib
find_package(Threads REQUIRED)

# Add Main.cpp
add_executable(${PROJECT_NAME} "${SRC_DIR}/Main.cpp" ${BACKWARD_ENABLE})
target_include_directories(${PROJECT_NAME} PRIVATE Source)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

