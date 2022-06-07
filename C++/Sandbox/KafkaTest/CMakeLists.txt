cmake_minimum_required(VERSION 3.15)
project(cpp_kafka_test)


set(SOURCE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(BINARY_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR})



# Find necessary packages
#find_package(librdkafka REQUIRED)


# Set some output locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/artifacts/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/artifacts/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SOURCE_ROOT_DIR}/artifacts/lib)


# Add c++ library to link against
add_executable(Consumer src/KafkaConsumer.cpp)
add_executable(Producer src/KafkaProducer.cpp)

target_compile_options(Consumer PRIVATE
  -fPIC
  -std=c++17
  -Wall
  -Wextra
  )

target_compile_options(Producer PRIVATE
  -fPIC
  -std=c++17
  -Wall
  -Wextra
  )

#target_link_libraries(Consumer PRIVATE ${librdkafka_LIBRARIES})
target_include_directories(Consumer PRIVATE "/usr/include/librdkafka")
target_include_directories(Producer PRIVATE "/usr/include/librdkafka")

target_link_directories(Producer PRIVATE "/usr/lib/x86_64-linux-gnu")
target_link_libraries(Producer PRIVATE rdkafka++)