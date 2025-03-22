#pragma once
#include <VSDA/EM/MeshGenerator/MeshGeneratorPool.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {

class MeshingStage {
public:
    MeshingStage(BG::Common::Logger::LoggingSystem* logger, 
                VoxelArray* voxelArray, 
                float isolevel,
                const std::string& outputDir,
                int chunkSize = 32);
    
    void Process();  // No longer returns combined meshes

private:
    BG::Common::Logger::LoggingSystem* logger;
    VoxelArray* voxelArray;
    float isolevel;
    std::string outputDir;
    int chunkSize;
};

} // namespace Simulator
} // namespace NES
} // namespace BG