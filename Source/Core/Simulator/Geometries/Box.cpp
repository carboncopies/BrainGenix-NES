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
    return this->Dims_um.x_um * this->Dims_um.y_um * this->Dims_um.z_um;
};

//! Returns 1 of n equally sliced subpartitions of the Box
//! shape, when lined up from left to right along the width.
std::vector<std::vector<float>> Box::EqualSliceBounds(int nSlices, int slice) {
    std::vector<float> topLeft = {0.0, 0.0, 0.0};
    std::vector<float> bottomRight = {0.0, 0.0, 0.0};
    float pWidth = 0.0, y0 = 0.0;

    assert(slice <= nSlices);

    pWidth = this->Dims_um.y_um / nSlices;
    y0 = this->Center_um.y_um - this->Dims_um.y_um / 2.0;

    topLeft[0] = this->Center_um.x_um - this->Dims_um.x_um / 2.0;
    topLeft[1] = y0 + slice * pWidth;
    topLeft[2] = this->Center_um.z_um - this->Dims_um.z_um / 2.0;

    bottomRight[0] = this->Center_um.x_um + this->Dims_um.x_um / 2.0;
    bottomRight[1] = y0 + (slice + 1.0) * pWidth;
    bottomRight[2] = this->Center_um.z_um + this->Dims_um.z_um / 2.0;

    std::vector<std::vector<float>> bounds = {topLeft, bottomRight};
    return bounds;
}

//! Returns a vector of dimensions of the Box.
std::vector<float> Box::Sides() {
    std::vector<float> sides = {this->Dims_um.x_um, this->Dims_um.y_um,
                                this->Dims_um.z_um};
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


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
