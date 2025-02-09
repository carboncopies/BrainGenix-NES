#include "SegmentationCompression.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <cstring>




// Returns:
//   true upon success.
//   false upon failure, and set the std::error_code & err accordingly.
// https://stackoverflow.com/questions/71658440/c17-create-directories-automatically-given-a-file-path
bool CreateDirectoryRecursive2(std::string const & dirName, std::error_code & err)
{
    err.clear();
    if (!std::filesystem::create_directories(dirName, err))
    {
        if (std::filesystem::exists(dirName))
        {
            // The folder already exists:
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
                                          uint64_t blockX,
                                          uint64_t blockY, 
                                          uint64_t blockZ,
                                          const std::vector<uint64_t>& blockSize,
                                          std::vector<uint8_t>& output) {
    // Collect block data
    std::unordered_map<uint64_t, uint32_t> valueMap;
    std::vector<uint64_t> blockValues;
    
    const uint64_t maxX = std::min(blockX+blockSize[0], uint64_t(array.GetX()));
    const uint64_t maxY = std::min(blockY+blockSize[1], uint64_t(array.GetY()));
    const uint64_t maxZ = std::min(blockZ+blockSize[2], uint64_t(array.GetZ()));
    
    // Collect unique values and create mapping
    for(uint64_t z = blockZ; z < maxZ; ++z) {
        for(uint64_t y = blockY; y < maxY; ++y) {
            for(uint64_t x = blockX; x < maxX; ++x) {
                const auto& voxel = array.GetVoxel(x, y, z);
                if(valueMap.find(voxel.ParentUID) == valueMap.end()) {
                    valueMap[voxel.ParentUID] = static_cast<uint32_t>(valueMap.size());
                }
                blockValues.push_back(voxel.ParentUID);
            }
        }
    }
    
    // Determine bit size
    const uint32_t numUnique = static_cast<uint32_t>(valueMap.size());
    uint8_t bitsPerValue = 0;
    if(numUnique > 1) {
        bitsPerValue = static_cast<uint8_t>(ceil(log2(numUnique)));
        bitsPerValue = std::max<uint8_t>(bitsPerValue, 1);
    }
    
    // Pack values
    const uint64_t totalBits = blockValues.size() * bitsPerValue;
    const uint64_t totalBytes = (totalBits + 7) / 8;
    std::vector<uint8_t> packed(totalBytes, 0);
    
    uint64_t bitPos = 0;
    for(auto value : blockValues) {
        const uint32_t index = valueMap[value];
        for(uint8_t b = 0; b < bitsPerValue; ++b) {
            if(index & (1 << b)) {
                packed[bitPos/8] |= (1 << (bitPos%8));
            }
            bitPos++;
        }
    }
    
    // Create header (simple version)
    struct {
        uint32_t blockX, blockY, blockZ;
        uint16_t blockSizeX, blockSizeY, blockSizeZ;
        uint8_t bitsPerValue;
        uint32_t numValues;
    } header;
    
    // Fill header
    header.blockX = static_cast<uint32_t>(blockX);
    header.blockY = static_cast<uint32_t>(blockY);
    header.blockZ = static_cast<uint32_t>(blockZ);
    header.blockSizeX = static_cast<uint16_t>(blockSize[0]);
    header.blockSizeY = static_cast<uint16_t>(blockSize[1]);
    header.blockSizeZ = static_cast<uint16_t>(blockSize[2]);
    header.bitsPerValue = bitsPerValue;
    header.numValues = static_cast<uint32_t>(blockValues.size());
    
    // Serialize
    output.resize(sizeof(header) + packed.size());
    memcpy(output.data(), &header, sizeof(header));
    memcpy(output.data() + sizeof(header), packed.data(), packed.size());
}

void SegmentationCompressor::ProcessTask(SegmentationCompressionTask* task) {
    VoxelArray& array = *task->Voxels_;
    const auto blockSize = task->BlockSize_;
    
    const uint64_t gridX = (array.GetX() + blockSize[0] - 1) / blockSize[0];
    const uint64_t gridY = (array.GetY() + blockSize[1] - 1) / blockSize[1];
    
    std::vector<std::vector<uint8_t>> blocks;
    
    // Process all blocks at this Z level
    for(uint64_t y = 0; y < gridY; ++y) {
        for(uint64_t x = 0; x < gridX; ++x) {
            std::vector<uint8_t> blockData;
            CompressBlock(array, 
                         x * blockSize[0],
                         y * blockSize[1],
                         task->ZLevel_,
                         blockSize,
                         blockData);
            blocks.emplace_back(std::move(blockData));
        }
    }
    
    // Save to disk (simple concatenation)
    // In real implementation, use proper file format
    // std::cout<<"Making File, "<<std::to_string(blocks.size())<<"blocks to: "<<task->OutputPath_ + "/Segmentation"<<std::endl;
    std::error_code Code;
    if (!CreateDirectoryRecursive2(task->OutputPath_, Code)) {
        std::cerr<<"Failed To Create Directory!!!!!";
        return;
    }

    std::ofstream out(task->OutputPath_ + "/Segmentation", std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << task->OutputPath_ + "/Segmentation" << std::endl;
        return;
    }

    for (const auto& block : blocks) {
        out.write(reinterpret_cast<const char*>(block.data()), block.size());   
        if (!out) {
            std::cerr << "Failed to write to file." << std::endl;
            out.close();
            return;
        }
    }

    out.close();

    task->IsDone_ = true;
}

}}}