# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BrainGenix-ERS"
	VERSION "0.1.2"
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
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)
include(${CMAKE_UTIL_DIR}/DisableTargetWarnings.cmake)


# Include Package Addition Scripts
include(${CMAKE_BUILD_UTILS_DIR}/Threads.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Glad.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/GLM.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/GLFW.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/STB.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Assimp.cmake)

add_subdirectory(Source/learnopengl)

# Add Main.cpp
add_executable(${PROJECT_NAME} ${SRC_DIR}/Main.cpp)
target_link_libraries(${PROJECT_NAME}

  # Third Party Libs
  glad
  glfw

  # Internal lib
  learnopengl

  )


# COPY FILES TO BINARY DIR
file(COPY "${SRC_DIR}/Container.png" DESTINATION "${BIN_DIR}/")
file(COPY "${SRC_DIR}/Shaders/" DESTINATION "${BIN_DIR}/")
