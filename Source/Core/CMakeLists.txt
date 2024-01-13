# Configure Sources
set(MAIN_SOURCES 


  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h
  

  ${SRC_DIR}/Core/Config/ConfigurationManager.cpp
  ${SRC_DIR}/Core/Config/ConfigurationManager.h
  # ${SRC_DIR}/Core/Config/ArgumentParser.cpp
  # ${SRC_DIR}/Core/Config/ArgumentParser.h
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

  ${SRC_DIR}/Core/Simulator/BrainRegion/BrainRegion.h

  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.cpp
  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.h
  ${SRC_DIR}/Core/Simulator/Distributions/Distribution.cpp
  ${SRC_DIR}/Core/Simulator/Distributions/Distribution.h

  ${SRC_DIR}/Core/Simulator/Receptors/Receptor.cpp
  ${SRC_DIR}/Core/Simulator/Receptors/Receptor.h
  ${SRC_DIR}/Core/Simulator/Receptors/AMPAReceptor.cpp
  ${SRC_DIR}/Core/Simulator/Receptors/AMPAReceptor.h
  ${SRC_DIR}/Core/Simulator/Receptors/NMDAReceptor.cpp
  ${SRC_DIR}/Core/Simulator/Receptors/NMDAReceptor.h
  
  ${SRC_DIR}/Core/Simulator/Structs/Simulation.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Simulation.h
  ${SRC_DIR}/Core/Simulator/Structs/BoundingBox.cpp
  ${SRC_DIR}/Core/Simulator/Structs/BoundingBox.h
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
  ${SRC_DIR}/Core/Simulator/Structs/SignalFunctions.cpp
  ${SRC_DIR}/Core/Simulator/Structs/SignalFunctions.h
  ${SRC_DIR}/Core/Simulator/Structs/Neuron.h
  ${SRC_DIR}/Core/Simulator/Structs/NeuralCircuit.h
  ${SRC_DIR}/Core/Simulator/Structs/RecordingElectrode.h
  ${SRC_DIR}/Core/Simulator/Structs/RecordingElectrode.cpp
  ${SRC_DIR}/Core/Simulator/Structs/SynTrQuantalRelease.h
  ${SRC_DIR}/Core/Simulator/Structs/SynTrQuantalRelease.cpp
  
  ${SRC_DIR}/Core/Simulator/Updaters/Staple.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/Staple.h
  ${SRC_DIR}/Core/Simulator/Updaters/Receptor.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/Receptor.h
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampDAC.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampDAC.h
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampADC.cpp
  ${SRC_DIR}/Core/Simulator/Updaters/PatchClampADC.h

  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.h
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNC.h
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNC.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSMorphology.h
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSMorphology.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedBrainRegion.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedBrainRegion.h
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNCRandomUniform.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNCRandomUniform.h


  ${SRC_DIR}/Core/Util/JSONHelpers.cpp
  ${SRC_DIR}/Core/Util/JSONHelpers.h


  ${SRC_DIR}/Core/VSDA/RPCInterface.cpp
  ${SRC_DIR}/Core/VSDA/RPCInterface.h
  ${SRC_DIR}/Core/VSDA/UpdaterManager.cpp
  ${SRC_DIR}/Core/VSDA/UpdaterManager.h
  ${SRC_DIR}/Core/VSDA/RenderPool.cpp
  ${SRC_DIR}/Core/VSDA/RenderPool.h
  ${SRC_DIR}/Core/VSDA/SliceGenerator/SliceGenerator.cpp
  ${SRC_DIR}/Core/VSDA/SliceGenerator/SliceGenerator.h
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/MicroscopeParameters.cpp
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/MicroscopeParameters.h
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/ScanRegion.cpp
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/ScanRegion.h
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/VoxelArray.cpp
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/VoxelArray.h
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/VSDAData.cpp
  ${SRC_DIR}/Core/VSDA/SliceGenerator/Structs/VSDAData.h

  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshBuilder.cpp
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshBuilder.h
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshConversionHelpers.cpp
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshConversionHelpers.h


)
 
set(UNITTEST_SOURCES

  ${SRC_DIR}/Core/Simulator/Geometries/Box.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Cylinder.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/Sphere.test.cpp
  ${SRC_DIR}/Core/Simulator/Geometries/VecTools.test.cpp

  ${SRC_DIR}/Core/Simulator/Distributions/TruncNorm.test.cpp
  
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSNeuron.test.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNC.test.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSAlignedNCRandomUniform.test.cpp
  ${SRC_DIR}/Core/Simulator/BallAndStick/BSMorphology.test.cpp

  ${SRC_DIR}/Core/Simulator/Receptors/NMDAReceptor.test.cpp
  ${SRC_DIR}/Core/Simulator/Receptors/AMPAReceptor.test.cpp
  ${SRC_DIR}/Core/Simulator/Receptors/Receptor.test.cpp
  
  ${SRC_DIR}/Core/Simulator/Structs/SignalFunctions.test.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Simulation.test.cpp
  ${SRC_DIR}/Core/Simulator/Structs/RecordingElectrode.test.cpp
  ${SRC_DIR}/Core/Simulator/Structs/SynTrQuantalRelease.test.cpp
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
    # Boost::program_options
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    NESRenderer
    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)
target_include_directories(${PROJECT_LIBRARY_NAME} PUBLIC ${SRC_DIR}/Core)
target_include_directories(${PROJECT_LIBRARY_NAME} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})


# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    # Boost::program_options
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    NESRenderer
    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)
target_include_directories(${PROJECT_NAME} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})

# Configure testing.
enable_testing()
include(GoogleTest)

message(STATUS "Testing enabled. Building test binaries ...")
add_executable(${TEST_BINS} ${UNITTEST_SOURCES})
target_compile_features(${TEST_BINS} PRIVATE cxx_std_17)

target_link_libraries(${TEST_BINS} PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    ${PROJECT_LIBRARY_NAME}
)
target_include_directories(${TEST_BINS} PUBLIC ${SRC_DIR}/Core)
target_include_directories(${TEST_BINS} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})

gtest_discover_tests(${TEST_BINS})
