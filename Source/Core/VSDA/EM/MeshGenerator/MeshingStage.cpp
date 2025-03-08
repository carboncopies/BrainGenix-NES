#include <VSDA/EM/MeshGenerator/MeshingStage.h>
#include <VSDA/EM/MeshGenerator/OBJWriter.h>

#define MC_IMPLEM_ENABLE
#include "MC.h" // Include the working MC implementation

namespace BG {
namespace NES {
namespace Simulator {

MeshingStage::MeshingStage(BG::Common::Logger::LoggingSystem* logger,
                           VoxelArray* voxelArray,
                           float isolevel,
                           const std::string& outputDir,
                           int chunkSize)
    : logger(logger), voxelArray(voxelArray),
      isolevel(isolevel), outputDir(outputDir),
      chunkSize(chunkSize) {}

void MeshingStage::Process() {
    int sizeX, sizeY, sizeZ;
    voxelArray->GetSize(&sizeX, &sizeY, &sizeZ);

    logger->Log("Starting meshing process...", 2);

    int chunkIndex = 0;
    for (int x = 0; x < sizeX; x += chunkSize) {
        int endX = std::min(x + chunkSize, sizeX);
        for (int y = 0; y < sizeY; y += chunkSize) {
            int endY = std::min(y + chunkSize, sizeY);
            for (int z = 0; z < sizeZ; z += chunkSize) {
                int endZ = std::min(z + chunkSize, sizeZ);

                // Extract scalar field for this chunk
                const int chunkNX = endX - x;
                const int chunkNY = endY - y;
                const int chunkNZ = endZ - z;
                std::vector<MC::MC_FLOAT> scalarField(chunkNX * chunkNY * chunkNZ);

                for (int cz = z; cz < endZ; ++cz) {
                    for (int cy = y; cy < endY; ++cy) {
                        for (int cx = x; cx < endX; ++cx) {
                            // Convert voxel data to MC's scalar field format
                            const auto& voxel = voxelArray->GetVoxel(cx, cy, cz);
                            const int idx = (cz - z) * chunkNY * chunkNX + (cy - y) * chunkNX + (cx - x);
                            scalarField[idx] = voxel.DistanceToEdge_vox_ - isolevel; // Adjust sign as needed
                        }
                    }
                }

                // Generate mesh using MC.h
                MC::mcMesh mcResult;
                MC::marching_cube(scalarField.data(), chunkNX, chunkNY, chunkNZ, mcResult);

                // Convert MC's mesh format to your Mesh structure
                std::unordered_map<uint64_t, Mesh> neuronMeshes;
                Mesh& mesh = neuronMeshes[0]; // Assign to a default UID (adjust if needed)
                for (const auto& v : mcResult.vertices) {
                    mesh.vertices.push_back({v.x, v.y, v.z});
                }
                mesh.indices = mcResult.indices;

                // Write the chunk
                ChunkedOBJWriter::WriteChunk(outputDir, chunkIndex, neuronMeshes, logger, false);
                chunkIndex++;
            }
        }
    }

    logger->Log("Meshing completed. Files written to: " + outputDir, 2);
}

} // namespace Simulator
} // namespace NES
} // namespace BG