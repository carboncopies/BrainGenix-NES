//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2023-10-09
*/

#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

/**
 * @brief This struct provides the base struct for all neurons.
 *
 */
struct Neuron {

    std::string Name; /**Name of the Neuron*/
    int ID; /**ID of the Neuron */

    Geometries::Vec3D cell_center; // *** FIX THIS!

    //! Returns the time since the action potential threshold was
    //! crossed last.
    virtual float DtAct_ms(float t_ms) { return 0.0; }; // *** FIX THIS!

    //! Tells if the action potential threshold has been crossed.
    virtual bool HasSpiked() { return false; } // *** FIX THIS!

    //! Returns the geometric center of the neuron.
    virtual Geometries::Vec3D &GetCellCenter() { return cell_center; } // *** FIX THIS!
};

//! ReceptorData is a tuple containing a pointer to the source (pre-synaptic)
//! neuron of a connection and its weight.
typedef std::tuple<std::shared_ptr<CoreStructs::Neuron>, float> ReceptorData;

//! NeuronRecording is an unordered map containing data from simulation in a
//! neuron.
typedef std::unordered_map<std::string, std::vector<float>> NeuronRecording;

struct BSNeuron: Neuron {

    int SomaShapeID;
    int AxonShapeID;

    float MembranePotential_mV;
    float RestingPotential_mV;
    float SpikeThreshold_mV;
    float DecayTime_ms;
    float AfterHyperpolarizationAmplitude_mV;
    float PostsynapticPotentialRiseTime_ms;
    float PostsynapticPotentialDecayTime_ms;
    float PostsynapticPotentialAmplitude_mV;

    virtual float DtAct_ms(float t_ms) { return 0.0; }; // *** FIX THIS!

    virtual bool HasSpiked() { return false; } // *** FIX THIS!

    virtual Geometries::Vec3D &GetCellCenter() { return cell_center; } // *** FIX THIS!

};

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
