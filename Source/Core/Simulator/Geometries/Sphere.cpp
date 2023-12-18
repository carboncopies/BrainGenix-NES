
#include <Simulator/Geometries/Sphere.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Sphere::Sphere(){};

Sphere::Sphere(float _Radius_um) : Radius_um(_Radius_um){};

Sphere::Sphere(Vec3D _Center_um, float _Radius_um) {
    this->Center_um = _Center_um;
    this->Radius_um = _Radius_um;
};

//! Renders the sphere in 3D.
void Sphere::Show() { return; };

//! Returns the volume of the sphere in micrometer^3.
float Sphere::Volume_um3() {
    return 4.0 / 3.0 * M_PI * pow(this->Radius_um, 3.0);
    ;
};

BoundingBox Sphere::GetBoundingBox() {
    BoundingBox bb;

    bb.bb_point1[0] = Center_um.x - Radius_um;
    bb.bb_point1[1] = Center_um.y - Radius_um;
    bb.bb_point1[2] = Center_um.z - Radius_um;

	bb.bb_point2[0] = Center_um.x + Radius_um;
	bb.bb_point2[1] = Center_um.y + Radius_um;
	bb.bb_point2[2] = Center_um.z + Radius_um;


    return bb;
}


bool Sphere::IsPointInShape(Vec3D _Position_um) {
	float Distance = Center_um.Distance(_Position_um);
	return Distance <= Radius_um;
}


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
