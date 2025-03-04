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
    void CompressSegmentationRegion(VoxelArray& _Array, uint64_t _StartX, uint64_t _EndX, uint64_t _StartY, uint64_t _EndY, uint64_t _StartZ, uint64_t _EndZ, std::vector<uint32_t>* _Output);
                             
    void ProcessTask(ProcessingTask* task);
};

}}}