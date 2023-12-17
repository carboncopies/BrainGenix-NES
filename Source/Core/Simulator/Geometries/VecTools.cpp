#include <sstream>

#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Vec3D::Vec3D(){};
Vec3D::Vec3D(float _x_um, float _y_um, float _z_um)
    : x_um(_x_um), y_um(_y_um), z_um(_z_um){};

//! Operators
//! Vector + Vector
Vec3D Vec3D::operator+(const Vec3D &other) const {
    return Vec3D(this->x_um + other.x_um, this->y_um + other.y_um,
                 this->z_um + other.z_um);
};

//! Vector - Vector
Vec3D Vec3D::operator-(const Vec3D &other) const {
    return Vec3D(this->x_um - other.x_um, this->y_um - other.y_um,
                 this->z_um - other.z_um);
};

//! Vector * Vector
Vec3D Vec3D::operator*(const Vec3D other) const {
    return Vec3D(this->x_um * other.x_um, this->y_um * other.y_um,
                 this->z_um * other.z_um);
};

//! Vector * scalar
Vec3D Vec3D::operator*(const float other) const {
    return Vec3D(this->x_um * other, this->y_um * other, this->z_um * other);
};

//! Vector / scalar
Vec3D Vec3D::operator/(const float other) const {
    assert(other != 0.0);
    return Vec3D(this->x_um / other, this->y_um / other, this->z_um / other);
};

//! Vector == Vector
bool Vec3D::operator==(const Vec3D &other) const {
    return (this->x_um == other.x_um) && (this->y_um == other.y_um) &&
           (this->z_um == other.z_um);
};

//! Vector != Vector
bool Vec3D::operator!=(const Vec3D &other) const {
    return (this->x_um != other.x_um) || (this->y_um != other.y_um) ||
           (this->z_um != other.z_um);
};


//! Vector <= Vector
bool Vec3D::AllElementsLTE(const Vec3D &other) const {
    return (this->x_um <= other.x_um) && (this->y_um <= other.y_um) && (this->z_um <= other.z_um);
};

//! Vector >= Vector
bool Vec3D::AllElementsGTE(const Vec3D &other) const {
    return (this->x_um >= other.x_um) && (this->y_um >= other.y_um) && (this->z_um >= other.z_um);
};



float Vec3D::Distance(const Vec3D &other) const {
    float distance = 0.0;
    Vec3D diff = *this - other;
    distance += pow(diff.x_um, 2.0) + pow(diff.y_um, 2.0) + pow(diff.z_um, 2.0);
    return sqrt(distance);
}

//! Dot product of two vectors
float Vec3D::Dot(const Vec3D &other) const {
    return this->x_um * other.x_um + this->y_um * other.y_um +
           this->z_um * other.z_um;
}

//! Maximum coordinate value.
float Vec3D::Max() const {
    float max = this->x_um;
    if (max < this->y_um)
        max = this->y_um;
    if (max < this->z_um)
        max = this->z_um;

    return max;
}

//! Minimum coordinate value.
float Vec3D::Min() const {
    float min = this->x_um;
    if (min > this->y_um)
        min = this->y_um;
    if (min > this->z_um)
        min = this->z_um;

    return min;
}

//! Return string representation of vector.
//! TODO: Add in the precision code.
std::string Vec3D::str(int precision) const {
    std::stringstream s;
    s << this->x_um << ',' << this->y_um << ',' << this->z_um;
    return s.str();
}

//! Convert cartesian 3D vector to spherical coordinates.
//! Returns (r, theta, phi) where theta is the angle between z and r (rotation around y-axis)
//! and phi is rotation around z-axis in the x-y plane.
Vec3D Vec3D::cartesianToSpherical() const {
    float r = sqrt(pow(this->x_um, 2) + pow(this->y_um, 2) + pow(this->z_um, 2));
    if (r==0.0) return Vec3D(0.0, 0.0, 0.0);
    float theta = acos(this->z_um / r); // Rotation around x-axis (z toward y).
    float phi = atan2(this->y_um, this->x_um); // Rotation around z-axis (x toward y).

    return Vec3D(r, theta, phi);
}

