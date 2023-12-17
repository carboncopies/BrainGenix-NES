#include <iostream>

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
//! Returns (length, rot_x, rot_z).
//  Note: Cartesian (1,0,0) becomes Spherical (1, 1.57, 0).
//        Cartesian (0,1,0) becomes (1, 1.57, 1.57).
//        Cartesian (0,0,1) becomes (1, 0, undefined).
//        To get back to Cartesian, this is interpreted as
//        starting with a vector at (length, 0, 0), then
//        rotating rot_x around x and rot_z around z.
Vec3D Cylinder::difference_vector_spherical_coordinates() {
    // float r = End1Pos_um.Distance(End0Pos_um);
    // if (r == 0.0) {
    //     return Vec3D(0.0, 0.0, 0.0); // actually undefined
    // }

    Vec3D diff = End1Pos_um - End0Pos_um;

    return diff.cartesianToSpherical();

    // // Phi is a rotation around x (from positive z axis).
    // float phi = std::acos(diff.z_um / r);
    // std::cout << "Phi (around x): " << phi << '\n';

    // // Theta is a rotation around z (from positive x axis).
    // float theta = std::atan2(diff.y_um, diff.x_um); // BEWARE: There might be illegal values!
    // std::cout << "Theta (around z): " << theta << '\n';

    // return Vec3D(r, phi, theta); // length, rot x, rot z
}

// I have no clude what I'm doing
BoundingBox Cylinder::GetBoundingBox() {
	BoundingBox bb;


	return bb;
}

void add_disk_points(float _z, float _radius, float stepsize, std::vector<Vec3D> & point_cloud) {
    point_cloud.emplace_back(Vec3D(0.0, 0.0, _z));
    for (float r = stepsize; r <= _radius; r += stepsize) {
        //float circumpherence = 2.0*M_PI*r;
        //float steps_in_circle = circumpherence / stepsize;
        //float radians_per_step = 2.0*M_PI / steps_in_circle;
        float radians_per_step = stepsize / r;
        for (float theta = 0; theta < 2.0*M_PI; theta += radians_per_step) {
            float y = r*std::cos(theta);
            float x = r*std::sin(theta);
            point_cloud.emplace_back(x, y, _z);
        }
    }
}

//! Returns a point cloud that can be used to fill voxels representing the cylinder.
//! To use the spherical coordinates of the end-points difference
//! vector, we have to create the cylinder around the z-axis, then
//! use theta to rotate around the y-axis, then use phi to rotate
//! around the z-axis.
// for [0, 5, 0] => theta = pi/2, phi = pi/2
// from length 5 along z: [0, 0, 5]
// rotate around y by theta (pi/2): [5, 0, 0]
// rotate around z by phi (pi/2): [0, 5, 0]
// for [5, 0, 0] => theta = pi/2, phi = 0
// from length 5 along z: [0, 0, 5]
// rotate around y by theta (pi/2): [5, 0, 0]
// rotate around z by phi (0): [5, 0, 0]
// for [0, 0, 5] => theta = 0, phi = irrelevant
// from length 5 along z: [0, 0, 5]
// rotate around y by theta (0): [0, 0, 5]
// rotate around z by phi (any angle): [0, 0, 5]
std::vector<Vec3D> Cylinder::GetPointCloud(float _VoxelScale) {
    //std::cout << "-----> Calling GetPointCloud for Cylinder!\n";
    //std::cout << "-----> Cylinder end point 0: " << End0Pos_um.x_um << ',' << End0Pos_um.y_um << ',' << End0Pos_um.z_um << '\n';
    //std::cout << "-----> Cylinder end point 1: " << End1Pos_um.str() << '\n';
    std::cout << "----> Radius at point 0: " << End0Radius_um << '\n';
    std::cout << "----> Radius at point 1: " << End1Radius_um << '\n';
    std::vector<Vec3D> point_cloud;

    // 0. Get rotation angles and length (r, theta, phi).
    Vec3D diff = End1Pos_um - End0Pos_um;
    Vec3D diff_spherical_coords = diff.cartesianToSpherical();
    float rot_y = diff_spherical_coords.theta();
    float rot_z = diff_spherical_coords.phi();
    //std::cout << "Rot y: " << rot_y << '\n';
    //std::cout << "Rot z: " << rot_z << '\n';

    // 1. Imagine the cylinder lying flat along x and at the origin and walk along its length.
    float distance = diff_spherical_coords.r();
    //std::cout << "----> Length of cylinder: " << distance << '\n';
    //std::cout << "----> VoxelScale: " << _VoxelScale << '\n';
    float radius_difference = End1Radius_um - End0Radius_um;
    float stepsize = 0.5*_VoxelScale;
    for (float z = -distance/2.0; z <= distance/2.0; z += stepsize) {
        // 2. At each step, get points in a disk around the axis at the right radius.
        float d_ratio = z / distance;
        float radius = End0Radius_um + d_ratio*radius_difference; // Radius at this position on the axis.
        add_disk_points(z, radius, stepsize, point_cloud);

    }
    std::cout << "---> Number of points in point cloud: " << point_cloud.size() << '\n';

    // 3. Rotate all points in the cloud according to the cylinder rotation
    //    and translate to End0Pos_um.
    Vec3D spherical_halfdist_v(distance/2.0, rot_y, rot_z);
    Vec3D cartesian_halfdist_v = spherical_halfdist_v.sphericalToCartesian();
    //std::cout << "Cartesian half-distance: " << cartesian_halfdist_v.str() << '\n';
    Vec3D translate = End0Pos_um + cartesian_halfdist_v;
    //std::cout << "Translate: " << translate.str() << '\n';
    std::vector<Vec3D> rotated_and_translated_point_cloud;
    for (const Vec3D & p : point_cloud) { // Beware that the order of rotations is important!
        Vec3D p_roty = p.rotate_around_y(rot_y);
        rotated_and_translated_point_cloud.emplace_back(translate + p_roty.rotate_around_z(rot_z));
    }

    std::cout << "---> Number of points in rotated and translated point cloud: " << rotated_and_translated_point_cloud.size() << '\n';

    return rotated_and_translated_point_cloud;
}

bool Cylinder::IsPointInShape(Vec3D _Position_um) {
    return false;
}


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
