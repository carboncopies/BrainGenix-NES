//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {
namespace VoxelArrayGenerator {




bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

    VoxelType Voxel;
    Voxel.IsBorder_ = true;

    // Z Alligned Border
    for (int Z = 0; Z < _Array->GetZ(); Z++) {
        _Array->SetVoxel(0, 0, Z, Voxel);
        _Array->SetVoxel(_Array->GetX()-1, 0, Z, Voxel);
        _Array->SetVoxel(0, _Array->GetY()-1, Z, Voxel);
        _Array->SetVoxel(_Array->GetX()-1, _Array->GetY()-1, Z, Voxel);
    }

    return true;

}


bool FillShape(VoxelArray* _Array, Simulator::Geometries::Geometry* _Shape, float _VoxelScale, size_t _CompartmentID) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop
    Simulator::BoundingBox BB = _Shape->GetBoundingBox();

    for (float X = BB.bb_point1[0]; X < BB.bb_point2[0]; X+= _VoxelScale) {
        for (float Y = BB.bb_point1[1]; Y < BB.bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = BB.bb_point1[2]; Z < BB.bb_point2[2]; Z+= _VoxelScale) {
                if (_Shape->IsPointInShape(Simulator::Geometries::Vec3D(X, Y, Z))) {
                    VoxelType ThisVoxel;
                    ThisVoxel.IsFilled_ = true;
                    ThisVoxel.CompartmentID_ = _CompartmentID;
                    _Array->SetVoxelAtPosition(X, Y, Z, ThisVoxel);
                }
            }
        }
    }

    return true;

}

// NOTE: This needs a different FillShape function, because we will not be going through the bounding box and testing if in shape.
//       Instead, we will be placing rotated points that are in the shape.
bool FillCylinder(VoxelArray* _Array, Simulator::Geometries::Cylinder* _Cylinder, float _VoxelScale, size_t _CompartmentID) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // Setup this voxel so we know what to fill
    VoxelType ThisVoxel;
    ThisVoxel.IsFilled_ = true;
    ThisVoxel.CompartmentID_ = _CompartmentID;

    // Now Go And Fill It
    _Cylinder->WriteToVoxelArray(_VoxelScale, _Array, ThisVoxel);

    return true;
}

bool FillBox(VoxelArray* _Array, Simulator::Geometries::Box* _Box, float _VoxelScale, size_t _CompartmentID) {
    assert(_Array != nullptr);
    assert(_Box != nullptr);
    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // Setup this voxel so we know what to fill
    VoxelType ThisVoxel;
    ThisVoxel.IsFilled_ = true;
    ThisVoxel.CompartmentID_ = _CompartmentID;

    // Now Go And Fill It
    _Box->WriteToVoxelArray(_VoxelScale, _Array, ThisVoxel);


    return true;
}



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG