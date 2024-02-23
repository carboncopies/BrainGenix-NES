//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>


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

bool FillShape(VoxelArray* _Array, Geometries::Geometry* _Shape, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop
    assert(_Params != nullptr);
    assert(_Generator != nullptr);

    BoundingBox BB = _Shape->GetBoundingBox(_WorldInfo);

    for (float X = BB.bb_point1[0]; X < BB.bb_point2[0]; X+= _WorldInfo.VoxelScale_um) {
        for (float Y = BB.bb_point1[1]; Y < BB.bb_point2[1]; Y+= _WorldInfo.VoxelScale_um) {
            for (float Z = BB.bb_point1[2]; Z < BB.bb_point2[2]; Z+= _WorldInfo.VoxelScale_um) {
                if (_Shape->IsPointInShape(Geometries::Vec3D(X, Y, Z), _WorldInfo)) {

                    // Now, generate the color based on some noise constraints, Clamp it between 0 and 1, then scale based on parameters
                    float SpatialScale = _Params->SpatialScale_;
                    double NoiseValue = _Generator->GetValue(X * SpatialScale, Y * SpatialScale, Z * SpatialScale);
                    NoiseValue = (NoiseValue / 2.) + 0.5;
                    NoiseValue *= _Params->NoiseIntensity_;

                    double VoxelColorValue = _Params->DefaultIntensity_ - NoiseValue;
                    VoxelColorValue = std::min(255., VoxelColorValue);
                    VoxelColorValue = std::max(0., VoxelColorValue);


                    // Now, limit this to 128-255, and scale accordingly
                    int FinalVoxelValue = (VoxelColorValue / 2.) + 128;
                    assert(FinalVoxelValue > VOXELSTATE_MAX_VALUE && FinalVoxelValue <= 255);
                    _Array->SetVoxelIfNotDarker(X, Y, Z, FinalVoxelValue);
                }
            }
        }
    }

    return true;

}

// NOTE: This needs a different FillShape function, because we will not be going through the bounding box and testing if in shape.
//       Instead, we will be placing rotated points that are in the shape.
bool FillCylinder(VoxelArray* _Array, Geometries::Cylinder* _Cylinder, VSDA::WorldInfo& _WorldInfo) {

    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop

    _Cylinder->WriteToVoxelArray(_Array, _WorldInfo);



    return true;
}

bool FillBox(VoxelArray* _Array, Geometries::Box* _Box, VSDA::WorldInfo& _WorldInfo) {
    assert(_Array != nullptr);
    assert(_Box != nullptr);
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop


    _Box->WriteToVoxelArray(_Array, _WorldInfo);



    return true;
}



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG