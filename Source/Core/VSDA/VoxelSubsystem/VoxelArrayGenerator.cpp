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



bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);

    _Logger->Log(std::string("Building Voxel Array For Simulation (Threaded) '") + _Sim->Name + "'", 2);


    // Create Vector to store list of tasks so we can check if they all got done at the end before returning
    std::vector<std::unique_ptr<VoxelArrayGenerator::Task>> Tasks;


    // Build Bounding Boxes For All Compartments
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];

        // Create a working task for the generatorpool to complete
        std::unique_ptr<VoxelArrayGenerator::Task> Task = std::make_unique<VoxelArrayGenerator::Task>();
        Task->Array_ = _Array;
        Task->GeometryCollection_ = &_Sim->Collection;
        Task->ShapeID_ = ThisCompartment->ShapeID;
        Task->VoxelResolution_um_ = _Params->VoxelResolution_um;


        // Now submit to render queue
        _GeneratorPool->QueueWorkOperation(Task.get());

        // Then move it to the list so we can keep track of it
        Tasks.push_back(std::move(Task));
    }

    // Add our border frame while we wait
    VoxelArrayGenerator::CreateVoxelArrayBorderFrame(_Array);


    // Okay, now we just go through the list of tasks and make sure they're all done
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
