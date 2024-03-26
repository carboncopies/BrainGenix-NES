# Configure Sources
set(MAIN_SOURCES 


  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h
  

  ${SRC_DIR}/Core/Config/ConfigurationManager.cpp
  ${SRC_DIR}/Core/Config/ConfigurationManager.h
  ${SRC_DIR}/Core/Config/ConfigFileParser.cpp
  ${SRC_DIR}/Core/Config/ConfigFileParser.h
  ${SRC_DIR}/Core/Config/Config.cpp
  ${SRC_DIR}/Core/Config/Config.h
  ${SRC_DIR}/Core/Config/ConfigDefaults.cpp
  ${SRC_DIR}/Core/Config/ConfigDefaults.h





  ${SRC_DIR}/Core/Simulator/RPC/SimulationRPCInterface.cpp
  ${SRC_DIR}/Core/Simulator/RPC/SimulationRPCInterface.h
  ${SRC_DIR}/Core/Simulator/RPC/GeometryRPCInterface.cpp
  ${SRC_DIR}/Core/Simulator/RPC/GeometryRPCInterface.h
  ${SRC_DIR}/Core/Simulator/RPC/ModelRPCInterface.cpp
  ${SRC_DIR}/Core/Simulator/RPC/ModelRPCInterface.h

  ${SRC_DIR}/Core/Simulator/Engine.cpp
  ${SRC_DIR}/Core/Simulator/Engine.h
  ${SRC_DIR}/Core/Simulator/EngineController.cpp
  ${SRC_DIR}/Core/Simulator/EngineController.h


  ${SRC_DIR}/Core/RPC/RPCManager.cpp
  ${SRC_DIR}/Core/RPC/RPCManager.h
  ${SRC_DIR}/Core/RPC/RPCHandlerHelper.cpp
  ${SRC_DIR}/Core/RPC/RPCHandlerHelper.h
  ${SRC_DIR}/Core/RPC/APIStatusCode.cpp
  ${SRC_DIR}/Core/RPC/APIStatusCode.h
  ${SRC_DIR}/Core/RPC/StaticRoutes.cpp
  ${SRC_DIR}/Core/RPC/StaticRoutes.h
  ${SRC_DIR}/Core/RPC/ManagerTaskData.cpp
  ${SRC_DIR}/Core/RPC/ManagerTaskData.h


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
  ${SRC_DIR}/Core/Simulator/Distributions/Generic.cpp
  ${SRC_DIR}/Core/Simulator/Distributions/Generic.h

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
  ${SRC_DIR}/Core/Simulator/Structs/SC.cpp
  ${SRC_DIR}/Core/Simulator/Structs/SC.h
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
  ${SRC_DIR}/Core/Simulator/Structs/Neuron.cpp
  ${SRC_DIR}/Core/Simulator/Structs/Neuron.h
  ${SRC_DIR}/Core/Simulator/Structs/NeuralCircuit.cpp
  ${SRC_DIR}/Core/Simulator/Structs/NeuralCircuit.h
  ${SRC_DIR}/Core/Simulator/Structs/RecordingElectrode.h
  ${SRC_DIR}/Core/Simulator/Structs/RecordingElectrode.cpp
  ${SRC_DIR}/Core/Simulator/Structs/CalciumImaging.h
  ${SRC_DIR}/Core/Simulator/Structs/CalciumImaging.cpp
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

  ${SRC_DIR}/Core/Simulator/SimpleCompartmental/SCNeuron.h
  ${SRC_DIR}/Core/Simulator/SimpleCompartmental/SCNeuron.cpp

  ${SRC_DIR}/Core/Util/JSONHelpers.cpp
  ${SRC_DIR}/Core/Util/JSONHelpers.h
  ${SRC_DIR}/Core/Util/LogLogo.cpp
  ${SRC_DIR}/Core/Util/LogLogo.h

  ${SRC_DIR}/Core/Profiling/ProfilingManager.cpp
  ${SRC_DIR}/Core/Profiling/ProfilingManager.h

  ${SRC_DIR}/Core/VSDA/VSDARPCInterface.cpp
  ${SRC_DIR}/Core/VSDA/VSDARPCInterface.h

  ${SRC_DIR}/Core/VSDA/RenderPool.cpp
  ${SRC_DIR}/Core/VSDA/RenderPool.h
  ${SRC_DIR}/Core/VSDA/RPCRoutes/EM.cpp
  ${SRC_DIR}/Core/VSDA/RPCRoutes/EM.h
  ${SRC_DIR}/Core/VSDA/RPCRoutes/Ca.cpp
  ${SRC_DIR}/Core/VSDA/RPCRoutes/Ca.h

  ${SRC_DIR}/Core/VSDA/EM/EMRenderer.cpp
  ${SRC_DIR}/Core/VSDA/EM/EMRenderer.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/EMSubRegion.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/EMSubRegion.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/LegacySliceGenerator.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/LegacySliceGenerator.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/VoxelArray.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/VSDAData.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/VSDAData.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/SubRegion.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/Structs/SubRegion.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/Image.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/Image.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ProcessingTask.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/Task.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/Task.h
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.cpp
  ${SRC_DIR}/Core/VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h


  ${SRC_DIR}/Core/VSDA/Common/Structs/ScanRegion.cpp
  ${SRC_DIR}/Core/VSDA/Common/Structs/ScanRegion.h
  ${SRC_DIR}/Core/VSDA/Common/Structs/WorldInfo.cpp
  ${SRC_DIR}/Core/VSDA/Common/Structs/WorldInfo.h

  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshBuilder.cpp
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshBuilder.h
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshConversionHelpers.cpp
  ${SRC_DIR}/Core/VSDA/DebugHelpers/MeshConversionHelpers.h



  ${SRC_DIR}/Core/VSDA/Ca/CaRenderer.cpp
  ${SRC_DIR}/Core/VSDA/Ca/CaRenderer.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaSubRegionRenderer.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaSubRegionRenderer.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaVoxelArrayGenerator.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaVoxelArrayGenerator.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaVoxelArrayRenderer.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/CaVoxelArrayRenderer.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaMicroscopeParameters.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaMicroscopeParameters.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaData.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaData.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaSubRegion.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/Structs/CaSubRegion.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/Image.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/Image.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ProcessingTask.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ArrayGeneratorPool/Task.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ArrayGeneratorPool/Task.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ShapeToVoxel/CalciumConcentration.cpp
  ${SRC_DIR}/Core/VSDA/Ca/VoxelSubsystem/ShapeToVoxel/CalciumConcentration.h



  ${SRC_DIR}/Core/Visualizer/VisualizerRPCInterface.cpp
  ${SRC_DIR}/Core/Visualizer/VisualizerRPCInterface.h
  ${SRC_DIR}/Core/Visualizer/Visualizer.cpp
  ${SRC_DIR}/Core/Visualizer/Visualizer.h
  ${SRC_DIR}/Core/Visualizer/MeshRenderer.cpp
  ${SRC_DIR}/Core/Visualizer/MeshRenderer.h
  ${SRC_DIR}/Core/Visualizer/MeshBuilder.cpp
  ${SRC_DIR}/Core/Visualizer/MeshBuilder.h
  ${SRC_DIR}/Core/Visualizer/VisualizerParameters.cpp
  ${SRC_DIR}/Core/Visualizer/VisualizerParameters.h
  ${SRC_DIR}/Core/Visualizer/VisualizerPool.cpp
  ${SRC_DIR}/Core/Visualizer/VisualizerPool.h
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/Image.h
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/Image.cpp
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/ImageProcessorPool.h
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/ImageProcessorPool.cpp
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/ProcessingTask.h
  ${SRC_DIR}/Core/Visualizer/ImageProcessorPool/ProcessingTask.cpp

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
    unofficial::noise::noise-static

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
    unofficial::noise::noise-static

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
    NESRenderer
    GTest::gtest 
    GTest::gtest_main
    ${PROJECT_LIBRARY_NAME}
)
target_include_directories(${TEST_BINS} PUBLIC ${SRC_DIR}/Core)
target_include_directories(${TEST_BINS} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})

gtest_discover_tests(${TEST_BINS})
