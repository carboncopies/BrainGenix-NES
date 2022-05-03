# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BrainGenix-NES"
	VERSION "0.1.0"
	LANGUAGES CXX
	)

# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Third-Party")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(CMAKE_BUILD_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/BuildUtils")
set(CMAKE_FIND_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/FindUtils")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)
include(${CMAKE_UTIL_DIR}/DisableTargetWarnings.cmake)


# Include Package Addition Scripts
include(${CMAKE_BUILD_UTILS_DIR}/YAML-CPP.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Backward.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/PCIIDS.cmake)


# Add Main.cpp
add_executable(${PROJECT_NAME} ${SRC_DIR}/Main.cpp ${BACKWARD_ENABLE})
target_link_libraries(${PROJECT_NAME}

  # Third Party Libs
  yaml-cpp

  # Internal Libs
  NES_ConfigurationLoader
  NES_CLASS_LoggingSystem

  NES_ThreadingEngine

  )


# ADD INTERNAL SUBDIRECTORIES

add_subdirectory(${SRC_DIR}/Core/ModelExecutionSystem/NES_ThreadingEngine)

add_subdirectory(${SRC_DIR}/Core/Models/Neurons/NES_EmptyNeuronModel)

add_subdirectory(${SRC_DIR}/Core/Structures/NES_STRUCT_RGBColor)


add_subdirectory(${SRC_DIR}/Core/Utils/NES_LoggingSystem)
add_subdirectory(${SRC_DIR}/Core/Utils/NES_ConfigurationLoader)

# COPY FILES TO BINARY DIR
file(COPY "${SRC_DIR}/Config.yaml" DESTINATION "${BIN_DIR}")