#pragma once

#include <VSDA/EM/MeshGenerator/MarchingCubes.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>


namespace BG {
namespace NES {
namespace Simulator {

struct MeshTask {
    VoxelArray* voxelArray;
    int startX, startY, startZ;
    int endX, endY, endZ;
    float isolevel;

    std::unordered_map<uint64_t, Mesh> Execute() const {
        MarchingCubes mc;
        return mc.GenerateMeshes(voxelArray, startX, startY, startZ, 
                                endX, endY, endZ, isolevel);
    }
};

} // namespace Simulator
} // namespace NES
} // namespace BG