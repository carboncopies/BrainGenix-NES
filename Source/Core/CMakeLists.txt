


# Configure Sources
set(MAIN_SOURCES 


  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h

  
  )


# Create Main Executable
include(${CMAKE_SCRIPTS_DIR}/LinuxBundle/LinuxBundle.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    )
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)