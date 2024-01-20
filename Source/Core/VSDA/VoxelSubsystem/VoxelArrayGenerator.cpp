//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/VoxelArrayGenerator.h>





namespace BG {
namespace NES {
namespace Simulator {






bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

    // Z Alligned Border
    for (int Z = 0; Z < _Array->GetZ(); Z++) {
        _Array->SetVoxel(0, 0, Z, BORDER);
        _Array->SetVoxel(_Array->GetX()-1, 0, Z, BORDER);
        _Array->SetVoxel(0, _Array->GetY()-1, Z, BORDER);
        _Array->SetVoxel(_Array->GetX()-1, _Array->GetY()-1, Z, BORDER);
    }

    return true;

}

bool FillBoundingBox(VoxelArray* _Array, BoundingBox* _BB, float _VoxelScale) {

    for (float X = _BB->bb_point1[0]; X < _BB->bb_point2[0]; X+= _VoxelScale) {
        for (float Y = _BB->bb_point1[1]; Y < _BB->bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = _BB->bb_point1[2]; Z < _BB->bb_point2[2]; Z+= _VoxelScale) {
                _Array->SetVoxelAtPosition(X, Y, Z, FILLED);
            }
        }
    }

    return true;

}

bool FillShape(VoxelArray* _Array, Geometries::Geometry* _Shape, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop
    BoundingBox BB = _Shape->GetBoundingBox();

    for (float X = BB.bb_point1[0]; X < BB.bb_point2[0]; X+= _VoxelScale) {
        for (float Y = BB.bb_point1[1]; Y < BB.bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = BB.bb_point1[2]; Z < BB.bb_point2[2]; Z+= _VoxelScale) {
                if (_Shape->IsPointInShape(Geometries::Vec3D(X, Y, Z))) {
                    _Array->SetVoxelAtPosition(X, Y, Z, FILLED);
                }
            }
        }
    }

    return true;

}

// NOTE: This needs a different FillShape function, because we will not be going through the bounding box and testing if in shape.
//       Instead, we will be placing rotated points that are in the shape.
bool FillCylinder(VoxelArray* _Array, Geometries::Cylinder* _Cylinder, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // 1. Get rotated point cloud.
    std::vector<Geometries::Vec3D> point_cloud = _Cylinder->GetPointCloud(_VoxelScale);

    // 2. Set corresponding voxels.
    for (const Geometries::Vec3D & p : point_cloud) {
        _Array->SetVoxelAtPosition(p.x, p.y, p.z, FILLED);
    }

    return true;
}

bool FillBox(VoxelArray* _Array, Geometries::Box* _Box, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // 1. Get rotated point cloud.
    std::vector<Geometries::Vec3D> point_cloud = _Box->GetPointCloud(_VoxelScale);

    // 2. Set corresponding voxels.
    for (const Geometries::Vec3D & p : point_cloud) {
        //std::cout << "Point: " << p.str() << '\n';
        _Array->SetVoxelAtPosition(p.x, p.y, p.z, FILLED);
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
            FillShape(_Array, &ThisSphere, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(ThisShape)) {
            Geometries::Box & ThisBox = _Sim->Collection.GetBox(ShapeID); // std::get<Geometries::Box>(ThisShape);
            _Logger->Log("Adding Box To Voxel Array", 0);
            FillBox(_Array, &ThisBox, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsCylinder(ShapeID)) { // (std::holds_alternative<Geometries::Cylinder>(ThisShape)) {
            Geometries::Cylinder & ThisCylinder = _Sim->Collection.GetCylinder(ShapeID); // std::get<Geometries::Cylinder>(ThisShape);
            _Logger->Log("Adding Cylinder To Voxel Array", 0);
            FillCylinder(_Array, &ThisCylinder, _Params->VoxelResolution_um);
        }


    }

    CreateVoxelArrayBorderFrame(_Array);

    return true;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
