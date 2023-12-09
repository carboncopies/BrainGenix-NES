
#include <numbers>

#include <Simulator/Geometries/Cylinder.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors
Cylinder::Cylinder(){};

Cylinder::Cylinder(float _End0Radius_um,
                   BG::NES::Simulator::Geometries::Vec3D _End0Pos_um,
                   float _End1Radius_um,
                   BG::NES::Simulator::Geometries::Vec3D _End1Pos_um)
    : End0Radius_um(_End0Radius_um), End0Pos_um(_End0Pos_um),
      End1Radius_um(_End1Radius_um), End1Pos_um(_End1Pos_um){};

//! Renders the cylinder in 3D.
void Cylinder::Show() { return; };

//! Returns the volume of the cylinder in micrometer^3.
float Cylinder::Volume_um3() {
    float height_um = 0.0;
    float volume_um3 = 0.0;

    // Height is the Euclidian distance between the two end points.
    height_um += this->End0Pos_um.Distance(this->End1Pos_um);

    // Volume (conical frustum) = pi/3 * (R0^2 + R1^2 + R0*R1)
    // (Ref: https://mathworld.wolfram.com/ConicalFrustum.html)

    volume_um3 =
        M_PI *
        (pow(this->End0Radius_um, 2.0) + pow(this->End1Radius_um, 2.0) +
         this->End0Radius_um * this->End1Radius_um) *
        height_um / 3.0;

    return volume_um3;
};

//! Returns the distance from the origin along cylinder axis at specified
//! fraction of height distance from end 0.
float Cylinder::RAtPosition_um(float pos) {
    if (pos <= 0.0)
        return this->End0Radius_um;
    if (pos >= 1.0)
        return this->End1Radius_um;

    float rDiff = this->End1Radius_um - this->End0Radius_um;
    return this->End0Radius_um + pos * rDiff;
};

//! Returns a vector in spherical coordinates for the difference vector between the
//! two ends. This provides length and rotation values of the cylinder.
Vec3D Cylinder::difference_vector_spherical_coordinates() {
    float r = End1Pos_um.Distance(End0Pos_um);
    if (r == 0.0) {
        return Vec3D(0.0, 0.0, 0.0); // actually undefined
    }

    Vec3D diff = End1Pos_um - End0Pos_um;
    float theta = std::atan2(diff[1], diff[0]); // BEWARE: There might be illegal values!
    float phi = std::acos(diff[2] / r);

    return Vec3D(r, theta, phi);
}

// I have no clude what I'm doing
BoundingBox Cylinder::GetBoundingBox() {
	BoundingBox bb;


	return bb;
}

//! Obtain points in a disk in the y-z plane at an x position.
void add_disk_points(float _x, float _radius, float _VoxelScale, std::vector<Vec3D> & point_cloud) {
    point_cloud.emplace_back(Vec3D(_x, 0.0, 0.0));
    for (float r = _VoxelScale; r <= _radius; r += _VoxelScale) {
        float circumpherence = 2.0*std::numbers::pi<float>*r;
        float steps_in_circle = circumpherence / _VoxelScale;
        float radians_per_step = 2.0*std::numbers::pi<float> / steps_in_circle;
        for (float theta = 0; theta < 2.0*std::numbers::pi<float>; theta += radians_per_step) {
            float y = r*std::cos(theta);
            float z = r*std::sin(theta);
            point_cloud.emplace_back(_x, y, z);
        }
    }
}

//! Uses two concatenated rotation matrices to rotate a 3D point around the
//! z-axis and the y-axis. Use difference_vector_spherical_coordinates() to
//! obtain the angles.
Vec3D rotate_around_z_and_y(cont Vec3D & _point, float _zangle, float _yangle) {
    float x_rotz = _point[0]*std::cos(_zangle) - _point[1]*std::sin(_zangle);
    float y_rotz = _point[0]*std::sin(_zangle) + _point[1]*std::cos(_zangle);
    float x_rotz_roty = x_rotz*std::cos(_yangle) + point[2]*std::sin(_yangle);
    float z_roty = -x_rotz*std::sin(_yangle) + point[2]*std::cos(_yangle);
    return Vec3D(x_rotz_roty, y_rotz, z_roty);
}

//! Returns a point cloud that can be used to fill voxels representing the cylinder.
std::vector<Vec3D> Cylinder::GetPointCloud(float _VoxelScale) {
    std::vector<Vec3D> point_cloud;

    // 0. Get rotation angles and length (r, theta, phi).
    Vec3D diff_spherical_coords = difference_vector_spherical_coordinates();

    // 1. Imagine the cylinder lying flat and walk along its length.
    d = diff_spherical_coords[0];
    radius_difference = End1Radius_um - End0Radius_um;
    for (float x = 0.0; x <= d; x += _VoxelScale) {

        // 2. At each step, get points in a disk around the axis at the right radius.
        d_ratio = x / d;
        radius = End0Radius_um + d_ratio*radius_difference; // Radius at this position on the axis.
        add_disk_points(x, radius, _VoxelScale, point_cloud);

    }

    // 3. Rotate all points in the cloud to the cylinder rotation.
    std::vector<Vec3D> rotated_point_cloud;
    for (const Vec3D & p : point_cloud) {
        rotated_point_cloud.emplace_back(rotate_around_z_and_y(p, diff_spherical_coords[2], diff_spherical_coords[1]));
    }

    return rotated_point_cloud;
}

bool Cylinder::IsPointInShape(Vec3D _Position_um) {
    return false;
}


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
