
#include <cmath>
#include <cassert>

#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Vec3D::Vec3D() {};
Vec3D::Vec3D(float _x_um, float _y_um, float _z_um):\
    x_um(_x_um), y_um(_y_um), z_um(_z_um) { };

//! Operators
//! Vector + Vector
Vec3D Vec3D::operator+(const Vec3D& other) const {
    return Vec3D(this->x_um + other.x_um,
                this->y_um + other.y_um,
                this->z_um + other.z_um);
};

//! Vector - Vector
Vec3D Vec3D::operator-(const Vec3D& other) const {
    return Vec3D(this->x_um - other.x_um,
                this->y_um - other.y_um,
                this->z_um - other.z_um);
}; 

//! scalar * Vector
Vec3D Vec3D::operator*(const float other) const {
    return Vec3D(this->x_um * other, 
                this->y_um * other, this->z_um * other);
};

//! 1/scalar * Vector
Vec3D Vec3D::operator/(const float other) const {
    assert(other != 0.0);
    return Vec3D(this->x_um / other, 
                this->y_um / other, this->z_um / other);
};

float Vec3D::Distance(const Vec3D& other) const {
    float distance = 0.0;
    Vec3D diff = *this - other;
    distance += pow(diff.x_um, 2.0) + pow(diff.y_um, 2.0) + pow(diff.z_um, 2.0);
    return sqrt(distance);
}

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
