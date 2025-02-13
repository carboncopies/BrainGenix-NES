#include "SegmentationCompression.h"

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

void SegmentationCompressor::CompressBlock(VoxelArray& array,
                                          uint64_t startX, uint64_t endX,
                                          uint64_t startY, uint64_t endY,
                                          uint64_t startZ, uint64_t endZ,
                                          std::vector<uint8_t>& output) {
    // Calculate actual bounds
    const uint64_t maxX = std::min(endX, static_cast<uint64_t>(array.GetX()));
    const uint64_t maxY = std::min(endY, static_cast<uint64_t>(array.GetY()));
    const uint64_t maxZ = std::min(endZ, static_cast<uint64_t>(array.GetZ()));
    
    const uint64_t sizeX = maxX > startX ? maxX - startX : 0;
    const uint64_t sizeY = maxY > startY ? maxY - startY : 0;
    const uint64_t sizeZ = maxZ > startZ ? maxZ - startZ : 0;
    const uint64_t totalVoxels = sizeX * sizeY * sizeZ;

    BlockHeader header{};
    header.startX = startX;
    header.startY = startY;
    header.startZ = startZ;
    header.sizeX = sizeX;
    header.sizeY = sizeY;
    header.sizeZ = sizeZ;
    header.numValues = totalVoxels;

    std::unordered_map<uint64_t, uint32_t> valueMap;
    std::vector<uint64_t> blockValues;
    blockValues.reserve(totalVoxels);

    // Collect unique values
    for (uint64_t z = startZ; z < maxZ; ++z) {
        for (uint64_t y = startY; y < maxY; ++y) {
            for (uint64_t x = startX; x < maxX; ++x) {
                const auto& voxel = array.GetVoxel(x, y, z);
                if (valueMap.emplace(voxel.ParentUID, static_cast<uint32_t>(valueMap.size())).second) {
                    if (valueMap.size() > (1ULL << 32)) {
                        throw std::runtime_error("Too many unique values for 32-bit indexing");
                    }
                }
                blockValues.push_back(voxel.ParentUID);
            }
        }
    }

    // Handle special cases
    const uint32_t numUnique = static_cast<uint32_t>(valueMap.size());
    uint8_t bitsPerValue = 0;
    if (numUnique > 1) {
        bitsPerValue = static_cast<uint8_t>(std::ceil(std::log2(numUnique)));
        bitsPerValue = std::max(bitsPerValue, static_cast<uint8_t>(1));
    }
    header.bitsPerValue = bitsPerValue;

    // Create value list
    std::vector<uint64_t> uniqueValues(valueMap.size());
    for (const auto& pair : valueMap) {
        uniqueValues[pair.second] = pair.first;
    }

    // Pack bits
    std::vector<uint8_t> packed;
    if (bitsPerValue > 0) {
        const uint64_t totalBits = totalVoxels * bitsPerValue;
        const size_t totalBytes = (totalBits + 7) / 8; // Round up to the nearest byte
        const size_t paddedBytes = ((totalBytes + 3) / 4) * 4; // Align to 4-byte boundary
        packed.resize(paddedBytes, 0); // Pad with zeros to ensure alignment

        uint64_t bitPos = 0;
        for (uint64_t value : blockValues) {
            uint32_t index = valueMap[value];
            for (uint8_t b = 0; b < bitsPerValue; ++b) {
                if (index & (1ULL << b)) {
                    packed[bitPos / 8] |= (1 << (bitPos % 8));
                }
                bitPos++;
            }
        }
    }

    // Serialize output
    const uint32_t numUniqueValues = static_cast<uint32_t>(uniqueValues.size());
    const size_t headerSize = sizeof(BlockHeader);
    const size_t uniqueValuesSize = uniqueValues.size() * sizeof(uint64_t);
    
    // Ensure the output buffer is aligned to 4 bytes
    const size_t totalOutputSize = headerSize + sizeof(numUniqueValues) + uniqueValuesSize + packed.size();
    const size_t paddedOutputSize = ((totalOutputSize + 3) / 4) * 4; // Align to 4-byte boundary
    output.resize(paddedOutputSize, 0); // Pad with zeros to ensure alignment
    
    uint8_t* ptr = output.data();
    memcpy(ptr, &header, headerSize);
    ptr += headerSize;
    
    memcpy(ptr, &numUniqueValues, sizeof(numUniqueValues));
    ptr += sizeof(numUniqueValues);
    
    if (!uniqueValues.empty()) {
        memcpy(ptr, uniqueValues.data(), uniqueValuesSize);
        ptr += uniqueValuesSize;
    }
    
    if (!packed.empty()) {
        memcpy(ptr, packed.data(), packed.size());
    }
}

void SegmentationCompressor::ProcessTask(ProcessingTask* task) {
    try {
        VoxelArray& array = *task->Voxels_;
        std::vector<uint8_t> finalOutput;

        // Compress specified region directly
        std::vector<uint8_t> blockData;
        CompressBlock(array,
                     task->VoxelStartingX, task->VoxelEndingX,
                     task->VoxelStartingY, task->VoxelEndingY,
                     task->ZLevel_, task->ZLevel_ + SEGMENTATION_BLOCK_SIZE,
                     blockData);

        // Write to file
        std::error_code ec;
        if (!CreateDirectoryRecursive2(task->OutputPath_, ec)) {
            throw std::runtime_error("Failed to create directory: " + ec.message());
        }

        const std::string outputPath = task->TargetDirectory_ + task->TargetFileName_;//task->OutputPath_ + "/Segmentation";
        std::ofstream out(outputPath, std::ios::binary);
        if (!out) throw std::runtime_error("Failed to open output file");
        
        out.write(reinterpret_cast<const char*>(blockData.data()), blockData.size());
        if (!out) throw std::runtime_error("Failed to write to file");
        
        task->IsDone_ = true;
    } catch (const std::exception& e) {
        std::cerr << "Processing failed: " << e.what() << std::endl;
        task->IsDone_ = false;
    }
}

}}} // namespace BG::NES::Simulator