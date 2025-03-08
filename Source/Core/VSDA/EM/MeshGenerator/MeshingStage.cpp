#include <VSDA/EM/MeshGenerator/MeshingStage.h>
#include <VSDA/EM/MeshGenerator/MeshCombiner.h>
#include <vector>
#include <future>

namespace BG {
namespace NES {
namespace Simulator {

MeshingStage::MeshingStage(BG::Common::Logger::LoggingSystem* logger,
                          VoxelArray* voxelArray,
                          float isolevel,
                          int chunkSize)
    : logger(logger), voxelArray(voxelArray), 
      isolevel(isolevel), chunkSize(chunkSize) {}

std::unordered_map<uint64_t, Mesh> MeshingStage::Process() {
    int sizeX, sizeY, sizeZ;
    voxelArray->GetSize(&sizeX, &sizeY, &sizeZ);

    MeshGeneratorPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<std::unordered_map<uint64_t, Mesh>>> futures;

    // Chunk processing
    for (int x = 0; x < sizeX; x += chunkSize) {
        int endX = std::min(x + chunkSize, sizeX);
        for (int y = 0; y < sizeY; y += chunkSize) {
            int endY = std::min(y + chunkSize, sizeY);
            for (int z = 0; z < sizeZ; z += chunkSize) {
                int endZ = std::min(z + chunkSize, sizeZ);

                MeshTask task{
                    voxelArray,
                    x, y, z,
                    endX, endY, endZ,
                    isolevel
                };
                futures.push_back(pool.SubmitTask(task));
            }
        }
    }

    // Combine results
    std::unordered_map<uint64_t, Mesh> combinedMeshes;
    for (auto& future : futures) {
        auto chunkResult = future.get();
        for (auto& [uid, mesh] : chunkResult) {
            MeshCombiner::Combine(combinedMeshes[uid], mesh);
        }
    }

    logger->Log("Meshing completed with " + 
               std::to_string(combinedMeshes.size()) + 
               " unique neurons", 2);
    
    return combinedMeshes;
}

} // namespace Simulator
} // namespace NES
} // namespace BG