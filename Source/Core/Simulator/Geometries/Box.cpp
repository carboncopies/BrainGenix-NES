#include <vector>

#include <Simulator/Geometries/Box.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors

Box::Box(){};

Box::Box(Vec3D _Center_um, Vec3D _Dims_um) {
    this->Center_um = _Center_um;
    this->Dims_um = _Dims_um;
};

Box::Box(Vec3D _Center_um, Vec3D _Dims_um, Vec3D _Rotations_rad) {
    this->Center_um = _Center_um;
    this->Dims_um = _Dims_um;
    this->Rotations_rad = _Rotations_rad;
};

//! Renders the box in 3D.
void Box::Show() { return; };

//! Returns the volume of the box in micrometer^3.
float Box::Volume_um3() {
    return this->Dims_um.x * this->Dims_um.y * this->Dims_um.z;
};

//! Returns 1 of n equally sliced subpartitions of the Box
//! shape, when lined up from left to right along the width.
std::vector<std::vector<float>> Box::EqualSliceBounds(int nSlices, int slice) {
    std::vector<float> topLeft = {0.0, 0.0, 0.0};
    std::vector<float> bottomRight = {0.0, 0.0, 0.0};
    float pWidth = 0.0, y0 = 0.0;

    assert(slice <= nSlices);

    pWidth = this->Dims_um.y / nSlices;
    y0 = this->Center_um.y - this->Dims_um.y / 2.0;

    topLeft[0] = this->Center_um.x - this->Dims_um.x / 2.0;
    topLeft[1] = y0 + slice * pWidth;
    topLeft[2] = this->Center_um.z - this->Dims_um.z / 2.0;

    bottomRight[0] = this->Center_um.x + this->Dims_um.x / 2.0;
    bottomRight[1] = y0 + (slice + 1.0) * pWidth;
    bottomRight[2] = this->Center_um.z + this->Dims_um.z / 2.0;

    std::vector<std::vector<float>> bounds = {topLeft, bottomRight};
    return bounds;
}

//! Returns a vector of dimensions of the Box.
std::vector<float> Box::Sides() {
    std::vector<float> sides = {this->Dims_um.x, this->Dims_um.y,
                                this->Dims_um.z};
    return sides;
}


// This can be bad - if it's rotated, the bounding box is wrong!
BoundingBox Box::GetBoundingBox() {
	BoundingBox BB;

    BB.bb_point1[0] = Center_um.x_um - (Dims_um.x_um / 2);
    BB.bb_point1[1] = Center_um.y_um - (Dims_um.y_um / 2);
    BB.bb_point1[2] = Center_um.z_um - (Dims_um.z_um / 2);

    BB.bb_point2[0] = Center_um.x_um + (Dims_um.x_um / 2);
    BB.bb_point2[1] = Center_um.y_um + (Dims_um.y_um / 2);
    BB.bb_point2[2] = Center_um.z_um + (Dims_um.z_um / 2);

	return BB;
}

// Same here, we need to do this right.
bool Box::IsPointInShape(Vec3D _Position_um) {
    

    // https://stackoverflow.com/questions/52673935/check-if-3d-point-inside-a-box

    Vec3D Pos1 = Center_um - (Dims_um/2);
    Vec3D Pos2 = Center_um + (Dims_um/2);

    return _Position_um.AllElementsGTE(Pos1) && _Position_um.AllElementsLTE(Pos2);

}

//! Obtain points in a rectangle in the y-z plane at an x position.
void add_rectangle_points(float _x, float _ylen, float _zlen, float _VoxelScale, std::vector<Vec3D> & point_cloud) {
    float half_ylen = _ylen / 2.0;
    float half_zlen = _zlen / 2.0;
    for (float y = -half_ylen; y <= half_ylen; y += _VoxelScale) {
        for (float z = -half_zlen; z <= half_zlen; z += _VoxelScale) {
            point_cloud.emplace_back(_x, y, z);
        }
    }
}

// //! Uses three concatenated rotation matrices to rotate a 3D point around the
// //! x-axiz, y_axis and z-axis.
// Vec3D rotate_around_xyz(const Vec3D & _point, float _xangle, float _yangle, float _zangle) {
//     float x_rotz = _point.x_um*std::cos(_zangle) - _point.y_um*std::sin(_zangle);
//     float y_rotz = _point.x_um*std::sin(_zangle) + _point.y_um*std::cos(_zangle);
//     float x_rotz_roty = x_rotz*std::cos(_yangle) + _point.z_um*std::sin(_yangle);
//     float z_roty = -x_rotz*std::sin(_yangle) + _point.z_um*std::cos(_yangle);
//     float y_rotz_rotx = y_rotz*std::cos(_xangle) - z_roty*std::sin(_xangle);
//     float z_roty_rotx = y_rotz*std::sin(_xangle) + z_roty*std::cos(_xangle);
//     return Vec3D(x_rotz_roty, y_rotz_rotx, z_roty_rotx);
// }

//! Returns a point cloud that can be used to fill voxels representing the box.
std::vector<Vec3D> Box::GetPointCloud(float _VoxelScale) {
    std::vector<Vec3D> point_cloud;

    // 1. Imagine the box lying flat along x and at the origin and walk along its length.
    float d = Dims_um.x_um;
    float stepsize = 0.5*_VoxelScale;
    for (float x = 0.0; x <= d; x += stepsize) {

        // 2. At each step, get points in a rectangle around the axis at the right side lengths.
        add_rectangle_points(x, Dims_um.y_um, Dims_um.z_um, stepsize, point_cloud);

    }

    // 3. Rotate all points in the cloud according to the cylinder rotation
    //    and translate to End0Pos_um.
    std::vector<Vec3D> rotated_and_translated_point_cloud;
    for (const Vec3D & p : point_cloud) {
        rotated_and_translated_point_cloud.emplace_back(Center_um + p.rotate_around_xyz(Rotations_rad.x_um, Rotations_rad.y_um, Rotations_rad.z_um));
    }

    return rotated_and_translated_point_cloud;
}

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
