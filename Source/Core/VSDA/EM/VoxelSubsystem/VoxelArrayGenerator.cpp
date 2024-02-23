//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/Task.h>
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.h>






namespace BG {
namespace NES {
namespace Simulator {


bool IsShapeInsideRegion(Simulation* _Sim, size_t _ShapeID, BoundingBox _Region, VSDA::WorldInfo _WorldInfo) {

    bool IsInside = false;
    Geometries::GeometryCollection* GeometryCollection = &_Sim->Collection;
    if (GeometryCollection->IsSphere(_ShapeID)) {
        Geometries::Sphere& ThisSphere = GeometryCollection->GetSphere(_ShapeID);
        IsInside = ThisSphere.IsInsideRegion(_Region, _WorldInfo);
    }
    else if (GeometryCollection->IsBox(_ShapeID)) {
        Geometries::Box& ThisBox = GeometryCollection->GetBox(_ShapeID); 
        IsInside = ThisBox.IsInsideRegion(_Region, _WorldInfo);
    }
    else if (GeometryCollection->IsCylinder(_ShapeID)) {
        Geometries::Cylinder& ThisCylinder = GeometryCollection->GetCylinder(_ShapeID);
        IsInside = ThisCylinder.IsInsideRegion(_Region, _WorldInfo);
    }

    return IsInside;

}

bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array, ScanRegion _Region, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);

    _Logger->Log(std::string("Building Voxel Array For Simulation (Threaded) '") + _Sim->Name + "'", 2);


    // Create Vector to store list of tasks so we can check if they all got done at the end before returning
    std::vector<std::unique_ptr<VoxelArrayGenerator::Task>> Tasks;

    // Convert to bounding box so we can optimize out extra shapes
    BoundingBox RegionBoundingBox;
    RegionBoundingBox.bb_point1[0] = _Region.Point1X_um;
    RegionBoundingBox.bb_point1[1] = _Region.Point1Y_um;
    RegionBoundingBox.bb_point1[2] = _Region.Point1Z_um;
    RegionBoundingBox.bb_point2[0] = _Region.Point2X_um;
    RegionBoundingBox.bb_point2[1] = _Region.Point2Y_um;
    RegionBoundingBox.bb_point2[2] = _Region.Point2Z_um;


    VSDA::WorldInfo Info;
    Info.VoxelScale_um = _Params->VoxelResolution_um;
    Info.WorldRotationOffsetX_rad = _Region.SampleRotationX_rad;
    Info.WorldRotationOffsetY_rad = _Region.SampleRotationY_rad;
    Info.WorldRotationOffsetZ_rad = _Region.SampleRotationZ_rad;


    // Build Bounding Boxes For All Compartments
    int AddedShapes = 0;
    int TotalShapes = 0;
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];

        TotalShapes++;

        // Create a working task for the generatorpool to complete
        std::unique_ptr<VoxelArrayGenerator::Task> Task = std::make_unique<VoxelArrayGenerator::Task>();
        Task->Array_ = _Array;
        Task->GeometryCollection_ = &_Sim->Collection;
        Task->ShapeID_ = ThisCompartment->ShapeID;
        Task->WorldInfo_ = Info;
        Task->Parameters_ = _Params;


        // Now submit to render queue if it's inside the region, otherwise skip it
        if (IsShapeInsideRegion(_Sim, ThisCompartment->ShapeID, RegionBoundingBox, Info)) {
            
            AddedShapes++;

            _GeneratorPool->QueueWorkOperation(Task.get());

            // Then move it to the list so we can keep track of it
            Tasks.push_back(std::move(Task));

        }

    }

    // Now Do It For Receptors
    for (unsigned int i = 0; i < _Sim->Receptors.size(); i++) {

        Connections::Receptor* ThisReceptor = _Sim->Receptors[i].get();

        TotalShapes++;

        // Create a working task for the generatorpool to complete
        std::unique_ptr<VoxelArrayGenerator::Task> Task = std::make_unique<VoxelArrayGenerator::Task>();
        Task->Array_ = _Array;
        Task->GeometryCollection_ = &_Sim->Collection;
        Task->ShapeID_ = ThisReceptor->ShapeID;
        Task->WorldInfo_ = Info;
        Task->Parameters_ = _Params;


        // Now submit to render queue if it's inside the region, otherwise skip it
        if (IsShapeInsideRegion(_Sim, ThisReceptor->ShapeID, RegionBoundingBox, Info)) {
            
            AddedShapes++;

            _GeneratorPool->QueueWorkOperation(Task.get());

            // Then move it to the list so we can keep track of it
            Tasks.push_back(std::move(Task));

        }

    }


    // Log some info for the ratio of added shapes
    double RatioAdded = ((double)AddedShapes / (double)TotalShapes) * 100.;
    _Logger->Log("Added " + std::to_string(RatioAdded) + "% Of Compartments To This Subregion", 4);


    // Add our border frame while we wait
    VoxelArrayGenerator::CreateVoxelArrayBorderFrame(_Array);


    // Okay, now we just go through the list of tasks and make sure they're all done
    _GeneratorPool->BlockUntilQueueEmpty(true);
    for (size_t i = 0; i < Tasks.size(); i++) {
        while (Tasks[i]->IsDone_ != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    return true;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
