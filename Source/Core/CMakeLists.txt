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
  ${SRC_DIR}/Core/RPC/StaticRoutes.cpp
  ${SRC_DIR}/Core/RPC/StaticRoutes.h

  ${SRC_DIR}/Core/Simulator/Manager.cpp
  ${SRC_DIR}/Core/Simulator/Manager.h
  ${SRC_DIR}/Core/Simulator/Engine.cpp
  ${SRC_DIR}/Core/Simulator/Engine.h
  ${SRC_DIR}/Core/Simulator/EngineController.cpp
  ${SRC_DIR}/Core/Simulator/EngineController.h

  ${SRC_DIR}/Core/Simulator/Structs/Simulation.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Simulation.h
  ${SRC_DIR}/Core/Simulator/Structs/Shapes.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Shapes.h
  ${SRC_DIR}/Core/Simulator/Structs/Sphere.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Sphere.h
  ${SRC_DIR}/Core/Simulator/Structs/Cylinder.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Cylinder.h
  ${SRC_DIR}/Core/Simulator/Structs/Box.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Box.h
  ${SRC_DIR}/Core/Simulator/Structs/BS.cpp
  ${SRC_DIR}/Core/Simulator/Structs/BS.h
  ${SRC_DIR}/Core/Simulator/Structs/Staple.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Staple.h
  ${SRC_DIR}/Core/Simulator/Structs/Receptor.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Receptor.h
  ${SRC_DIR}/Core/Simulator/Structs/PatchClampDAC.cpp
  ${SRC_DIR}/Core/Simulator/Structs/PatchClampDAC.h
  ${SRC_DIR}/Core/Simulator/Structs/PatchClampADC.cpp
  ${SRC_DIR}/Core/Simulator/Structs/PatchClampADC.h

  ${SRC_DIR}/Core/Simulator/Updaters/Staple.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/Staple.h
  ${SRC_DIR}/Core/Simulator/Updaters/Receptor.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/Receptor.h
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampDAC.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampDAC.h
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampADC.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampADC.h

  ${SRC_DIR}/Core/Util/JSONHelpers.cpp
  ${SRC_DIR}/Core/Util/JSONHelpers.h


  ${SRC_DIR}/Core/SliceGenerator/SliceGenerator.cpp
  ${SRC_DIR}/Core/SliceGenerator/SliceGenerator.h

  ${SRC_DIR}/Core/SliceGenerator/Structs/MicroscopeParameters.cpp
  ${SRC_DIR}/Core/SliceGenerator/Structs/MicroscopeParameters.h
  ${SRC_DIR}/Core/SliceGenerator/Structs/ScanRegion.cpp
  ${SRC_DIR}/Core/SliceGenerator/Structs/ScanRegion.h
  ${SRC_DIR}/Core/SliceGenerator/Structs/VoxelArray.cpp
  ${SRC_DIR}/Core/SliceGenerator/Structs/VoxelArray.h

  )




# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    bg-common-logger
    
    NESRenderer

    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)