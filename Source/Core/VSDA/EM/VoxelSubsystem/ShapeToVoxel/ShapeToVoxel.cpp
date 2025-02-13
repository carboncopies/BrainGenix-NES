//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>

/**
 * Just as a reminder of the process used below:
 * 
 * 1) In each subregion of the 3D voxel space, only thos objects that intersect with (appear in)
 *    the subregion are handled.
 * 2) For each of the remaining objects, a 3D rectangular bounding box is determined in order
 *    to reduce the number of points in the 3D voxel space that need work for that object.
 * 3) For each of those points in 3D voxel space, determine the corresponding coordinate in
 *    3D world space.
 * 4) Determine if that 3D coordinate is within the object (e.g. using math to determine if
 *    a given point is within a cylinder, or a sphere, or a rotated box).
 * 
 * World rotations of objects precede all of this, so those do not need to be applied on a
 * per voxel basis.
 */


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

// VoxelType GenerateVoxelColor(float _X_um, float _Y_um, float _Z_um, MicroscopeParameters* _Params, noise::module::Perlin* _Generator, int _Offset=0) {

//     // Now, generate the color based on some noise constraints, Clamp it between 0 and 1, then scale based on parameters
//     double NoiseValue;
//     if (_Params->GeneratePerlinNoise_) {
//         float SpatialScale = _Params->SpatialScale_;
//         NoiseValue = _Generator->GetValue(_X_um * SpatialScale, _Y_um * SpatialScale, _Z_um * SpatialScale);
//         NoiseValue = (NoiseValue / 2.) + 0.5;
//         NoiseValue *= _Params->NoiseIntensity_;
//     }

//     double VoxelColorValue = _Params->DefaultIntensity_ - NoiseValue;
//     VoxelColorValue += _Offset;
//     VoxelColorValue = std::min(255., VoxelColorValue);
//     VoxelColorValue = std::max(0., VoxelColorValue);


//     VoxelType FinalVoxelValue;
//     FinalVoxelValue.Intensity_ = VoxelColorValue;
//     FinalVoxelValue.State_ = VoxelState_INTERIOR;


//     return FinalVoxelValue;

// }


// bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

//     VoxelType FinalVoxelValue;
//     FinalVoxelValue.Intensity_ = 0;

    
//     // Z Alligned Border
//     for (int Z = 0; Z < _Array->GetZ(); Z++) {
//         _Array->SetVoxel(0, 0, Z, FinalVoxelValue);
//         _Array->SetVoxel(_Array->GetX()-1, 0, Z, FinalVoxelValue);
//         _Array->SetVoxel(0, _Array->GetY()-1, Z, FinalVoxelValue);
//         _Array->SetVoxel(_Array->GetX()-1, _Array->GetY()-1, Z, FinalVoxelValue);
//     }

//     return true;

// }



// float LinearInterpolate(float _X, float _Val1, float _Val2) {
//     return _Val1 + _X * (_Val2 - _Val1);
// }

// VoxelType CalculateBorderColor(VoxelType _Source, float _DistanceFromEdge, MicroscopeParameters* _Params) {

//     if (_DistanceFromEdge < _Params->BorderThickness_um) {
//         _Source.State_ = VoxelState_BORDER;

//         float NormalizedDistanceFromEdge = 1.0f - (_DistanceFromEdge / _Params->BorderThickness_um);
//         _Source.Intensity_ = LinearInterpolate(NormalizedDistanceFromEdge, _Source.Intensity_, _Params->BorderEdgeIntensity);
//     }

//     return _Source;

// }


bool FillSpherePart(int _TotalThreads, int _ThisThread, VoxelArray* _Array, Geometries::Sphere*_Shape, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop
    assert(_Params != nullptr);
    assert(_Generator != nullptr);


    BoundingBox BB = _Shape->GetBoundingBox(_WorldInfo);

    for (float X = BB.bb_point1[0] + (_ThisThread * _WorldInfo.VoxelScale_um); X < BB.bb_point2[0]; X+= (_TotalThreads * _WorldInfo.VoxelScale_um)) {
        for (float Y = BB.bb_point1[1]; Y < BB.bb_point2[1]; Y+= _WorldInfo.VoxelScale_um) {
            for (float Z = BB.bb_point1[2]; Z < BB.bb_point2[2]; Z+= _WorldInfo.VoxelScale_um) {
                if (_Shape->IsPointInShape(Geometries::Vec3D(X, Y, Z), _WorldInfo)) {

                    float DistanceToEdge = _Shape->Radius_um - Geometries::Vec3D(X, Y, Z).Distance(_Shape->Center_um);
                    _Array->CompositeVoxel(X, Y, Z, VoxelState_INTERIOR, DistanceToEdge, _Shape->ParentID);

                }
            }
        }
    }

    return true;

}

