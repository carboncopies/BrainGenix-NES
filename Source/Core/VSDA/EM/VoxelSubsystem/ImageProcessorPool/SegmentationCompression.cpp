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

    // Step 1, Create an array of uint64_t values from the main array
    int TotalXVals = std::abs((int)_StartX - (int)_EndX);
    int TotalYVals = std::abs((int)_StartY - (int)_EndY);
    int TotalZVals = std::abs((int)_StartZ - (int)_EndZ);
    uint64_t TotalValues = TotalXVals * TotalYVals * TotalZVals;

    uint64_t* SegMapData = new uint64_t[TotalValues];

    // Calculate strides for x,y,z indexes
    uint64_t XStride_Indices = TotalYVals * TotalZVals;
    uint64_t YStride_Indices = TotalZVals;
    uint64_t ZStride_Indices = 1;

    // Populate parent segmap array from voxel array
    for (uint64_t X = _StartX; X < _EndX; X++) {
        for (uint64_t Y = _StartY; Y < _EndY; Y++) {
            for (uint64_t Z = _StartZ; Z < _EndZ; Z++) {
                uint64_t CurrentIndex = (XStride_Indices * X) + (YStride_Indices * Y) + (ZStride_Indices * Z);
                SegMapData[CurrentIndex] = _Array.GetVoxel(X, Y, Z).ParentUID;
            }
        }
    }
   

    // Step 2, use seung_labs' compress block function
    ptrdiff_t Strides[3] = {(long)XStride_Indices, (long)YStride_Indices, (long)ZStride_Indices};
    ptrdiff_t Volume[3] = {TotalXVals, TotalYVals, TotalZVals};
    ptrdiff_t BlockSize[3] = {SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE};

    int Status = compress_segmentation::CompressChannel(SegMapData, Strides, Volume, BlockSize, _Output);
    assert(Status == 0);


    // Step 3, free uint64_t array
    delete SegMapData;

}


void SegmentationCompressor::ProcessTask(ProcessingTask* task) {
    // try {
        VoxelArray& array = *task->Voxels_;
        std::vector<uint32_t> finalOutput;

        // Compress specified region directly
        std::vector<uint32_t> blockData;
        // blockData.resize(8192);
        CompressSegmentationRegion(array, (uint64_t)task->VoxelStartingX, (uint64_t)task->VoxelEndingX, (uint64_t)task->VoxelStartingY, (uint64_t)task->VoxelEndingY, (uint64_t)task->ZLevel_, (uint64_t)task->ZLevel_ + SEGMENTATION_BLOCK_SIZE, &blockData);

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