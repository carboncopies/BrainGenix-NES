#include "SegmentationCompression.h"

#include "seung_compress_segmentation.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cmath>

#pragma pack(push, 1)
struct BlockHeader {
    uint64_t startX;
    uint64_t startY;
    uint64_t startZ;
    uint64_t sizeX;
    uint64_t sizeY;
    uint64_t sizeZ;
    uint8_t bitsPerValue;
    uint64_t numValues;
};
#pragma pack(pop)

bool CreateDirectoryRecursive2(std::string const& dirName, std::error_code& err) {
    err.clear();
    if (!std::filesystem::create_directories(dirName, err)) {
        if (std::filesystem::exists(dirName)) {
            err.clear();
            return true;
        }
        return false;
    }
    return true;
}

namespace BG {
namespace NES {
namespace Simulator {

void SegmentationCompressor::CompressSegmentationRegion(VoxelArray& _Array, uint64_t _StartX, uint64_t _EndX, uint64_t _StartY, uint64_t _EndY, uint64_t _StartZ, uint64_t _EndZ, std::vector<uint32_t>* _Output) {

    // Calculate dimensions (assuming _End > _Start)
    uint64_t TotalXVals = _EndX - _StartX;
    uint64_t TotalYVals = _EndY - _StartY;
    uint64_t TotalZVals = _EndZ - _StartZ;
    uint64_t Channels = 1; // Add channel dimension

    uint64_t TotalValues = TotalXVals * TotalYVals * TotalZVals * Channels;

    std::unique_ptr<uint64_t[]> SegMapData = std::make_unique<uint64_t[]>(TotalValues);

    // 4D strides (C-order: X, Y, Z, Channel)
    uint64_t XStride = TotalYVals * TotalZVals * Channels;
    uint64_t YStride = TotalZVals * Channels;
    uint64_t ZStride = Channels;
    uint64_t ChannelStride = 1;

    // Populate data (assuming channel=0)
    for (uint64_t X = _StartX; X < _EndX; X++) {
        for (uint64_t Y = _StartY; Y < _EndY; Y++) {
            for (uint64_t Z = _StartZ; Z < _EndZ; Z++) {
                uint64_t index = (X - _StartX) * XStride + (Y - _StartY) * YStride + (Z - _StartZ) * ZStride + 0 * ChannelStride; // Channel index 0
                VoxelType Vox = _Array.GetVoxel(X, Y, Z);
                if (Vox.State_ != VoxelState_EMPTY) {
                    SegMapData[index] = Vox.ParentUID;
                } else {
                    SegMapData[index] = 0;
                }
            }
        }
    }

    // Pass 4D parameters to backend
    ptrdiff_t Strides[4] = {static_cast<ptrdiff_t>(XStride), 
                            static_cast<ptrdiff_t>(YStride), 
                            static_cast<ptrdiff_t>(ZStride), 
                            static_cast<ptrdiff_t>(ChannelStride)};
    ptrdiff_t Volume[4] = {static_cast<ptrdiff_t>(TotalXVals),
                           static_cast<ptrdiff_t>(TotalYVals),
                           static_cast<ptrdiff_t>(TotalZVals),
                           static_cast<ptrdiff_t>(Channels)};
    ptrdiff_t BlockSize[3] = {SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE};

    int Status = compress_segmentation::CompressChannels<uint64_t>(
        SegMapData.get(), 
        Strides, 
        Volume, 
        BlockSize, 
        _Output
    );
    assert(Status == 0);
}


void SegmentationCompressor::ProcessTask(ProcessingTask* task) {
    // try {
        VoxelArray& array = *task->Array_;
        std::vector<uint32_t> finalOutput;

        // Compress specified region directly
        std::vector<uint32_t> blockData;
        // blockData.resize(8192);
        CompressSegmentationRegion(array, (uint64_t)task->VoxelStartingX, (uint64_t)task->VoxelEndingX, (uint64_t)task->VoxelStartingY, (uint64_t)task->VoxelEndingY, (uint64_t)task->VoxelZ, (uint64_t)task->VoxelZ + SEGMENTATION_BLOCK_SIZE, &blockData);

        // Write to file
        std::error_code ec;
        if (!CreateDirectoryRecursive2(task->OutputPath_, ec)) {
            throw std::runtime_error("Failed to create directory: " + ec.message());
        }

        const std::string outputPath = task->TargetDirectory_ + task->TargetFileName_;
        std::ofstream out(outputPath, std::ios::binary);
        if (!out) throw std::runtime_error("Failed to open output file");

        out.write(reinterpret_cast<const char*>(blockData.data()), blockData.size() * sizeof(uint32_t));
        if (!out) throw std::runtime_error("Failed to write to file");

        task->IsDone_ = true;
    // } catch (const std::exception& e) {
    //     std::cerr << "Processing failed: " << e.what() << std::endl;
    //     task->IsDone_ = false;
    // }
}

}}} // namespace BG::NES::Simulator