int isPointInCylinder(const Geometries::Vec3D& P1, const Geometries::Vec3D& P2, double r1, double r2, const Geometries::Vec3D& point) {
	// Vector from P1 to P2
	Geometries::Vec3D axis = P2 - P1;
	double height = axis.Magnitude();
	Geometries::Vec3D axisDir = axis.Normalize();
    
	// Vector from P1 to the point
	Geometries::Vec3D pointVector = point - P1;
    
	// Projection of pointVector onto the axis
	double projectionLength = pointVector.Dot(axisDir);
    
	// Check if the point is within the bounds along the axis
	if (projectionLength < 0) {
        return 1;
    }
    if (projectionLength > height) {
        return 2;
    	//return false;
	}

	// Calculate the radius at the projection point along the cylinder's height
	double currentRadius = r1 + (r2 - r1) * (projectionLength / height);

	// Closest point on the axis to the given point
	Geometries::Vec3D closestPointOnAxis = P1 + axisDir * projectionLength;

	// Distance from the point to the closest point on the axis
	double distanceToAxis = (point - closestPointOnAxis).Magnitude();
    
	// Check if the distance is within the current radius
	//return distanceToAxis <= currentRadius;
    if (distanceToAxis <= currentRadius) {
        return 0;
    }
    return 3;
}

bool FillCylinderPart(int _TotalThreads, int _ThisThread, VoxelArray* _Array, Geometries::Cylinder* _Cylinder, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_Array != nullptr);
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop

    // Rotate The Endpoints Around World Origin By Amount Set In World Info
    //   This deals with the rotation of the whole model
    Geometries::Vec3D RotatedEnd0 = _Cylinder->End0Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);
    Geometries::Vec3D RotatedEnd1 = _Cylinder->End1Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

    // Get rotation angles and length (r, theta, phi).
    //   Moving the midline of the cylinder to (0,0,0) then switching to spherical coords
    //   allows us to find angles to rotate around Y and Z axes.
    Geometries::Vec3D diff = RotatedEnd1 - RotatedEnd0;
    Geometries::Vec3D diff_spherical_coords = diff.cartesianToSpherical();
    float rot_y = diff_spherical_coords.theta();
    float rot_z = diff_spherical_coords.phi();

    float cyl_length = diff_spherical_coords.r();

    // Use this to know the extent to which to gradually change the radius as you move along the length of the cylinder.
    float radius_difference = _Cylinder->End1Radius_um - _Cylinder->End0Radius_um;
    float midpoint_radius_um = _Cylinder->End0Radius_um + (0.5*radius_difference);

    // Stepping at half voxel size ensures finding voxels without gaps.
    float stepsize = 0.5*_WorldInfo.VoxelScale_um;

    Geometries::Vec3D spherical_halfdist_v(cyl_length/2.0, rot_y, rot_z); // mid point vector (from 0,0,0)
    Geometries::Vec3D cartesian_halfdist_v = spherical_halfdist_v.sphericalToCartesian();
    Geometries::Vec3D translate = RotatedEnd0 + cartesian_halfdist_v; // actual mid point

    //for (float X = BB.bb_point1[0] + (_ThisThread * _WorldInfo.VoxelScale_um); X < BB.bb_point2[0]; X+= (_TotalThreads * _WorldInfo.VoxelScale_um)) {

    // Calculate all the points and stuff them into the array
    for (float z = -0.5*cyl_length; z <= 0.5*cyl_length; z += stepsize) {
        // 2. At each step, get points in a disk around the axis at the right radius.
        float d_ratio = z / cyl_length; // This goes from -0.5 to 0.5 as we move along the length of the cylinder.
        float radius_at_z = midpoint_radius_um + d_ratio*radius_difference; // Radius at this position on the axis.

        // Next disc center point along cylinder midline.
        Geometries::Vec3D RotatedPoint = RotatedVec(0.0, 0.0, z, rot_y, rot_z, translate);

        // Set voxel for midline point.
        // VoxelType FinalVoxelValue = GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
        // _Array->SetVoxelIfNotDarker(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);
        float DistanceToEdge = radius_at_z;
        _Array->CompositeVoxel(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, VoxelState_INTERIOR, DistanceToEdge, _Cylinder->ParentID);

        // Find points on circles around the midline up to the radius at this point along the cylinder.
        for (float r = stepsize + (_ThisThread * stepsize); r <= radius_at_z; r += (_TotalThreads * stepsize)) {
            // Circumpherence is 2*pi*r, number of voxels that fit along the circumpherence is 2*pi*r/stepsize.
            // So, in a whole 2*pi rotation, for each step the angle change is 2*pi/(2*pi*r/stepsize) = stepsize/r.
            float radians_per_step = stepsize / r;
            for (float theta = 0; theta < 2.0*M_PI; theta += radians_per_step) {

                // Next point on circumpherence at radius r.
                float y = r*std::cos(theta);
                float x = r*std::sin(theta);
                Geometries::Vec3D RotatedPoint = RotatedVec(x, y, z, rot_y, rot_z, translate);

                float DistanceToEdge = radius_at_z - r;
                _Array->CompositeVoxel(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, VoxelState_INTERIOR, DistanceToEdge, _Cylinder->ParentID);


                // // Set voxel at the point.
                // VoxelType FinalVoxelValue = GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
                // if (_Params->RenderBorders) {
                //     float DistanceToEdge = radius_at_z - r;
                //     FinalVoxelValue = CalculateBorderColor(FinalVoxelValue, DistanceToEdge, _Params);
                // }
                // _Array->SetVoxelIfNotDarker(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);
            }
        }

    }

    return true;
}



