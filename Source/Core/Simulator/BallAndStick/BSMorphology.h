//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
Description: This file provides functions that use geometric structs to
             create morphologies of neurons in the ball and stick neural
             circuit.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#pragma once

#include <memory>
#include <vector>

#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Different ways the geometries can be aligned to create the morphology.
enum Align { LEFT = 0, RIGHT, CENTER };

//! Create the soma of a neuron in the shape of a sphere.
std::unique_ptr<Geometries::Sphere>
CreateBSSoma(std::vector<std::vector<float>> domainBounds, Align align,
             float radius_um = 0.5);

//! Create the axon of a neuron in the shape of a cylinder.
std::unique_ptr<Geometries::Cylinder>
CreateBSAxon(std::vector<std::vector<float>> domainBounds, Align align,
             float somaRadius_um = 0.5, float end0Radius_um = 0.1,
             float end1Radius_um = 0.1);

//! Create a receptor in the form of a box.
std::unique_ptr<Geometries::Box>
CreateBSReceptor(Geometries::Vec3D receptorLocation_um);

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
