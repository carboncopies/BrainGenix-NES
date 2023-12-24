#include <iostream>

#include <Simulator/Geometries/Cylinder.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors
Cylinder::Cylinder(){

};

Cylinder::Cylinder(float _End0Radius_um, BG::NES::Simulator::Geometries::Vec3D _End0Pos_um, float _End1Radius_um, BG::NES::Simulator::Geometries::Vec3D _End1Pos_um) : End0Radius_um(_End0Radius_um), End0Pos_um(_End0Pos_um), End1Radius_um(_End1Radius_um), End1Pos_um(_End1Pos_um) {

};

//! Renders the cylinder in 3D.
void Cylinder::Show() {
    return;
};

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



//! Gets the rotation in radians with respect to the x, y and z axes
std::tuple<float, float, float> Cylinder::GetRotation_rad() {
    // Assuming initial orientation of the cylinder is such that the
    // axis is along the direction (0, 0, 1), i.e. the Z-axis

    static Geometries::Vec3D refAxisVec{0.0, 0.0, 1.0};
    static Geometries::Vec3D originVec{0.0, 0.0, 0.0};
    Geometries::Vec3D axisVec = this->End1Pos_um - this->End0Pos_um;
    Geometries::Vec3D crossProdVec{};
    float alpha_rad = 0.0;
    float thetaX_rad = 0.0, thetaY_rad = 0.0, thetaZ_rad = 0.0;
    glm::mat3 rotationMat(0.0f);
    glm::mat3 normalMat(0.0f);

    // We use the axis-angle representation, axis being the axis about which
    // the rotation must be performed and angle being the angle by which
    // the rotation must be made.

    // 1. The angle is found from the Dot product of the two vectors.
    alpha_rad = acos(axisVec.Dot(refAxisVec) / (axisVec.Distance(originVec) * refAxisVec.Distance(originVec)));

    // 2. The axis is found from the cross product of the two vectors.
    crossProdVec = refAxisVec.Cross(axisVec);
    crossProdVec = crossProdVec / crossProdVec.Distance(originVec); // Normalize

    // 3. Fill normalMat from calculated cross product vector
    normalMat[0][1] = -crossProdVec.z;
    normalMat[0][2] = crossProdVec.y;
    normalMat[1][0] = crossProdVec.z;
    normalMat[1][2] = -crossProdVec.x;
    normalMat[2][0] = -crossProdVec.y;
    normalMat[2][1] = crossProdVec.x;

    // 4. Calculate rotation matrix (using Rodrigues' rotation formula.)
    // Ref.:
    // https://math.stackexchange.com/questions/296267/how-to-find-eulers-angles
    rotationMat = glm::mat3(1.0f) + normalMat * static_cast<float>(sin(alpha_rad)) + normalMat * normalMat * static_cast<float>(1.0 - cos(alpha_rad));

    // 5. Calculate Euler angles from rotation matrix. Ref:.
    // https://stackoverflow.com/questions/15022630/how-to-calculate-the-angle-from-rotation-matrix
    thetaX_rad = atan2(rotationMat[2][1], rotationMat[2][2]);
    thetaY_rad = atan2(-rotationMat[2][0], sqrt(rotationMat[2][2] * rotationMat[2][2] + rotationMat[2][1] * rotationMat[2][1]));
    thetaZ_rad = atan2(rotationMat[1][0], rotationMat[0][0]);

    return std::make_tuple(thetaX_rad, thetaY_rad, thetaZ_rad);
};




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
    //std::cout << "-----> Cylinder end point 0: " << End0Pos_um.x << ',' << End0Pos_um.y << ',' << End0Pos_um.z << '\n';
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