// bool FillCylinderPart(int _TotalThreads, int _ThisThread, VoxelArray* _Array, Geometries::Cylinder* _Shape, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
//     assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop
//     assert(_Params != nullptr);
//     assert(_Generator != nullptr);

//     // Code to select the lowest point in z direction as end0, other as end1
//     if (_Shape->End1Pos_um.z < _Shape->End0Pos_um.z) {
//         Geometries::Vec3D TmpEndPos = _Shape->End0Pos_um;
//         _Shape->End0Pos_um = _Shape->End1Pos_um;
//         _Shape->End1Pos_um = TmpEndPos;

//         double TmpEndRadius = _Shape->End0Radius_um;
//         _Shape->End0Radius_um = _Shape->End1Radius_um;
//         _Shape->End1Radius_um = TmpEndRadius;
//     } 

//     // Rotate The Endpoints Around World Origin By Amount Set In World Info
//     //   This deals with the rotation of the whole model
//     Geometries::Vec3D RotatedEnd0_um = _Shape->End0Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);
//     Geometries::Vec3D RotatedEnd1_um = _Shape->End1Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

//     /**
//      * New method:
//      * 1) Walk along integer Z steps of the 3D Voxel space (layers).
//      * 2) For each Z step, we know where that is in 3D float space.
//      * 3) We know a point in 3D float space (x,y,z) on the cylinder mid-line.
//      * 4) We find a 2D bounding box around that point expressed in Voxel space X,Y voxels.
//      * 5) We test each point in that voxel space to see if it is in the cylinder.
//      */
//     // Calculate all the points and stuff them into the array

//     // Firstly calculate the Z height of the start and end points on the cylinder

//     Geometries::Vec3D DifferenceVector_um = RotatedEnd1_um - RotatedEnd0_um;
//     float CylinderLength_um = RotatedEnd1_um.Distance(RotatedEnd0_um);
//     Geometries::Vec3D UnitVector = DifferenceVector_um / CylinderLength_um;
//     Geometries::Vec3D ScaledUnitVector_um = UnitVector * _Params->VoxelResolution_um;

//     float MaxRadius_um = std::max(_Shape->End0Radius_um, _Shape->End1Radius_um);
//     Geometries::Vec3D Offset_um = UnitVector * MaxRadius_um;

