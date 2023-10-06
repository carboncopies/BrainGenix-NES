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
    return 0.0;
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG

