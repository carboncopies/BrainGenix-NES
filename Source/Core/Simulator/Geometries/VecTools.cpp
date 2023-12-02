#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

//! Constructors
Vec3D::Vec3D(){};
Vec3D::Vec3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z){};

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

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
