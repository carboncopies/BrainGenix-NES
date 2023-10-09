#include <vector>
#include <cassert>

#include <Simulator/Geometries/Box.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors

Box::Box() { };

Box::Box(float Center_um[3], float Dims_um[3]){

    std::copy(Center_um, Center_um+3, this->Center_um);
    std::copy(Dims_um, Dims_um+3, this->Dims_um);
};

Box::Box(float Center_um[3], float Dims_um[3], float Rotations_rad[3]) {

    std::copy(Center_um, Center_um+3, this->Center_um);
    std::copy(Dims_um, Dims_um+3, this->Dims_um);
    std::copy(Rotations_rad, Rotations_rad+3, this->Rotations_rad);
};

//! Renders the box in 3D.
void Box::Show() {
    return;
}; 

//! Returns the volume of the box in micrometer^3.
float Box::Volume_um3() {
    return this->Dims_um[0] * this->Dims_um[1] * this->Dims_um[2];
};


//! Returns 1 of n equally sliced subpartitions of the Box
//! shape, when lined up from left to right along the width.
std::vector<std::vector<float>> Box::EqualSliceBounds(int nSlices, int slice) {
    std::vector<float> topLeft = {0.0, 0.0, 0.0};
    std::vector<float> bottomRight = {0.0, 0.0, 0.0};
    float pWidth = 0.0, y0 = 0.0;

    assert(slice <= nSlices);

    pWidth = this->Dims_um[1] / nSlices;
    y0 = this->Center_um[1] - this->Dims_um[1] / 2.0;

    topLeft[0] = this->Center_um[0] - this->Dims_um[0] / 2.0; 
    topLeft[1] = y0 + slice * pWidth; 
    topLeft[2] = this->Center_um[2] - this->Dims_um[2] / 2.0;

    bottomRight[0] = this->Center_um[0] + this->Dims_um[0] / 2.0; 
    bottomRight[1] = y0 + (slice + 1.0) * pWidth; 
    bottomRight[2] = this->Center_um[2] + this->Dims_um[2] / 2.0;

    std::vector<std::vector<float>> Bounds = {topLeft, bottomRight};
    return Bounds;
}


}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG

