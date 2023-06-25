# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BrainGenix-NES"
	VERSION "0.1.0"
	LANGUAGES CXX
	)

# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)
include(${CMAKE_UTIL_DIR}/DisableTargetWarnings.cmake)

# Enable CTests
enable_testing()

# Include Package Addition Scripts


# Add Main.cpp
add_executable(${PROJECT_NAME} ${SRC_DIR}/Main.cpp ${BACKWARD_ENABLE})
target_link_libraries(${PROJECT_NAME}

  # Third Party Libs
  yaml-cpp

  # Internal Libs
  )


# ADD INTERNAL SUBDIRECTORIES
add_subdirectory(${SRC_DIR}/)


# COPY FILES TO BINARY DIR
file(COPY "${SRC_DIR}/Config.yaml" DESTINATION "${BIN_DIR}")