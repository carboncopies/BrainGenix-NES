cmake_minimum_required (VERSION 3.1...3.21)


# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})


# Setup vcpkg script with CMake (note: should be placed before project() call)
set(CMAKE_TOOLCHAIN_FILE ${LIB_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")


# Define Project
project(
	"BrainGenix-NES"
	VERSION "0.2.0"
	LANGUAGES CXX
	)


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)

# Ensure config is copied to bin
file(COPY "${SRC_DIR}/Data/NES.cfg" DESTINATION "${BIN_DIR}")


add_subdirectory(${SRC_DIR}/Core)