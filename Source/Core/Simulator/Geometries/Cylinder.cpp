
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
    alpha_rad =
        acos(axisVec.Dot(refAxisVec) /
             (axisVec.Distance(originVec) * refAxisVec.Distance(originVec)));

    // 2. The axis is found from the cross product of the two vectors.
    crossProdVec = refAxisVec.Cross(axisVec);
    crossProdVec = crossProdVec / crossProdVec.Distance(originVec); // Normalize

    // 3. Fill normalMat from calculated cross product vector
    normalMat[0][1] = -crossProdVec.z_um;
    normalMat[0][2] = crossProdVec.y_um;
    normalMat[1][0] = crossProdVec.z_um;
    normalMat[1][2] = -crossProdVec.x_um;
    normalMat[2][0] = -crossProdVec.y_um;
    normalMat[2][1] = crossProdVec.x_um;

    // 4. Calculate rotation matrix (using Rodrigues' rotation formula.)
    // Ref.:
    // https://math.stackexchange.com/questions/296267/how-to-find-eulers-angles
    rotationMat =
        glm::mat3(1.0f) + normalMat * static_cast<float>(sin(alpha_rad)) +
        normalMat * normalMat * static_cast<float>(1.0 - cos(alpha_rad));

    // 5. Calculate Euler angles from rotation matrix. Ref:.
    // https://stackoverflow.com/questions/15022630/how-to-calculate-the-angle-from-rotation-matrix
    thetaX_rad = atan2(rotationMat[2][1], rotationMat[2][2]);
    thetaY_rad =
        atan2(-rotationMat[2][0], sqrt(rotationMat[2][2] * rotationMat[2][2] +
                                       rotationMat[2][1] * rotationMat[2][1]));
    thetaZ_rad = atan2(rotationMat[1][0], rotationMat[0][0]);

    return std::make_tuple(thetaX_rad, thetaY_rad, thetaZ_rad);
};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
