
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

// I have no clude what I'm doing
BoundingBox Cylinder::GetBoundingBox() {
	BoundingBox bb;


	return bb;
}

//1. Create a for-loop that walks along the axis of a cylinder (as if it is not rotatedd)
//2. At Every step, run through a loop that makes steps in a disk (or through a disk to fill the whole thing in) at the disk radius at that location (see start radius, end radius)
//3. These become a collection of 3D points. Rotate these according to cylinder rotation.
//4. Obtain Voxel indices (X,Y,Z) for each point.
//5. Eliminate duplicate Voxels in the set.
//6. Paint the voxels in the remaining set.
bool Cylinder::IsPointInShape(Vec3D _Position_um) {
    return false;
}


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
