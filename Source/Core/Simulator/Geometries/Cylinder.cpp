
#include <cmath>

#include <Simulator/Geometries/Cylinder.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors
Cylinder::Cylinder() {};

Cylinder::Cylinder(float End0Radius_um, float End0Pos_um[3], float End1Radius_um, float End1Pos_um[3]) {
    this->End0Radius_um = End0Radius_um;
    this->End1Radius_um = End1Radius_um;
    std::copy(End0Pos_um, End0Pos_um+3, this->End0Pos_um);
    std::copy(End1Pos_um, End1Pos_um+3, this->End1Pos_um);
}


//! Renders the cylinder in 3D.
void Cylinder::Show() {
    return;
}; 

//! Returns the volume of the cylinder in micrometer^3.
float Cylinder::Volume_um3() {
    float height_um = 0.0;
    float volume_um3 = 0.0;
    
    // Height is the Euclidian distance between the two end points.
    for (size_t i=0; i<3; ++i)
        height_um += pow(this->End0Pos_um[i] - this->End1Pos_um[i], 2.0);
    height_um = sqrt(height_um);

    // Volume (conical frustum) = pi/3 * (R0^2 + R1^2 + R0*R1)
    // (Ref: https://mathworld.wolfram.com/ConicalFrustum.html)
    
    volume_um3 = M_PI * (pow(this->End0Radius_um, 2.0) +
            pow(this->End1Radius_um, 2.0) +
            this->End0Radius_um * this->End1Radius_um
        ) * height_um / 3.0;

    return volume_um3;
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
