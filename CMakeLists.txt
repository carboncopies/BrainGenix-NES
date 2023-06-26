cmake_minimum_required (VERSION 3.1...3.21)


# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(CMAKE_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/CompileInfo")
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

# Add Libraries
message(STATUS "Finding nlohmann_json Package")
find_package(nlohmann_json CONFIG REQUIRED)

message(STATUS "Finding boost Package")
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost REQUIRED COMPONENTS program_options)

message(STATUS "Finding rpclib Package")
find_package(rpclib CONFIG REQUIRED)

# Ensure config is copied to bin
file(COPY "${SRC_DIR}/Data/NES.cfg" DESTINATION "${BIN_DIR}")

# Run Configuration Tools
include(${CMAKE_SCRIPTS_DIR}/VersioningSystem/VersioningSystem.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompilePlatformInfo/CompilePlatformInfo.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompileTimeStamp/CompileTimeStamp.cmake)




add_subdirectory(${SRC_DIR}/Core)