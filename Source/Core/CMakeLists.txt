# Configure Sources
set(MAIN_SOURCES 

  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h
  
  )




# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    rpclib::rpc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)