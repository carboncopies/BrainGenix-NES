//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/CaSubRegionRenderer.h>
#include <VSDA/Ca/VoxelSubsystem/CaVoxelArrayGenerator.h>
#include <VSDA/Ca/VoxelSubsystem/CaVoxelArrayRenderer.h>

#include <VSDA/Ca/VoxelSubsystem/ShapeToVoxel/CalciumConcentration.h>

#include <Simulator/Structs/Simulation.h>



namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {



bool CaRenderSubRegion(BG::Common::Logger::LoggingSystem* _Logger, SubRegion* _SubRegion, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {
    _Logger->Log("Executing Calcium SubRegion Render For Region Starting At " + std::to_string(_SubRegion->RegionOffsetX_um) + "X, " + std::to_string(_SubRegion->RegionOffsetY_um) + "Y, Layer " + std::to_string(_SubRegion->LayerOffset), 4);


    // Get Local Variables
    Simulator::ScanRegion RequestedRegion = _SubRegion->Region;
    Simulator::Simulation* Sim = _SubRegion->Sim;
    CalciumImagingData* CaData_ = &Sim->CaData_; 
    int SliceOffset = _SubRegion->LayerOffset;
    double XOffset = _SubRegion->RegionOffsetX_um;
    double YOffset = _SubRegion->RegionOffsetY_um;


    // Setup Metadata For GetRenderStatus
    float TotalRegionThickness = abs(RequestedRegion.Point1Z_um - RequestedRegion.Point2Z_um);
    CaData_->TotalSlices_ = TotalRegionThickness / CaData_->Params_.VoxelResolution_um;


    std::vector<std::vector<float>> CalciumIndexes;
    float CalciumTimestep = 50.; // Randal - this is done since we want to know how much time passes between each step
    // That's why I had the GetCalciumConcentrationTimestep
    bool Status = VoxelArrayGenerator::CalculateCalciumConcentrations(_Logger, Sim, &CalciumIndexes);
    if (!Status) {
        return false;
    }
    // get timestep here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    CaData_->CalciumConcentrationByIndex_ = &CalciumIndexes;
    CaData_->CalciumConcentrationTimestep_ms = CalciumTimestep;

    std::cout<<"-------------------------------------------------------\n\n";
    for (unsigned int i =0; i < (*CaData_->CalciumConcentrationByIndex_)[0].size(); i++) {
        std::cout<<(*CaData_->CalciumConcentrationByIndex_)[0][i]<<", ";
    }
    std::cout<<"\n";

    // Now We Check That The Calcium Concentrations Are Generated Right
    size_t BaseSize = 0;
    if (CaData_->CalciumConcentrationByIndex_->size() > 0) {
        BaseSize = (*CaData_->CalciumConcentrationByIndex_)[0].size();
    } else {
        _Logger->Log("Error, No Calcium Concentration Generated (Have You Simulated Yet?)", 8);
        return false;
    }
    for (size_t i = 0; i < CaData_->CalciumConcentrationByIndex_->size(); i++) {
        if (BaseSize != (*CaData_->CalciumConcentrationByIndex_)[i].size()) {
            _Logger->Log("Error, Calcium Concentration Lists Are Not Same Size", 8);
            return false;
        }
    }

    // Create Voxel Array
    _Logger->Log(std::string("Creating Calcium Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
    uint64_t TargetArraySize = RequestedRegion.GetVoxelSize(CaData_->Params_.VoxelResolution_um);
    if (CaData_->Array_.get() == nullptr || CaData_->Array_->GetSize() != TargetArraySize) {
        _Logger->Log("Calcium Voxel Array Does Not Exist Yet Or Is Wrong Size, (Re)Creating Now", 2);
        CaData_->Array_ = std::make_unique<VoxelArray>(RequestedRegion, CaData_->Params_.VoxelResolution_um);
    } else {
        _Logger->Log("Reusing Existing Calcium Voxel Array, Clearing Data", 2);
        CaData_->Array_->ClearArrayThreaded(std::thread::hardware_concurrency());
        CaData_->Array_->SetBB(RequestedRegion);
    }
    CaCreateVoxelArrayFromSimulation(_Logger, Sim, &CaData_->Params_, CaData_->Array_.get(), RequestedRegion, _GeneratorPool);



    // Clear Scene In Preperation For Rendering
    unsigned int NumSlices = CaData_->Array_.get()->GetZ() / CaData_->Params_.NumVoxelsPerSlice;
    _Logger->Log("Calcium Renderer Will Render '" + std::to_string(NumSlices) + "' Slices", 3);
    for (unsigned int i = 0; i < NumSlices; i++) {
        std::string FileNamePrefix = "Simulation" + std::to_string(Sim->ID) + "/Calcium/Region" + std::to_string(CaData_->ActiveRegionID_);

        unsigned int CurrentSlice = (i + 1) * CaData_->Params_.NumVoxelsPerSlice;
        std::vector<std::string> Files = CaRenderSliceFromArray(_Logger, _SubRegion->MaxImagesX, _SubRegion->MaxImagesY, &Sim->CaData_, CaData_->Array_.get(), FileNamePrefix, CurrentSlice, _ImageProcessorPool, XOffset, YOffset, SliceOffset);
        for (size_t i = 0; i < Files.size(); i++) {
            CaData_->RenderedImagePaths_[CaData_->ActiveRegionID_].push_back(Files[i]);
        }

        
    }



    // Ensure All Tasks Are Finished
    while (_ImageProcessorPool->GetQueueSize() > 0) {

        // Calculate Desired Image Size
        // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
        // then later on, we resample it to be the right size (for the target image)
        int VoxelsPerStepX = ceil(CaData_->Params_.ImageWidth_px / CaData_->Params_.NumPixelsPerVoxel_px);
        int VoxelsPerStepY = ceil(CaData_->Params_.ImageHeight_px / CaData_->Params_.NumPixelsPerVoxel_px);
        int NumChannels = 3;
        float CameraStepSizeX_um = VoxelsPerStepX * CaData_->Params_.VoxelResolution_um;
        float CameraStepSizeY_um = VoxelsPerStepY * CaData_->Params_.VoxelResolution_um;

        double TotalSliceWidth = abs((double)CaData_->Array_->GetBoundingBox().bb_point1[0] - (double)CaData_->Array_->GetBoundingBox().bb_point2[0]);
        double TotalSliceHeight = abs((double)CaData_->Array_->GetBoundingBox().bb_point1[1] - (double)CaData_->Array_->GetBoundingBox().bb_point2[1]);
        int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
        int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);

        int ImagesPerSlice = TotalXSteps * TotalYSteps;


        // Update Current Slice Information (Account for slice numbers not starting at 0)
        CaData_->TotalSlices_ = CaData_->Array_.get()->GetZ();
        CaData_->CurrentSlice_ = CaData_->Array_.get()->GetZ() - ceil((float)_ImageProcessorPool->GetQueueSize() / ImagesPerSlice);
        CaData_->TotalSliceImages_ = ImagesPerSlice;
        CaData_->CurrentSliceImage_ = _ImageProcessorPool->GetQueueSize() % ImagesPerSlice;

        // Log Queue Size
        _Logger->Log("ImageProcessorPool Queue Length '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "'", 1);


        // Now wait a while so we don't spam the console
        std::this_thread::sleep_for(std::chrono::milliseconds(100));


    }
    for (unsigned int i = 0; i < CaData_->Tasks_.size(); i++) {
        ProcessingTask* Task = CaData_->Tasks_[i].get();
        while (Task->IsDone_ != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }


    return true;



}




}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace NES
}; // Close Namespace BG
