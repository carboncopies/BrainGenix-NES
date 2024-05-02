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

#include <VSDA/EM/VoxelSubsystem/TearGenerator.h>





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


std::vector<Geometries::Vec3D> SubdivideLine(Geometries::Vec3D Point1, Geometries::Vec3D Point2, int NumPoints) {
    std::vector<Geometries::Vec3D> segments;

    float deltaX = Point2.x - Point1.x;
    float deltaY = Point2.y - Point1.y;
    float deltaZ = Point2.z - Point1.z;

    float segmentLength = sqrt(deltaX * deltaX * deltaX + deltaY * deltaZ);

    float step = segmentLength / NumPoints;

    float x = Point1.x;
    float y = Point1.y;
    float z = Point1.z;

    for (int i = 0; i < NumPoints; i++) {

        Geometries::Vec3D segment;
        segment.x = x + (i / NumPoints * deltaX);
        segment.y = y + (i / NumPoints * deltaY);
        segment.z = z + (i / NumPoints * deltaZ);
        segments.push_back(segment);

    }

    return segments;
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
            
            
            // Check if we need to render this in parts

            // Check Volume of Shape if it's a cylinder, (for optional subdivision)
            uint64_t SubdivisionThreshold_vox = 250000; 
            if (_Sim->Collection.IsSphere(ThisCompartment->ShapeID)) {

                // Calculate Size in voxels of the shape
                Geometries::Sphere & ThisSphere = _Sim->Collection.GetSphere(ThisCompartment->ShapeID);
                uint64_t EstimatedSize_vox = pow(ThisSphere.Radius_um / _Params->VoxelResolution_um, 3);

                // Now check if the sphere should be broken up
                if (EstimatedSize_vox > SubdivisionThreshold_vox) {

                    // subdivide the cylinder into segments until it's shorter than the threshold number of voxels
                    int NumSegments = ceil(double(EstimatedSize_vox) / double(SubdivisionThreshold_vox));
                    _Logger->Log("Detected Sphere of Size " + std::to_string(EstimatedSize_vox) + "vox, Subdividing Into " + std::to_string(NumSegments) + " Segments", 2);

                    // now, create a task for each of these
                    // note that we assume the PointList has at least two segments in it, else it will crash
                    for (unsigned int i = 0; i < NumSegments; i++) {

                        std::unique_ptr<VoxelArrayGenerator::Task> Task = std::make_unique<VoxelArrayGenerator::Task>();
                        Task->Array_ = _Array;
                        Task->GeometryCollection_ = &_Sim->Collection;
                        Task->ShapeID_ = -1;
                        Task->CustomShape_ = VoxelArrayGenerator::CUSTOM_SPHERE;
                        Task->WorldInfo_ = Info;
                        Task->Parameters_ = _Params;

                        Task->CustomSphere_ = ThisSphere;

                        Task->CustomThisComponent = i;
                        Task->CustomTotalComponents = NumSegments;

                        // Now, enqueue it
                        _GeneratorPool->QueueWorkOperation(Task.get());

                        // Then move it to the list so we can keep track of it
                        Tasks.push_back(std::move(Task));


                    }
                    
                    AddedShapes++;


                    // skip the rest of this loop - we don't want to add the shape we just subdividied
                    continue;

                }

            } 
            // else if (_Sim->Collection.IsCylinder(ThisCompartment->ShapeID)) {

            //     // Calculate size of the cylinder in question
            //     Geometries::Cylinder& ThisCylinder = _Sim->Collection.GetCylinder(ThisCompartment->ShapeID);

            //     double AverageRadius_um = (ThisCylinder.End0Radius_um + ThisCylinder.End1Radius_um) / 2.;
            //     double Distance_um = ThisCylinder.End0Pos_um.Distance(ThisCylinder.End1Pos_um);
            //     double Volume_um3 = pow(AverageRadius_um * 3.14159, 2) * Distance_um;

            //     double Voxel_um3 = pow(_Params->VoxelResolution_um, 3);

            //     uint64_t EstimatedSize_vox = Volume_um3 / Voxel_um3;
                

            //     // Now, if the cylinder is too big to fit properly, we're going to subdivide it
            //     if (EstimatedSize_vox > SubdivisionThreshold_vox) {

            //         // subdivide the cylinder into segments until it's shorter than the threshold number of voxels
            //         int NumSegments = ceil(double(EstimatedSize_vox) / double(SubdivisionThreshold_vox));
            //         _Logger->Log("Detected Cylinder of Size " + std::to_string(EstimatedSize_vox) + "vox, Subdividing Into " + std::to_string(NumSegments) + " Segments", 2);
            //         std::vector<Geometries::Vec3D> PointList = SubdivideLine(ThisCylinder.End0Pos_um, ThisCylinder.End1Pos_um, NumSegments);

            //         // now, create a task for each of these
            //         // note that we assume the PointList has at least two segments in it, else it will crash
            //         for (unsigned int i = 0; i < NumSegments - 1; i++) {

            //             std::unique_ptr<VoxelArrayGenerator::Task> Task = std::make_unique<VoxelArrayGenerator::Task>();
            //             Task->Array_ = _Array;
            //             Task->GeometryCollection_ = &_Sim->Collection;
            //             Task->ShapeID_ = -1;
            //             Task->CustomShape_ = VoxelArrayGenerator::CUSTOM_CYLINDER;
            //             Task->WorldInfo_ = Info;
            //             Task->Parameters_ = _Params;

            //             Task->CustomCylinder_.End0Pos_um = PointList[i];
            //             Task->CustomCylinder_.End0Radius_um = ThisCylinder.End0Radius_um; // this is wrong, we need to interpolate it - fix later!
            //             Task->CustomCylinder_.End1Pos_um = PointList[i+1];
            //             Task->CustomCylinder_.End1Radius_um = ThisCylinder.End1Radius_um; // this is also wrong, we need to interpolate it - fix later!

            //             // Now, enqueue it
            //             _GeneratorPool->QueueWorkOperation(Task.get());

            //             // Then move it to the list so we can keep track of it
            //             Tasks.push_back(std::move(Task));


            //         }
                    
            //         AddedShapes++;


            //         // skip the rest of this loop - we don't want to add the shape we just subdividied
            //         continue;

            //     }
                
            // }


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

    // Now Add Tears
    if (_Params->TearingEnabled) {
        for (size_t z = 0; z < _Array->GetZ(); z++) {

            // Generate the number of tears per slice
            std::random_device Device;
            std::mt19937 Generator(Device());
            std::uniform_int_distribution<> Distribution(_Params->TearingNumPerSlice - _Params->TearingNumVariation, _Params->TearingNumPerSlice + _Params->TearingNumVariation);
            int NumTearsThisSlice = Distribution(Generator);

            // Now, generate the tears
            for (int i = 0; i < NumTearsThisSlice; i++) {

                GenerateTear(_Logger, Tasks, _GeneratorPool, _Region, _Params, _Array, Info, z, Generator());


            }

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
