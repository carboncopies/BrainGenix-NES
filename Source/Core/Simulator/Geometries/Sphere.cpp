
#include <Simulator/Geometries/Sphere.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Sphere::Sphere(){
    GeometryShape = GeometrySphere;
};

Sphere::Sphere(float _Radius_um) : Radius_um(_Radius_um){
    GeometryShape = GeometrySphere;
};

Sphere::Sphere(const Vec3D & _Center_um, float _Radius_um) {
    GeometryShape = GeometrySphere;
    this->Center_um = _Center_um;
    this->Radius_um = _Radius_um;
};

//! Renders the sphere in 3D.
void Sphere::Show() { return; };

//! Returns the volume of the sphere in micrometer^3.
float Sphere::Volume_um3() {
    return 4.0 / 3.0 * M_PI * pow(this->Radius_um, 3.0);
};

BoundingBox Sphere::GetBoundingBox(VSDA::WorldInfo& _WorldInfo) {
    BoundingBox bb;

    Geometries::Vec3D RotatedCenter = Center_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

    bb.bb_point1[0] = RotatedCenter.x - Radius_um;
    bb.bb_point1[1] = RotatedCenter.y - Radius_um;
    bb.bb_point1[2] = RotatedCenter.z - Radius_um;

	bb.bb_point2[0] = RotatedCenter.x + Radius_um;
	bb.bb_point2[1] = RotatedCenter.y + Radius_um;
	bb.bb_point2[2] = RotatedCenter.z + Radius_um;


    return bb;
}


bool Sphere::IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo) {

    Geometries::Vec3D RotatedCenter = Center_um.rotate_around_xyz(_WorldInfo.WorldRotationOffsetX_rad, _WorldInfo.WorldRotationOffsetY_rad, _WorldInfo.WorldRotationOffsetZ_rad);

	float Distance = RotatedCenter.Distance(_Position_um);
	return Distance <= Radius_um;
}


bool Sphere::IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo) {
    return GetBoundingBox(_WorldInfo).IsIntersecting(_Region);
}



}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
