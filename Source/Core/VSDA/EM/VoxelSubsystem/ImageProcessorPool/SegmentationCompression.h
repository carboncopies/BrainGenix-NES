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

// struct SegmentationCompressionTask : public ProcessingTask {
//     std::string OutputPath_;
//     VoxelArray* Voxels_;
//     uint64_t ZLevel_;
//     std::vector<uint8_t> CompressedData_;
//     std::vector<uint64_t> BlockSize_{SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE};
// };

namespace SegmentationCompressor {
    void CompressBlock(VoxelArray& array,
                        uint64_t startX, uint64_t endX,
                        uint64_t startY, uint64_t endY,
                        uint64_t startZ, uint64_t endZ,
                        std::vector<uint8_t>& output);
                             
    void ProcessTask(ProcessingTask* task);
};

}}}