//     Geometries::Vec3D ShiftedEnd0_um = RotatedEnd0_um - Offset_um;
//     Geometries::Vec3D ShiftedEnd1_um = RotatedEnd1_um + Offset_um;

//     int StartZ = _Array->GetZIndexAtPosition(ShiftedEnd0_um.z - _Shape->End0Radius_um);
//     int EndZ = _Array->GetZIndexAtPosition(ShiftedEnd1_um.z + _Shape->End1Radius_um);


//     int ZDiff = EndZ - StartZ;

//     // Now enumerate the entire cylinder one z layer at a time, and test voxels in a square (of size r) at that layer
//     // Notice that this loop does not count from 0 but from StartZ! Beware of this when mapping back to world coordinates!
//     for (int CurrentZIndex = StartZ + _ThisThread; CurrentZIndex < EndZ; CurrentZIndex += _TotalThreads) {

//         int ZOffset = CurrentZIndex - StartZ;
//         float ZRatio = float(ZOffset)/float(ZDiff); // Because we can't divide by 0, we can't do this if StartZ == EndZ.
//         float CurrentRadius_um = (1.0 - ZRatio)*_Shape->End0Radius_um + ZRatio*_Shape->End1Radius_um;

//         Geometries::Vec3D CylinderMidpointAtCurrentLayer_um = ShiftedEnd0_um + (ScaledUnitVector_um * ZOffset);

//         // Now create a square at the midpoint here, and then test all points in that square
//         int XTestSpaceMin = _Array->GetXIndexAtPosition(CylinderMidpointAtCurrentLayer_um.x - MaxRadius_um);
//         int XTestSpaceMax = _Array->GetXIndexAtPosition(CylinderMidpointAtCurrentLayer_um.x + MaxRadius_um);
//         int YTestSpaceMin = _Array->GetYIndexAtPosition(CylinderMidpointAtCurrentLayer_um.y - MaxRadius_um);
//         int YTestSpaceMax = _Array->GetYIndexAtPosition(CylinderMidpointAtCurrentLayer_um.y + MaxRadius_um);

//         for (int CurrentYIndex = YTestSpaceMin; CurrentYIndex <= YTestSpaceMax; CurrentYIndex++) {
//             for (int CurrentXIndex = XTestSpaceMin; CurrentXIndex <= XTestSpaceMax; CurrentXIndex++) {

//                 Geometries::Vec3D CurrentWorldSpacePosition_um = _Array->GetPositionAtIndex(CurrentXIndex, CurrentYIndex, CurrentZIndex);

//                 int res = isPointInCylinder(RotatedEnd0_um, RotatedEnd1_um, _Shape->End0Radius_um, _Shape->End1Radius_um, CurrentWorldSpacePosition_um);
//                 if (res==0) {

//                     VoxelType FinalVoxelValue = GenerateVoxelColor(CurrentWorldSpacePosition_um.x, CurrentWorldSpacePosition_um.y, CurrentWorldSpacePosition_um.z, _Params, _Generator);
//                     if (_Params->RenderBorders) {
//                         float DistanceToCenter_um = CurrentWorldSpacePosition_um.Distance(CylinderMidpointAtCurrentLayer_um);
//                         FinalVoxelValue = CalculateBorderColor(FinalVoxelValue, CurrentRadius_um - DistanceToCenter_um, _Params);
//                     }

//                     _Array->SetVoxelAtIndex(CurrentXIndex, CurrentYIndex, CurrentZIndex, FinalVoxelValue);

//                 }

//             }
//         }

//     }

//     return true;
// }


