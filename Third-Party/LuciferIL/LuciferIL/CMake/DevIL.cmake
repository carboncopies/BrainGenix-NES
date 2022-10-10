add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/DevIL/DevIL)

target_include_directories(IL-Lucifer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/DevIL/DevIL/include)
target_include_directories(IL-Lucifer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/DevIL/DevIL/src-IL/include)

disable_target_warnings(IL-Lucifer)
