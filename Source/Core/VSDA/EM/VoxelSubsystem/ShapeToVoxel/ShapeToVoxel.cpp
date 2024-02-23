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




Geometries::Vec3D RotateAroundYZ(Geometries::Vec3D _Vec, float _RY, float _RZ) {
    return _Vec.rotate_around_y(_RY).rotate_around_z(_RZ);
}

Geometries::Vec3D RotatedVec(float _X, float _Y, float _Z, float _RY, float _RZ, Geometries::Vec3D _Translate) {
    Geometries::Vec3D NewVec(_X, _Y, _Z);
    return _Translate + RotateAroundYZ(NewVec, _RY, _RZ);
}

VoxelType GenerateVoxelColor(float _X_um, float _Y_um, float _Z_um, MicroscopeParameters* _Params, noise::module::Perlin* _Generator, int _Offset=0) {

    // Now, generate the color based on some noise constraints, Clamp it between 0 and 1, then scale based on parameters
    float SpatialScale = _Params->SpatialScale_;
    double NoiseValue = _Generator->GetValue(_X_um * SpatialScale, _Y_um * SpatialScale, _Z_um * SpatialScale);
    NoiseValue = (NoiseValue / 2.) + 0.5;
    NoiseValue *= _Params->NoiseIntensity_;

    double VoxelColorValue = _Params->DefaultIntensity_ - NoiseValue;
    VoxelColorValue += _Offset;
    VoxelColorValue = std::min(255., VoxelColorValue);
    VoxelColorValue = std::max(0., VoxelColorValue);


    VoxelType FinalVoxelValue;
    FinalVoxelValue.Intensity_ = VoxelColorValue;
    FinalVoxelValue.State_ = VOXELSTATE_INTENSITY;


    return FinalVoxelValue;

}


bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

    VoxelType FinalVoxelValue;
    FinalVoxelValue.Intensity_ = 0;
    FinalVoxelValue.State_ = BORDER;

    
    // Z Alligned Border
    for (int Z = 0; Z < _Array->GetZ(); Z++) {
        _Array->SetVoxel(0, 0, Z, FinalVoxelValue);
        _Array->SetVoxel(_Array->GetX()-1, 0, Z, FinalVoxelValue);
        _Array->SetVoxel(0, _Array->GetY()-1, Z, FinalVoxelValue);
        _Array->SetVoxel(_Array->GetX()-1, _Array->GetY()-1, Z, FinalVoxelValue);
    }

    return true;

}

bool FillBoundingBox(VoxelArray* _Array, BoundingBox* _BB, float _VoxelScale) {

    for (float X = _BB->bb_point1[0]; X < _BB->bb_point2[0]; X+= _VoxelScale) {
        for (float Y = _BB->bb_point1[1]; Y < _BB->bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = _BB->bb_point1[2]; Z < _BB->bb_point2[2]; Z+= _VoxelScale) {
                
                VoxelType FinalVoxelValue;
                FinalVoxelValue.Intensity_ = 0;
                FinalVoxelValue.State_ = BORDER;
                _Array->SetVoxelAtPosition(X, Y, Z, FinalVoxelValue);
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
                    VoxelType FinalVoxelValue = GenerateVoxelColor(X, Y, Z, _Params, _Generator);
                    _Array->SetVoxelIfNotDarker(X, Y, Z, FinalVoxelValue);
                }
            }
        }
    }

    return true;

}

bool FillCylinder(VoxelArray* _Array, Geometries::Cylinder* _Cylinder, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_Array != nullptr);
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop

    // Rotate The Endpoints Around World Origin By Amount Set In World Info
    Geometries::Vec3D RotatedEnd0 = _Cylinder->End0Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);
    Geometries::Vec3D RotatedEnd1 = _Cylinder->End1Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

    // 0. Get rotation angles and length (r, theta, phi).
    Geometries::Vec3D diff = RotatedEnd1 - RotatedEnd0;
    Geometries::Vec3D diff_spherical_coords = diff.cartesianToSpherical();
    float rot_y = diff_spherical_coords.theta();
    float rot_z = diff_spherical_coords.phi();

    float distance = diff_spherical_coords.r();
    float radius_difference = _Cylinder->End1Radius_um - _Cylinder->End0Radius_um;
    float stepsize = 0.5*_WorldInfo.VoxelScale_um;

    Geometries::Vec3D spherical_halfdist_v(distance/2.0, rot_y, rot_z);
    Geometries::Vec3D cartesian_halfdist_v = spherical_halfdist_v.sphericalToCartesian();
    Geometries::Vec3D translate = RotatedEnd0 + cartesian_halfdist_v;


    // Calculate all the points and stuff them intot he array
    for (float z = -distance/2.0; z <= distance/2.0; z += stepsize) {
        // 2. At each step, get points in a disk around the axis at the right radius.
        float d_ratio = z / distance;
        float radius = _Cylinder->End0Radius_um + d_ratio*radius_difference; // Radius at this position on the axis.

        Geometries::Vec3D RotatedPoint = RotatedVec(0.0, 0.0, z, rot_y, rot_z, translate);
        VoxelType FinalVoxelValue = GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
        _Array->SetVoxelIfNotDarker(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);

        for (float r = stepsize; r <= radius; r += stepsize) {
            float radians_per_step = stepsize / r;
            for (float theta = 0; theta < 2.0*M_PI; theta += radians_per_step) {
                float y = r*std::cos(theta);
                float x = r*std::sin(theta);
                Geometries::Vec3D RotatedPoint = RotatedVec(x, y, z, rot_y, rot_z, translate);
                VoxelType FinalVoxelValue = GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
                _Array->SetVoxelIfNotDarker(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);
            }
        }

    }





    return true;
}

bool FillBox(VoxelArray* _Array, Geometries::Box* _Box, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_Array != nullptr);
    assert(_Box != nullptr);
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop


    // Now fill it (based on the algorithm in GetPointCloud)
    float stepsize = 0.5*_WorldInfo.VoxelScale_um;

    float half_xlen = _Box->Dims_um.x / 2.0;
    float half_ylen = _Box->Dims_um.y / 2.0;
    float half_zlen = _Box->Dims_um.z / 2.0;

    for (float x = -half_xlen; x <= half_xlen; x += stepsize) {
        for (float y = -half_ylen; y <= half_ylen; y += stepsize) {
            for (float z = -half_zlen; z <= half_zlen; z += stepsize) {

                // Firstly, we create and rotate the local-space points (around object center)
                Geometries::Vec3D Point(x, y, z);
                Point = Point.rotate_around_xyz(_Box->Rotations_rad.x, _Box->Rotations_rad.y, _Box->Rotations_rad.z);

                // Now we transform and rotate around world origin
                Point = Point + _Box->Center_um;
                Point = Point.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

                // Rather than making a point cloud like before, we just write it directly into the array
                VoxelType FinalVoxelValue = GenerateVoxelColor(Point.x, Point.y, Point.z, _Params, _Generator, -180);
                _Array->SetVoxelIfNotDarker(Point.x, Point.y, Point.z, FinalVoxelValue);

            }
        }
    }



    return true;
}



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG