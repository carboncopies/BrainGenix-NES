# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BrainGenix-ERS"
	VERSION "0.4.0"
	LANGUAGES CXX
	)

# Set Configuration Options
option(ENABLE_BACKWARD "Build ERS With Backward to provide some stack trace information or not" ON)


# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Third-Party")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(CMAKE_BUILD_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/BuildUtils")
set(CMAKE_FIND_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/FindUtils")
set(CMAKE_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts")
set(CMAKE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EditorAssets")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_VISIBILITY_PRESET hidden)


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)
include(${CMAKE_UTIL_DIR}/DisableTargetWarnings.cmake)

# Run Configuration Tools
include(${CMAKE_SCRIPTS_DIR}/VersioningSystem/VersioningSystem.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompilePlatformInfo/CompilePlatformInfo.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompileTimeStamp/CompileTimeStamp.cmake)

# Include Package Addition Scripts
include(${CMAKE_BUILD_UTILS_DIR}/Backward.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ThreadNamingUtils.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Threads.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/YAML-CPP.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/PCIIDS.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Assimp.cmake)


# Add LIBRARY INTERFACE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Library Interfaces")

ERSBuildLogger(${BoldGreen} "Finished Configuring Library Interfaces")


# ADD STRUCT INTERFACE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Structure Interfaces")

add_subdirectory(${SRC_DIR}/Internal/Structs/SystemUtils)
message("-- Configured Internal Interface 'SystemUtils'")


ERSBuildLogger(${BoldGreen} "Finished Configuring Structure Interfaces")


# ADD INTERNAL SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Internal Libraries")

add_subdirectory(${SRC_DIR}/Internal/LoggingSystem)
message("-- Configured Internal Library 'LoggingSystem'")
add_subdirectory(${SRC_DIR}/Internal/ArgumentParser)
message("-- Configured Internal Library 'ArgumentParser'")
add_subdirectory(${SRC_DIR}/Internal/HardwareInformation)
message("-- Configured Internal Library 'HardwareInformation'")

ERSBuildLogger(${BoldGreen} "Finished Configuring Internal Libraries")

# ADD CORE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Core Subsystems")

ERSBuildLogger(${BoldGreen} "Finished Configuring Core Subsystems")


# Add Main.cpp
add_executable(${PROJECT_NAME} ${SRC_DIR}/Main.cpp ${BACKWARD_ENABLE})
target_link_libraries(${PROJECT_NAME}

  ERS_ArgumentParser

  ERS_STRUCT_SystemUtils

  )


# Set Configuration Based On Platform
configure_file("${SRC_DIR}/Config.yaml" "${BIN_DIR}/Config.yaml" COPYONLY)