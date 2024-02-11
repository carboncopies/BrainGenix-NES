#include <sstream>

#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Vec3D::Vec3D(){};
Vec3D::Vec3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z){};
Vec3D::Vec3D(const std::vector<float> & vec) : x(vec.at(0)), y(vec.at(1)), z(vec.at(2)) {};

//! Operators
//! Vector + Vector
Vec3D Vec3D::operator+(const Vec3D &other) const {
    return Vec3D(this->x + other.x, this->y + other.y, this->z + other.z);
};

//! Vector - Vector
Vec3D Vec3D::operator-(const Vec3D &other) const {
    return Vec3D(this->x - other.x, this->y - other.y, this->z - other.z);
};

//! Vector * Vector
Vec3D Vec3D::operator*(const Vec3D other) const {
    return Vec3D(this->x * other.x, this->y * other.y, this->z * other.z);
};

//! Vector * scalar
Vec3D Vec3D::operator*(const float other) const {
    return Vec3D(this->x * other, this->y * other, this->z * other);
};

//! Vector / scalar
Vec3D Vec3D::operator/(const float other) const {
    assert(other != 0.0);
    return Vec3D(this->x / other, this->y / other, this->z / other);
};

//! Vector == Vector
bool Vec3D::operator==(const Vec3D &other) const {
    return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);
};

//! Vector != Vector
bool Vec3D::operator!=(const Vec3D &other) const {
    return (this->x != other.x) || (this->y != other.y) || (this->z != other.z);
};


//! Vector <= Vector
bool Vec3D::AllElementsLTE(const Vec3D &other) const {
    return (this->x <= other.x) && (this->y <= other.y) && (this->z <= other.z);
};

//! Vector >= Vector
bool Vec3D::AllElementsGTE(const Vec3D &other) const {
    return (this->x >= other.x) && (this->y >= other.y) && (this->z >= other.z);
};



float Vec3D::Distance(const Vec3D &other) const {
    float distance = 0.0;
    Vec3D diff = *this - other;
    distance += pow(diff.x, 2.0) + pow(diff.y, 2.0) + pow(diff.z, 2.0);
    return sqrt(distance);
}

//! Dot product of two vectors
float Vec3D::Dot(const Vec3D &other) const {
    return this->x * other.x + this->y * other.y + this->z * other.z;
}

//! Cross product of two vectors
Vec3D Vec3D::Cross(const Vec3D &other) const {
    float iComp = this->y * other.z - this->z * other.y;
    float jComp = this->z * other.x - this->x * other.z;
    float kComp = this->x * other.y - this->y * other.x;
    return Vec3D(iComp, jComp, kComp);
}

//! Maximum coordinate value.
float Vec3D::Max() const {
    float max = this->x;
    if (max < this->y)
        max = this->y;
    if (max < this->z)
        max = this->z;

    return max;
}

//! Minimum coordinate value.
float Vec3D::Min() const {
    float min = this->x;
    if (min > this->y)
        min = this->y;
    if (min > this->z)
        min = this->z;

    return min;
}

//! Return string representation of vector.
//! TODO: Add in the precision code.
std::string Vec3D::str(int precision) const {
    std::stringstream s;
    s << this->x << ',' << this->y << ',' << this->z;
    return s.str();
}

//! Convert cartesian 3D vector to spherical coordinates.
//! Returns (r, theta, phi) where theta is the angle between z and r (rotation around y-axis)
//! and phi is rotation around z-axis in the x-y plane.
Vec3D Vec3D::cartesianToSpherical() const {
    float r = sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    if (r==0.0) return Vec3D(0.0, 0.0, 0.0);
    float theta = acos(this->z / r); // Rotation around x-axis (z toward y).
    float phi = atan2(this->y, this->x); // Rotation around z-axis (x toward y).

    return Vec3D(r, theta, phi);
}

//! Convert spherical 3D vector to cartesian coordinates.
//! Returns (x, y, z) from (r, theta, phi).
Vec3D Vec3D::sphericalToCartesian() const {
    float r = this->x;
    if (r==0.0) return Vec3D(0.0, 0.0, 0.0);
    float theta = this->y;
    float phi = this->z;

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);

    return Vec3D(x, y, z);
}

float Vec3D::r() const {
    return x;
}

float Vec3D::theta() const {
    return y;
}

float Vec3D::phi() const {
    return z;
}

Vec3D Vec3D::rotate_around_x(float _xangle) const {
    float y_rotx = this->y*std::cos(_xangle) - this->z*std::sin(_xangle);
    float z_rotx = this->y*std::sin(_xangle) + this->z*std::cos(_xangle);
    return Vec3D(this->x, y_rotx, z_rotx);
}

Vec3D Vec3D::rotate_around_y(float _yangle) const {
    float x_roty = this->x*std::cos(_yangle) + this->z*std::sin(_yangle);
    float z_roty = -this->x*std::sin(_yangle) + this->z*std::cos(_yangle);
    return Vec3D(x_roty, this->y, z_roty);
}

Vec3D Vec3D::rotate_around_z(float _zangle) const {
    float x_rotz = this->x*std::cos(_zangle) - this->y*std::sin(_zangle);
    float y_rotz = this->x*std::sin(_zangle) + this->y*std::cos(_zangle);
    return Vec3D(x_rotz, y_rotz, this->z);
}

Vec3D Vec3D::rotate_around_xyz(float _xangle, float _yangle, float _zangle) const {
    float y_rotx = this->y*std::cos(_xangle) - this->z*std::sin(_xangle);
    float z_rotx = this->y*std::sin(_xangle) + this->z*std::cos(_xangle);

    float x_rotx_roty = this->x*std::cos(_yangle) + z_rotx*std::sin(_yangle);
    float z_rotx_roty = -this->x*std::sin(_yangle) + z_rotx*std::cos(_yangle);

    float x_rotx_roty_rotz = x_rotx_roty*std::cos(_zangle) - y_rotx*std::sin(_zangle);
    float y_rotx_roty_rotz = x_rotx_roty*std::sin(_zangle) + y_rotx*std::cos(_zangle);
    return Vec3D(x_rotx_roty_rotz, y_rotx_roty_rotz, z_rotx_roty);
}

//! Uses three concatenated rotation matrices to rotate a 3D point around the
//! x-axiz, y_axis and z-axis in cartesian coordinates.
Vec3D Vec3D::rotate_around_zyx(float _xangle, float _yangle, float _zangle) const {
    float x_rotz = this->x*std::cos(_zangle) - this->y*std::sin(_zangle);
    float y_rotz = this->x*std::sin(_zangle) + this->y*std::cos(_zangle); 

    float x_rotz_roty = x_rotz*std::cos(_yangle) + this->z*std::sin(_yangle);
    float z_roty = -x_rotz*std::sin(_yangle) + this->z*std::cos(_yangle);

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
