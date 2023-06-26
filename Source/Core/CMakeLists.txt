# Configure Sources
set(MAIN_SOURCES 

  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h
  
  ${SRC_DIR}/Core/Config/ConfigurationManager.cpp
  ${SRC_DIR}/Core/Config/ConfigurationManager.h
  ${SRC_DIR}/Core/Config/ArgumentParser.cpp
  ${SRC_DIR}/Core/Config/ArgumentParser.h
  ${SRC_DIR}/Core/Config/ConfigFileParser.cpp
  ${SRC_DIR}/Core/Config/ConfigFileParser.h
  ${SRC_DIR}/Core/Config/Config.cpp
  ${SRC_DIR}/Core/Config/Config.h
  ${SRC_DIR}/Core/Config/ConfigDefaults.cpp
  ${SRC_DIR}/Core/Config/ConfigDefaults.h

  ${SRC_DIR}/Core/RPC/Manager.cpp
  ${SRC_DIR}/Core/RPC/Manager.h

  )




# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    rpclib::rpc

    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)