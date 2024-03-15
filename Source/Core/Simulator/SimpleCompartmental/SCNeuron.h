//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the SimpleCompartmental struct,
                 representing a basic building block for a 
                 SimpleCompartmental neural circuit.
    Additional Notes: None
    Date Created: 2024-03-11
*/

#pragma once

#include <Simulator/BallAndStick/BSNeuron.h>

namespace BG {
namespace NES {
namespace Simulator {

// Forward declaration.
// This is used to give SCNeuron knowledge of the Simulation it is in, so
// That it can use compartment IDs and more to find its compartments and
// other related objects without needing cached pointers to them and without
// having to supply a whole list of references, since those references are
// all already available in the handy Simulation object.
struct Simulation;

/**
 * @brief Provides object data and member functions of the Simple Compartmental
 *        neuron class.
 */
class SCNeuron: public BallAndStick::BSNeuron {
protected:
    Simulation & Sim;
    Geometries::Vec3D GeoCenter_um{}; // This cache is updated whenever GetCellCenter() is called.

public:
    CoreStructs::SCNeuronStruct build_data; // Copy of the struct that was used to build this neuron.

public:
    //SCNeuron(int ID, std::unordered_map<std::string, Geometries::Geometry*> & morphology);
    SCNeuron(const CoreStructs::SCNeuronStruct & scneuronstruct, Simulation & _Sim);

    //! Returns the geometric center of the neuron.
    virtual Geometries::Vec3D& GetCellCenter();
};

} // Simulator
} // NES
} // BG
