#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <BG/Common/Logger/Logger.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h>

namespace BG {
namespace NES {
namespace Simulator {

struct SegmentationCompressionTask : public ProcessingTask {
    std::string OutputPath_;
    VoxelArray* Voxels_;
    uint64_t ZLevel_;
    std::vector<uint8_t> CompressedData_;
    std::vector<uint64_t> BlockSize_{8, 8, 8};
};

class SegmentationCompressor {
public:
    static void CompressBlock(VoxelArray& array,
                             uint64_t blockX,
                             uint64_t blockY,
                             uint64_t blockZ,
                             const std::vector<uint64_t>& blockSize,
                             std::vector<uint8_t>& output);
                             
    static void ProcessTask(SegmentationCompressionTask* task);
};

}}}