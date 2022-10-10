cmake_minimum_required(VERSION 3.16.2)
project(FreeImagePlus VERSION 3.18.0)

if(WIN32)
	# __declspec(dllexport) or __declspec(dllimport) for FreeImagePlus
	add_compile_definitions(FIP_EXPORTS)
endif()

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC FreeImage)
add_dependencies(${PROJECT_NAME} FreeImage)
