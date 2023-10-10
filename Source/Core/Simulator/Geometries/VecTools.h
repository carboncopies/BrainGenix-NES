//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides utilities for creating and interacting with 3D vectors.
    Additional Notes: None
    Date Created: 2023-10-09
*/

#pragma once

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


/**
 * @brief This struct defines a 3D object that will be used to represent Cartesian coordinates.
 * 
 */
struct Vec3D {
    
    float x_um{}; //! X-coordinate in micrometers.
    float y_um{}; //! Y-coordinate in micrometers.
    float z_um{}; //! Z-coordinate in micrometers.

    //! Constructors
    Vec3D();
    Vec3D(float _x_um, float _y_um, float _z_um);

    //! Operators
    //! Vector + Vector
    Vec3D operator+(const Vec3D& other) const;
    //! Vector - Vector
    Vec3D operator-(const Vec3D& other) const;
    //! scalar * Vector
    Vec3D operator*(const float other) const;
    //! 1/scalar * Vector
    Vec3D operator/(const float other) const;
    
    //! Distance between two vectors
    float Distance(const Vec3D& other) const; 
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
