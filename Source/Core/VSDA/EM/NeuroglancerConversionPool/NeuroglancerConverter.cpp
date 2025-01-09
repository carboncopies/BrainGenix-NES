//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>

#include <unistd.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <uuid.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/NeuroglancerConversionPool/NeuroglancerConverter.h>




// Returns:
//   true upon success.
//   false upon failure, and set the std::error_code & err accordingly.
// https://stackoverflow.com/questions/71658440/c17-create-directories-automatically-given-a-file-path
bool CreateDirectoryRecursive(std::string const & dirName, std::error_code & err) {
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
namespace VSDA {





bool ExecuteConversionOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, ConversionPool::ConversionPool* _ConversionPool, int _NumResolutionLevels) {

    // Check that the simulation has been initialized and everything is ready to have work done
    if (_Simulation->VSDAData_.State_ != VSDA_CONVERSION_REQUESTED) {
        return false;
    }
    _Simulation->VSDAData_.State_ = VSDA_CONVERSION_IN_PROGRESS;
    
    _Logger->Log("Executing Conversion Job For Requested Simulation", 4);


    // Unpack Variables For Easier Access
    MicroscopeParameters* Params = &_Simulation->VSDAData_.Params_;
    ScanRegion* BaseRegion = &_Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];


    // Stage 0: Firstly, we're going to create the dataset path for this conversion output 
    // Create our new dataset path, and create the directory for the output
    uuids::uuid const ThisID = uuids::uuid_system_generator{}();
    std::string UUID = uuids::to_string(ThisID);
    std::string BasePath = "NeuroglancerDatasets/" + UUID + "/";

    std::error_code Error;
    bool Status = CreateDirectoryRecursive(BasePath, Error);
    if (!Status) {
        return false;
    }



    // Stage 1: Create the metadata for the precomptued format

    /*
    Example:
    {
    "data_type": "uint8",
    "num_channels": 3,
    "scales": [
        {
        "chunk_sizes": [
            [
            1024,
            1024,
            1
            ]
        ],
        "encoding": "png",
        "key": "Data",
        "resolution": [
            20,
            20,
            40
        ],
        "size": [
            1024,
            1024,
            9
        ],
        "voxel_offset": [
            0,
            0,
            0
        ]
        }
    ],
    "type": "image"
    }
    */


    // Create the JSON info data

    // Generate Scales List
    std::vector<nlohmann::json> ScalesList;

    for (int ReductionLevel = 1; ReductionLevel <= _NumResolutionLevels; ReductionLevel++) {
        // - Create the scales list
        nlohmann::json Scales;
        Scales["encoding"] = "jpeg";
        Scales["key"] = "ReductionLevel-" + std::to_string(ReductionLevel);


        std::error_code Error;
        bool Status = CreateDirectoryRecursive(BasePath + "/ReductionLevel-" + std::to_string(ReductionLevel), Error);
        if (!Status) {
            return false;
        }

        //  - Create the chunk sizes
        std::vector<int> ChunkSizeList{Params->ImageWidth_px / ReductionLevel, Params->ImageHeight_px / ReductionLevel, 1};
        std::vector<std::vector<int>> ChunksList{ChunkSizeList};
        Scales["chunk_sizes"] = nlohmann::json(ChunksList);

        //  - Populate information about the resolution of each voxel in this output image (scaled by our total size)
        int ResX_nm = Params->VoxelResolution_um * 1000;
        int ResY_nm = Params->VoxelResolution_um * 1000;
        int ResZ_nm = (Params->SliceThickness_um / Params->VoxelResolution_um) * 1000 * Params->VoxelResolution_um;
        ResX_nm *= ReductionLevel; // Note here that resolution means the size of each voxel in nanometres
        ResY_nm *= ReductionLevel;
        // ResZ_nm *= ReductionLevel;
        std::vector<int> Resolution{ResX_nm, ResY_nm, ResZ_nm};
        Scales["resolution"] = Resolution;

        //  - Create the size values
        int ResX_px = ceil(double(BaseRegion->RegionIndexInfo_.EndX) / double(Params->ImageWidth_px)) * Params->ImageWidth_px;
        int ResY_px = ceil(double(BaseRegion->RegionIndexInfo_.EndY) / double(Params->ImageHeight_px)) * Params->ImageHeight_px;
        int ResZ_px = BaseRegion->RegionIndexInfo_.EndZ;
        ResX_px /= ReductionLevel;
        ResY_px /= ReductionLevel;
        // ResZ_px /= ReductionLevel;
        std::vector<int> Sizes{ResX_px, ResY_px, ResZ_px};
        Scales["size"] = Sizes;

        //  - Create the voxel offset values
        std::vector<int> Offset{0, 0, 0};
        Scales["voxel_offset"] = Offset;

        ScalesList.push_back(Scales);
    }
    

    nlohmann::json Info;
    Info["data_type"] = "uint8";
    Info["num_channels"] = "3";
    Info["type"] = "image";
    Info["scales"] = ScalesList;

    
    // Now, Write Info to disk, then write the provenance json too
    {
        std::string FileContents = Info.dump();
        std::ofstream File(BasePath + "info");
        File << FileContents;
    }

    {
        std::string FileContents = "{ \
  \"description\": \"\", \
  \"owners\": [], \
  \"processing\": [], \
  \"sources\": [] \
}";
        std::ofstream File(BasePath + "provenance");
        File << FileContents;
    }


    // Stage 2: Image conversion
    // Now we're going to convert all of the images
    if (BaseRegion->ImageVoxelIndexes_.size() != BaseRegion->ImageFilenames_.size()) {
        _Logger->Log("Something is seriously wrong! FilenameList Size != ImageVoxelIndexes Size", 10);
        return false;
    }

    for (size_t i = 0; i < BaseRegion->ImageVoxelIndexes_.size(); i++) {

        std::unique_ptr<ConversionPool::ProcessingTask> ThisTask = std::make_unique<ConversionPool::ProcessingTask>();

        ThisTask->IndexInfo_ = BaseRegion->ImageVoxelIndexes_[i];
        ThisTask->OutputDirectoryBasePath_ = BasePath;
        ThisTask->SourceFilePath_ = BaseRegion->ImageFilenames_[i];
        ThisTask->ReductionLevels_ = _NumResolutionLevels;

        _ConversionPool->QueueEncodeOperation(ThisTask.get());
        _Simulation->VSDAData_.ConversionTasks_.push_back(std::move(ThisTask));

    }
    

    // wait for all tasks to finish
    for (size_t i = 0; i < _Simulation->VSDAData_.ConversionTasks_.size(); i++) {
        while (!_Simulation->VSDAData_.ConversionTasks_[i]->IsDone_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }



    _Logger->Log("Generated Neuroglancer Dataset At Path " + BasePath, 5);
    BaseRegion->NeuroglancerDatasetHandle_ = UUID;

    _Simulation->VSDAData_.State_ = VSDA_RENDER_DONE;

    return true;

}




}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
