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

  ${SRC_DIR}/Core/Simulator/Geometries/Geometry.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Geometry.h
  ${SRC_DIR}/Core/Simulator/Geometries/Sphere.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Sphere.h
  ${SRC_DIR}/Core/Simulator/Geometries/Cylinder.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Cylinder.h
  ${SRC_DIR}/Core/Simulator/Geometries/Box.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Box.h
  ${SRC_DIR}/Core/Simulator/Geometries/VecTools.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/VecTools.h

  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.h
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.cpp

  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.cpp
  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.h
  ${SRC_DIR}/Core/Simulator/Distributions/Distribution.cpp
  ${SRC_DIR}/Core/Simulator/Distributions/Distribution.h

  ${SRC_DIR}/Core/Simulator/Structs/Simulation.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Simulation.h
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
  ${SRC_DIR}/Core/Simulator/Structs/Neuron.h
  
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

)
 
set(UNITTEST_SOURCES

  ${SRC_DIR}/Core/Simulator/Geometries/Box.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Cylinder.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Sphere.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/VecTools.test.cpp

  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.test.cpp
  
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.test.cpp
)

# Configure test binaries
set(TEST_BINS

    BrainGenix-NES.test
)

# Create Main Library
add_library(${PROJECT_LIBRARY_NAME} STATIC ${MAIN_SOURCES})
target_compile_features(${PROJECT_LIBRARY_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_LIBRARY_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)
target_include_directories(${PROJECT_LIBRARY_NAME} PUBLIC ${SRC_DIR}/Core)

# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::program_options
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)

# Configure testing.
enable_testing()
include(GoogleTest)

message(STATUS "Testing enabled. Building test binaries ...")
add_executable(${TEST_BINS} ${UNITTEST_SOURCES})
target_link_libraries(${TEST_BINS} PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    ${PROJECT_LIBRARY_NAME}
)
target_include_directories(${TEST_BINS} PUBLIC ${SRC_DIR}/Core)

gtest_discover_tests(${TEST_BINS})
