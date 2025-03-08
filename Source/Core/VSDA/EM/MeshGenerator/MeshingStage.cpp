#include <VSDA/EM/MeshGenerator/MeshingStage.h>
#include <VSDA/EM/MeshGenerator/MeshCombiner.h>
#include <VSDA/EM/MeshGenerator/OBJWriter.h>

#include <vector>
#include <future>


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

    logger->Log("Starting meshing process for volume (" + 
                std::to_string(sizeX) + "x" + 
                std::to_string(sizeY) + "x" + 
                std::to_string(sizeZ) + ")", 2);

    MeshGeneratorPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<std::unordered_map<uint64_t, Mesh>>> futures;

    // Chunk processing
    int chunkIndex = 0;
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
                
                logger->Log("Submitted chunk " + std::to_string(chunkIndex) + 
                            ": [" + std::to_string(x) + "-" + std::to_string(endX) + "]x" +
                            std::to_string(y) + "-" + std::to_string(endY) + "]x" +
                            std::to_string(z) + "-" + std::to_string(endZ) + "]", 5);
                chunkIndex++;
            }
        }
    }

    // Write OBJ files for each chunk
    for (size_t i = 0; i < futures.size(); ++i) {
        auto chunkResult = futures[i].get();
        ChunkedOBJWriter::WriteChunk(outputDir, i, chunkResult, logger, false); // Set true for debug cubes
    }

    logger->Log("Meshing completed. Files written to: " + outputDir, 2);
}

} // namespace Simulator
} // namespace NES
} // namespace BG