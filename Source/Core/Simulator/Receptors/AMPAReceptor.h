//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the AMPAReceptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#pragma once

#include <cstddef>

#include <Simulator/Receptors/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

/**
 * @brief This struct defines the AMPA receptor, a basic model of synaptic
 * transmission. Compare with: Rothman, J.S. and Silver, R.A. (2016),
 * "Data-Driven Modeling of Synaptic Transmission and Integration",
 * Prog.Mol.Biol.Transl.Sci.
 * https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4748401/
 */

struct AMPAReceptor : Receptor {
    float GSyn_pS = 0;   //! AMPAR conductance, picoSiemens
    float GPeak_pS = 39; //! Receptor peak conductance, picoSiemens.
    float ESyn_mV = 0;   //! AMPAR reverse potential, milliVolts
    float Vm_mV = 0;     //! Membrane potential, milliVolts
    float ISyn_pA = 0;   //! Synthetic membrane current, picoAmperes.
    float TauD_ms = 2.0; //! Receptor conductance decay time constant,
                         //! milliseconds (typical: 0.3-2.0 ms)
    float TauR_ms = 0.2; //! Receptor conductance rise time constant,
                         //! milliseconds (typical: 0.2 ms)

    float TauD2_ms = 3.0;
    float TauD3_ms = 4.0;
    float D1 = 0.5;
    float D2 = 0.3;
    float D3 = 0.2;
    float X = 1.5;

    //! Constructors
    AMPAReceptor();
    AMPAReceptor(size_t _ID, float _GSyn_pS, float _GPeak_pS, float _ESyn_mV,
                 float _Vm_mV, float _ISyn_pA, float _TauD_ms, float _TauR_ms,
                 float _TauD2_ms, float _TauD3_ms, float _D1, float _D2,
                 float _D3, float _X, float _ANorm);
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
