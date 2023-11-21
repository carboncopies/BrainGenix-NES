//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides utilities for creating and interacting with
   3D vectors. Additional Notes: None Date Created: 2023-10-09
*/

#pragma once

#include <cassert>
#include <cmath>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

/**
 * @brief This struct defines a 3D object that will be used to represent
 * Cartesian coordinates.
 *
 */
struct Vec3D {

    float x{}; //! X-coordinate.
    float y{}; //! Y-coordinate.
    float z{}; //! Z-coordinate.

    //! Constructors
    Vec3D();
    Vec3D(float _x, float _y, float _z);

    //! Operators
    //! Vector + Vector
    Vec3D operator+(const Vec3D &other) const;
    //! Vector - Vector
    Vec3D operator-(const Vec3D &other) const;
    //! Vector * Vector (Elementwise)
    Vec3D operator*(const Vec3D other) const;
    //! scalar * Vector
    Vec3D operator*(const float other) const;
    //! Vector / scalar
    Vec3D operator/(const float other) const;
    //! Vector == Vector
    bool operator==(const Vec3D &other) const;
    //! Vector != Vector
    bool operator!=(const Vec3D &other) const;

    //! Distance between two vectors
    float Distance(const Vec3D &other) const;

    //! Dot product of two vectors
    float Dot(const Vec3D &other) const;

    //! Cross product of two vectors
    Vec3D Cross(const Vec3D &other) const;

    //! Maximum coordinate value
    float Max() const;

    //! Minimum coordinate value
    float Min() const;
};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
