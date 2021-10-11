cmake_minimum_required(VERSION 3.15)
project(cpp_py_test)

cmake_policy(SET CMP0078 NEW) # Use standard names for swig targets
cmake_policy(SET CMP0086 NEW) # Pass module name to swig from cmake


set(SOURCE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(BINARY_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR})


# Find necessary packages
find_package(PythonLibs REQUIRED)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


# Set some output locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/Artifacts/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/Artifacts/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/Artifacts/lib)
set(CMAKE_SWIG_OUTDIR              ${SOURCE_ROOT_DIR}/Artifacts/py)
set(WRAPPER_DIR                    ${SOURCE_ROOT_DIR}/Wrap)
set(SWIG_OUTFILE_DIR               ${BINARY_ROOT_DIR}/swig_generated)


# Add c++ library to link against

add_library(cpptest OBJECT
  cpp/src/cpptest.cpp
  cpp/include/cpptest/cpptest.h
  )
target_include_directories(cpptest PUBLIC cpp/include)
target_compile_options(cpptest PRIVATE
  -fPIC
  -std=c++17
  -Wall
  -Wextra
  )


# Generate wrapper with swig

set_source_files_properties(${WRAPPER_DIR}/pycpptest.i PROPERTIES
  CPLUSPLUS ON
  SWIG_MODULE_NAME pycpptest
  )

swig_add_library(pycpptest LANGUAGE python SOURCES ${WRAPPER_DIR}/pycpptest.i)

target_include_directories(pycpptest PRIVATE ${PYTHON_INCLUDE_PATH})
target_link_libraries(pycpptest PRIVATE cpptest ${PYTHON_LIBRARIES})
set_target_properties(pycpptest PROPERTIES
  SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
  GENERATED_COMPILE_OPTIONS "-fPIC -std=c++17 -Wall -Wextra"
  )