//bool FillLine(VoxelArray* _Array, int P1X, int P1Y, int P1Thickness, int P2X, int P2Y, int P2Thickness, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
bool FillWedge(VoxelArray* _Array, Geometries::Wedge* _Wedge, VSDA::WorldInfo& _WorldInfo, MicroscopeParameters* _Params, noise::module::Perlin* _Generator) {
    assert(_Array != nullptr);
    assert(_WorldInfo.VoxelScale_um != 0); // Will get stuck in infinite loop

    // Rotate The Endpoints Around World Origin By Amount Set In World Info
    //   This deals with the rotation of the whole model
    Geometries::Vec3D RotatedEnd0 = _Wedge->End0Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);
    Geometries::Vec3D RotatedEnd1 = _Wedge->End1Pos_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

    // Get rotation angles and length (r, theta, phi).
    //   Moving the midline of the wedge to (0,0,0) then switching to spherical coords
    //   allows us to find angles to rotate around Y and Z axes.
    Geometries::Vec3D diff = RotatedEnd1 - RotatedEnd0;
    Geometries::Vec3D diff_spherical_coords = diff.cartesianToSpherical();
    float rot_y = diff_spherical_coords.theta();
    float rot_z = diff_spherical_coords.phi();

    float wedge_length = diff_spherical_coords.r();

    float width_difference = _Wedge->End1Width_um - _Wedge->End0Width_um;
    float height_difference = _Wedge->End1Height_um - _Wedge->End0Height_um;

    float midpoint_width_um = _Wedge->End0Width_um + (0.5*width_difference);
    float midpoint_height_um = _Wedge->End0Height_um + (0.5*height_difference);

    // Stepping at half voxel size ensures finding voxels without gaps.
    float stepsize = 0.5*_WorldInfo.VoxelScale_um;

    Geometries::Vec3D spherical_halfdist_v(wedge_length/2.0, rot_y, rot_z); // mid point vector (from 0,0,0)
    Geometries::Vec3D cartesian_halfdist_v = spherical_halfdist_v.sphericalToCartesian();
    Geometries::Vec3D translate = RotatedEnd0 + cartesian_halfdist_v; // actual mid point


    // Calculate all the points and stuff them into the array
    for (float z = -0.5*wedge_length; z <= 0.5*wedge_length; z += stepsize) {
        // 2. At each step, get points in a rectangle around the axis at the right radius.
        float d_ratio = z / wedge_length; // This goes from -0.5 to 0.5 as we move along the length of the wedge.
        float width_at_z = midpoint_width_um + d_ratio*width_difference; // Width at this position on the midline.
        float height_at_z = midpoint_height_um + d_ratio*height_difference; // Height at this position on the midline.

        // Next rectangle center point along wedge midline.
        Geometries::Vec3D RotatedPoint = RotatedVec(0.0, 0.0, z, rot_y, rot_z, translate);

        // Set voxel for midline point.
        // VoxelType FinalVoxelValue = GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
        VoxelType FinalVoxelValue; //GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
        // FinalVoxelValue.Intensity_ = 0;
        FinalVoxelValue.State_ = VoxelState_BLACK;
        _Array->SetVoxelAtPosition(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);
        // _Array->SetVoxelIfNotDarker(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);

        // Find points on filled rectangles around the midline up to the radius at this point along the wedge.
        for (float x = -0.5*width_at_z; x <= 0.5*width_at_z; x += stepsize) {
            for (float y = -0.5*height_at_z; y <= 0.5*height_at_z; y += stepsize) {

                // Next point in filled rectangle.
                Geometries::Vec3D RotatedPoint = RotatedVec(x, y, z, rot_y, rot_z, translate);

                // Set voxel at the point.
                VoxelType FinalVoxelValue; //GenerateVoxelColor(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, _Params, _Generator);
                // FinalVoxelValue.Intensity_ = 0;
                FinalVoxelValue.State_ = VoxelState_BLACK;
                // if (_Params->RenderBorders) {
                //     float DistanceToWidthEdge = (0.5*width_at_z) - fabs(x);
                //     float DistanceToHeightEdge = (0.5*height_at_z) - fabs(y);
                //     float DistanceToEdge = DistanceToWidthEdge < DistanceToHeightEdge ? DistanceToWidthEdge : DistanceToHeightEdge;
                //     FinalVoxelValue = CalculateBorderColor(FinalVoxelValue, DistanceToEdge, _Params);
                // }
                _Array->SetVoxelAtPosition(RotatedPoint.x, RotatedPoint.y, RotatedPoint.z, FinalVoxelValue);

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
                
                // VoxelType FinalVoxelValue = GenerateVoxelColor(Point.x, Point.y, Point.z, _Params, _Generator, -180);
                // _Array->SetVoxelIfNotDarker(Point.x, Point.y, Point.z, FinalVoxelValue);

                _Array->CompositeVoxel(x, y, z, VoxelState_BLACK, 0, _Box->ParentID);

            }
        }
    }



    return true;
}



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG