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
                int chunkSize = 32);
    
    std::unordered_map<uint64_t, Mesh> Process();

private:
    BG::Common::Logger::LoggingSystem* logger;
    VoxelArray* voxelArray;
    float isolevel;
    int chunkSize;
};

} // namespace Simulator
} // namespace NES
} // namespace BG