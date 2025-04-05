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
#include <VSDA/EM/NeuroglancerConversionPool/IgneousPipeline.h>



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

    // Update Status
    _Simulation->VSDAData_.CurrentOperation_ = "Converting Image Data To Neuroglancer Format";
    _Simulation->VSDAData_.TotalSliceImages_ = 0;
    _Simulation->VSDAData_.CurrentSliceImage_ = 0;
    _Simulation->VSDAData_.VoxelQueueLength_ = 0;
    _Simulation->VSDAData_.TotalVoxelQueueLength_ = 0;
    _Simulation->VSDAData_.TotalSlices_ = 0;
    _Simulation->VSDAData_.CurrentSlice_ = 0;


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

    // _NumResolutionLevels = 1;

    // Stage 1: Create the metadata for the precomptued format (images)
    {
        // Generate Scales List
        std::vector<nlohmann::json> ScalesList;

        for (int ReductionLevel = 0; ReductionLevel <= _NumResolutionLevels; ReductionLevel++) {
            // - Create the scales list
            nlohmann::json Scales;
            Scales["encoding"] = "jpeg";
            Scales["key"] = "ReductionLevel-" + std::to_string(ReductionLevel);


            std::error_code Error;
            bool Status = CreateDirectoryRecursive(BasePath + "Data/ReductionLevel-" + std::to_string(ReductionLevel), Error);
            if (!Status) {
                return false;
            }

            //  - Create the chunk sizes
            std::vector<int> ChunkSizeList{Params->ImageWidth_px / int(pow(2,ReductionLevel)), Params->ImageHeight_px / int(pow(2,ReductionLevel)), 1};
            std::vector<std::vector<int>> ChunksList{ChunkSizeList};
            Scales["chunk_sizes"] = nlohmann::json(ChunksList);

            //  - Populate information about the resolution of each voxel in this output image (scaled by our total size)
            int ResX_nm = Params->VoxelResolution_um * 1000;
            int ResY_nm = Params->VoxelResolution_um * 1000;
            int ResZ_nm = (Params->SliceThickness_um / Params->VoxelResolution_um) * 1000 * Params->VoxelResolution_um;
            ResX_nm *= pow(2,ReductionLevel); // Note here that resolution means the size of each voxel in nanometres
            ResY_nm *= pow(2,ReductionLevel);
            // ResZ_nm *= ReductionLevel;
            std::vector<int> Resolution{ResX_nm, ResY_nm, ResZ_nm};
            Scales["resolution"] = Resolution;

            //  - Create the size values
            int ResX_px = ceil(double(BaseRegion->RegionIndexInfo_.EndX) / double(Params->ImageWidth_px)) * Params->ImageWidth_px;
            int ResY_px = ceil(double(BaseRegion->RegionIndexInfo_.EndY) / double(Params->ImageHeight_px)) * Params->ImageHeight_px;
            int ResZ_px = BaseRegion->RegionIndexInfo_.EndZ;
            ResX_px /= pow(2,ReductionLevel);
            ResY_px /= pow(2,ReductionLevel);
            // ResZ_px /= ReductionLevel;
            std::vector<int> Sizes{ResX_px, ResY_px, ResZ_px};
            Scales["size"] = Sizes;

            //  - Create the voxel offset values
            std::vector<int> Offset{0, 0, 0};
            Scales["voxel_offset"] = Offset;

            // Scales["compressed_segmentation_block_size"] = std::vector<int>({8,8,8});


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
            std::ofstream File(BasePath + "Data/info");
            File << FileContents;
        }

        {
            std::string FileContents = "{ \
\"description\": \"\", \
\"owners\": [], \
\"processing\": [], \
\"sources\": [] \
}";
            std::ofstream File(BasePath + "Data/provenance");
            File << FileContents;
        }
    }

    // Stage 1.5 - Generate Segmentation Map
    {
        // Generate Scales List
        std::vector<nlohmann::json> ScalesList;

        // for (int ReductionLevel = 0; ReductionLevel < 1; ReductionLevel++) {
        int ReductionLevel = 0;
            // - Create the scales list
            nlohmann::json Scales;
            Scales["encoding"] = "compressed_segmentation";
            Scales["key"] = "Segmentation";


            Status = CreateDirectoryRecursive(BasePath + "/Segmentation/Segmentation", Error);
            if (!Status) {
                return false;
            }

            //  - Create the chunk sizes
            std::vector<int> ChunkSizeList{Params->ImageWidth_px / int(pow(2,ReductionLevel)), Params->ImageHeight_px / int(pow(2,ReductionLevel)), SEGMENTATION_BLOCK_SIZE};
            std::vector<std::vector<int>> ChunksList{ChunkSizeList};
            Scales["chunk_sizes"] = nlohmann::json(ChunksList);

            //  - Populate information about the resolution of each voxel in this output image (scaled by our total size)
            int ResX_nm = Params->VoxelResolution_um * 1000;
            int ResY_nm = Params->VoxelResolution_um * 1000;
            int ResZ_nm = (Params->SliceThickness_um / Params->VoxelResolution_um) * 1000 * Params->VoxelResolution_um;
            ResX_nm *= pow(2,ReductionLevel); // Note here that resolution means the size of each voxel in nanometres
            ResY_nm *= pow(2,ReductionLevel);
            // ResZ_nm *= ReductionLevel;
            std::vector<int> Resolution{ResX_nm, ResY_nm, ResZ_nm};
            Scales["resolution"] = Resolution;

            //  - Create the size values
            int ResX_px = ceil(double(BaseRegion->RegionIndexInfo_.EndX) / double(Params->ImageWidth_px)) * Params->ImageWidth_px;
            int ResY_px = ceil(double(BaseRegion->RegionIndexInfo_.EndY) / double(Params->ImageHeight_px)) * Params->ImageHeight_px;
            int ResZ_px = BaseRegion->RegionIndexInfo_.EndZ;
            ResX_px /= pow(2,ReductionLevel);
            ResY_px /= pow(2,ReductionLevel);
            // ResZ_px /= ReductionLevel;
            std::vector<int> Sizes{ResX_px, ResY_px, ResZ_px};
            Scales["size"] = Sizes;

            //  - Create the voxel offset values
            std::vector<int> Offset{0, 0, 0};
            Scales["voxel_offset"] = Offset;

            Scales["compressed_segmentation_block_size"] = std::vector<int>({SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE});


            ScalesList.push_back(Scales);
        // }
        

        nlohmann::json Info;
        Info["data_type"] = "uint64";
        Info["num_channels"] = "1";
        Info["type"] = "segmentation";
        Info["scales"] = ScalesList;

        // Now, Write Info to disk, then write the provenance json too
        {
            std::string FileContents = Info.dump();
            std::ofstream File(BasePath + "/Segmentation/info");
            File << FileContents;
        }

        {
            std::string FileContents = "{ \
\"description\": \"\", \
\"owners\": [], \
\"processing\": [], \
\"sources\": [] \
}";
            std::ofstream File(BasePath + "/Segmentation/provenance");
            File << FileContents;
        }


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
        ThisTask->OutputDirectoryBasePath_ = BasePath + "/Data";
        ThisTask->SourceFilePath_ = BaseRegion->ImageFilenames_[i];
        ThisTask->ReductionLevels_ = _NumResolutionLevels;
        ThisTask->IsSegmentation_ = false;

        _ConversionPool->QueueEncodeOperation(ThisTask.get());
        _Simulation->VSDAData_.ConversionTasks_.push_back(std::move(ThisTask));

    }
    

    // Stage 3: Segmap conversion
    // Now we're going to convert all of the images
    if (BaseRegion->SegmentationVoxelIndexes_.size() != BaseRegion->SegmentationFilenames_.size()) {
        _Logger->Log("Something is seriously wrong! FilenameList Size != SegmentationVoxelIndexes Size", 10);
        return false;
    }

    for (size_t i = 0; i < BaseRegion->SegmentationVoxelIndexes_.size(); i++) {

        std::unique_ptr<ConversionPool::ProcessingTask> ThisTask = std::make_unique<ConversionPool::ProcessingTask>();

        ThisTask->IndexInfo_ = BaseRegion->SegmentationVoxelIndexes_[i];
        ThisTask->OutputDirectoryBasePath_ = BasePath + "/Segmentation/";
        ThisTask->SourceFilePath_ = BaseRegion->SegmentationFilenames_[i];
        ThisTask->ReductionLevels_ = _NumResolutionLevels;
        ThisTask->IsSegmentation_ = true;

        _ConversionPool->QueueEncodeOperation(ThisTask.get());
        _Simulation->VSDAData_.ConversionTasks_.push_back(std::move(ThisTask));

    }


    // wait for all tasks to finish
    for (size_t i = 0; i < _Simulation->VSDAData_.ConversionTasks_.size(); i++) {
        while (!_Simulation->VSDAData_.ConversionTasks_[i]->IsDone_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }



    // Now run mesh generation optionally
    if (Params->GenerateMeshes && Params->GenerateSegmentation) { 

        // Update Status
        _Simulation->VSDAData_.CurrentOperation_ = "Running Igneous Pipeline Mesh Generation";
        _Simulation->VSDAData_.TotalSliceImages_ = 0;
        _Simulation->VSDAData_.CurrentSliceImage_ = 0;
        _Simulation->VSDAData_.VoxelQueueLength_ = 0;
        _Simulation->VSDAData_.TotalVoxelQueueLength_ = 0;
        _Simulation->VSDAData_.TotalSlices_ = 0;
        _Simulation->VSDAData_.CurrentSlice_ = 0;

        std::string DatasetPath = "NeuroglancerDatasets/" + UUID + "/Segmentation";
        std::string OutputPath = "Meshes/" + UUID;
        if(!ProcessIgneousPipeline(_Logger, DatasetPath, OutputPath, true, 0, std::thread::hardware_concurrency())) {
            _Logger->Log("Igneous meshing pipeline execution failed!", 10);
            return false;
        }
    }


    _Logger->Log("Generated Neuroglancer Dataset At Path " + BasePath, 5);
    BaseRegion->NeuroglancerDatasetHandle_ = UUID;

    _Simulation->VSDAData_.State_ = VSDA_RENDER_DONE;

    return true;

}


// Function to URL-encode a string
std::string urlEncode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // Keep alphanumeric characters as is
        if (isalnum(c)) {
            escaped << c;
        }
        // Any other characters are percent-encoded
        else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }

    return escaped.str();
}


std::string NGurlEncode(const std::string& value) {
    std::ostringstream encoded;
    encoded << std::hex;
    
    for (unsigned char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::uppercase << std::setw(2) << std::setfill('0') << (int)c;
        }
    }
    return encoded.str();
}

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

std::string GenerateNeuroglancerJson(const std::string& imageDatasetUrl, const std::string& segmentationDatasetUrl, bool enableSegmentation) {

    nlohmann::json neuroglancerConfig;
    
    // Set dimensions
    nlohmann::json dimensions;
    dimensions["x"] = std::vector<nlohmann::json>{100, "nm"};
    dimensions["y"] = std::vector<nlohmann::json>{100, "nm"};
    dimensions["z"] = std::vector<nlohmann::json>{200, "nm"};
    neuroglancerConfig["dimensions"] = dimensions;
    
    // Set position and orientation
    neuroglancerConfig["position"] = std::vector<double>{0.0, 0.0, 0.0};
    neuroglancerConfig["projectionOrientation"] = std::vector<double>{
        0.09116003662347794,
        0.28062376379966736,
        -0.19248539209365845,
        0.935889720916748
    };
    
    // Create layers array
    std::vector<nlohmann::json> layers;
    
    // Add microscopy image layer
    nlohmann::json imageLayer = {
        {"type", "image"},
        {"source", "precomputed://" + imageDatasetUrl},
        {"tab", "source"},
        {"name", "Microscopy Data"}
    };
    layers.push_back(imageLayer);
    
    // Conditionally add segmentation layer
    if (enableSegmentation) {
        nlohmann::json segmentationLayer = {
            {"type", "segmentation"},
            {"source", "precomputed://" + segmentationDatasetUrl},
            {"tab", "source"},
            {"name", "Ground Truth Segmentation"}
        };
        layers.push_back(segmentationLayer);
        
        // Set selected layer only when segmentation is enabled
        neuroglancerConfig["selectedLayer"] = {
            {"visible", true},
            {"layer", "Ground Truth Segmentation"}
        };
    }
    
    neuroglancerConfig["layers"] = layers;
    neuroglancerConfig["layout"] = "xy-3d";
    
    return neuroglancerConfig.dump();
}

std::string GenerateNeuroglancerURL(std::string _DatasetHandle, bool _EnableSegmentation, std::string _NeuroglancerBaseURL) {

    std::string URLBase = "URL_BASE_STRING"; // CHANGE THIS LATER TO A MACRO MAYBE "{URL_BASE_STRING}" OR SOMETHING, HAVE PYTHON CLIENT SUB IT IN!

    std::string ImgDatasetURL = URLBase + "/Dataset/" + _DatasetHandle + "/Data";
    std::string SegDatasetURL = URLBase + "/Dataset/" + _DatasetHandle + "/Segmentation";

    std::string NeuroglancerURLPrefix = _NeuroglancerBaseURL + "/#!";
    std::string NeuroglancerArgs = GenerateNeuroglancerJson(ImgDatasetURL, SegDatasetURL, _EnableSegmentation);
    std::string NeuroglancerArgsEncoded = NGurlEncode(NeuroglancerArgs);
    std::string NeuroglancerURL = NeuroglancerURLPrefix + NeuroglancerArgsEncoded;

    return NeuroglancerURL;
}



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