//! Convert spherical 3D vector to cartesian coordinates.
//! Returns (x, y, z) from (r, theta, phi).
Vec3D Vec3D::sphericalToCartesian() const {
    float r = this->x_um;
    if (r==0.0) return Vec3D(0.0, 0.0, 0.0);
    float theta = this->y_um;
    float phi = this->z_um;

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);

    return Vec3D(x, y, z);
}

float Vec3D::r() const {
    return x_um;
}

float Vec3D::theta() const {
    return y_um;
}

float Vec3D::phi() const {
    return z_um;
}

Vec3D Vec3D::rotate_around_x(float _xangle) const {
    float y_rotx = this->y_um*std::cos(_xangle) - this->z_um*std::sin(_xangle);
    float z_rotx = this->y_um*std::sin(_xangle) + this->z_um*std::cos(_xangle);
    return Vec3D(this->x_um, y_rotx, z_rotx);
}

Vec3D Vec3D::rotate_around_y(float _yangle) const {
    float x_roty = this->x_um*std::cos(_yangle) + this->z_um*std::sin(_yangle);
    float z_roty = -this->x_um*std::sin(_yangle) + this->z_um*std::cos(_yangle);
    return Vec3D(x_roty, this->y_um, z_roty);
}

Vec3D Vec3D::rotate_around_z(float _zangle) const {
    float x_rotz = this->x_um*std::cos(_zangle) - this->y_um*std::sin(_zangle);
    float y_rotz = this->x_um*std::sin(_zangle) + this->y_um*std::cos(_zangle);
    return Vec3D(x_rotz, y_rotz, this->z_um);
}

Vec3D Vec3D::rotate_around_xyz(float _xangle, float _yangle, float _zangle) const {
    float y_rotx = this->y_um*std::cos(_xangle) - this->z_um*std::sin(_xangle);
    float z_rotx = this->y_um*std::sin(_xangle) + this->z_um*std::cos(_xangle);

    float x_rotx_roty = this->x_um*std::cos(_yangle) + z_rotx*std::sin(_yangle);
    float z_rotx_roty = -this->x_um*std::sin(_yangle) + z_rotx*std::cos(_yangle);

    float x_rotx_roty_rotz = x_rotx_roty*std::cos(_zangle) - y_rotx*std::sin(_zangle);
    float y_rotx_roty_rotz = x_rotx_roty*std::sin(_zangle) + y_rotx*std::cos(_zangle);
    return Vec3D(x_rotx_roty_rotz, y_rotx_roty_rotz, z_rotx_roty);
}

//! Uses three concatenated rotation matrices to rotate a 3D point around the
//! x-axiz, y_axis and z-axis in cartesian coordinates.
Vec3D Vec3D::rotate_around_zyx(float _xangle, float _yangle, float _zangle) const {
    float x_rotz = this->x_um*std::cos(_zangle) - this->y_um*std::sin(_zangle);
    float y_rotz = this->x_um*std::sin(_zangle) + this->y_um*std::cos(_zangle); 

    float x_rotz_roty = x_rotz*std::cos(_yangle) + this->z_um*std::sin(_yangle);
    float z_roty = -x_rotz*std::sin(_yangle) + this->z_um*std::cos(_yangle);

    // x = x cos(phi) cos(theta) - y sin(phi) cos(theta) + z sin(theta)
    // y = x sin(phi) + y cos(phi)
    // z = x cos(phi) sin(theta) + y sin(phi) sin(theta) + z cos(theta)
    // [0, 0, 5]
    // [5, 0, 0]

    float y_rotz_rotx = y_rotz*std::cos(_xangle) - z_roty*std::sin(_xangle);
    float z_roty_rotx = y_rotz*std::sin(_xangle) + z_roty*std::cos(_xangle);

    return Vec3D(x_rotz_roty, y_rotz_rotx, z_roty_rotx);
}

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
