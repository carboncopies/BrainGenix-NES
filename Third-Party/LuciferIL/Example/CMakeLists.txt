cmake_minimum_required(VERSION 3.5)
project(Lucifer-Example)


# Define CMake Paths
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")

# Set Binary Dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(CMAKE_INSTALL_PREFIX ${BIN_DIR})

# Include Lucifer
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../LuciferIL/ ${CMAKE_CURRENT_BINARY_DIR}/LuciferIL)


# Build Executable
add_executable(Lucifer-Example Example.cpp)
target_link_libraries(Lucifer-Example Lucifer)


# Copy Assets
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Assets/" DESTINATION "${BIN_DIR}/Assets")