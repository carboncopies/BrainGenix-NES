#include <cmath>

#include <Simulator/Geometries/Sphere.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Sphere::Sphere() { };

Sphere::Sphere(Vec3D _Center_um, float _Radius_um) { 
    this->Center_um = _Center_um;
    this->Radius_um = _Radius_um;
};

//! Renders the sphere in 3D.
void Sphere::Show(){
    return;
}; 

//! Returns the volume of the sphere in micrometer^3.
float Sphere::Volume_um3() {
    return 4.0 / 3.0 * M_PI * pow(this->Radius_um, 3.0);
;
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
