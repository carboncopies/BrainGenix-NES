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
#include <VSDA/VoxelSubsystem/ArrayGeneratorPool/Task.h>
#include <VSDA/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <VSDA/VoxelSubsystem/VoxelArrayGenerator.h>






namespace BG {
namespace NES {
namespace Simulator {


bool IsShapeInsideRegion(Simulation* _Sim, size_t _ShapeID, BoundingBox _Region) {

    bool IsInside = false;
    Geometries::GeometryCollection* GeometryCollection = &_Sim->Collection;
    if (GeometryCollection->IsSphere(_ShapeID)) {
        Geometries::Sphere& ThisSphere = GeometryCollection->GetSphere(_ShapeID);
        IsInside = ThisSphere.IsInsideRegion(_Region);
    }
    else if (GeometryCollection->IsBox(_ShapeID)) {
        Geometries::Box& ThisBox = GeometryCollection->GetBox(_ShapeID); 
        IsInside = ThisBox.IsInsideRegion(_Region);
    }
    else if (GeometryCollection->IsCylinder(_ShapeID)) {
        Geometries::Cylinder& ThisCylinder = GeometryCollection->GetCylinder(_ShapeID);
        IsInside = ThisCylinder.IsInsideRegion(_Region);
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
        Task->VoxelResolution_um_ = _Params->VoxelResolution_um;


        // Now submit to render queue if it's inside the region, otherwise skip it
        if (IsShapeInsideRegion(_Sim, ThisCompartment->ShapeID, RegionBoundingBox)) {
            
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


bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);

    _Logger->Log(std::string("Building Voxel Array For Simulation '") + _Sim->Name + "'", 2);

    // Build Bounding Boxes For All Compartments
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];
        size_t ShapeID = ThisCompartment->ShapeID;
        //std::variant<Geometries::Sphere, Geometries::Cylinder, Geometries::Box> ThisShape = _Sim->Collection.Geometries[ThisCompartment->ShapeID];

        if (_Sim->Collection.IsSphere(ShapeID)) { // (std::holds_alternative<Geometries::Sphere>(ThisShape)) {
            Geometries::Sphere & ThisSphere = _Sim->Collection.GetSphere(ShapeID); // std::get<Geometries::Sphere>(ThisShape);
            _Logger->Log("Adding Sphere To Voxel Array", 0);
            VoxelArrayGenerator::FillShape(_Array, &ThisSphere, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(ThisShape)) {
            Geometries::Box & ThisBox = _Sim->Collection.GetBox(ShapeID); // std::get<Geometries::Box>(ThisShape);
            _Logger->Log("Adding Box To Voxel Array", 0);
            VoxelArrayGenerator::FillBox(_Array, &ThisBox, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsCylinder(ShapeID)) { // (std::holds_alternative<Geometries::Cylinder>(ThisShape)) {
            Geometries::Cylinder & ThisCylinder = _Sim->Collection.GetCylinder(ShapeID); // std::get<Geometries::Cylinder>(ThisShape);
            _Logger->Log("Adding Cylinder To Voxel Array", 0);
            VoxelArrayGenerator::FillCylinder(_Array, &ThisCylinder, _Params->VoxelResolution_um);
        }


    }

    VoxelArrayGenerator::CreateVoxelArrayBorderFrame(_Array);

    return true;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
