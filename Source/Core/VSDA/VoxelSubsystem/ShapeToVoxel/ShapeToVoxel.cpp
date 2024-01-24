//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace VoxelArrayGenerator {




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
    assert(_Array != nullptr);
    assert(_Box != nullptr);
    assert(_VoxelScale != 0); // Will get stuck in infinite loop


    _Box->WriteToVoxelArray(_VoxelScale, _Array);

    // // 1. Get rotated point cloud.
    // std::vector<Geometries::Vec3D> point_cloud;
    // _Box->GetPointCloudFast(_VoxelScale, &point_cloud);

    // // 2. Set corresponding voxels.
    // for (const Geometries::Vec3D & p : point_cloud) {
    //     //std::cout << "Point: " << p.str() << '\n';
    //     _Array->SetVoxelAtPosition(p.x, p.y, p.z, FILLED);
    // }

    return true;
}



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG