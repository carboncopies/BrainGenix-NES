#//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Receptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

//! Maximum number of iterations for finding a_norm
#define _FIND_ANORM_MAX_ITERS 100
//! Number of time (t_ms) values to use when finding a_norm
#define _FIND_ANORM_NUM_TIMESTEPS 10000
//! Tolerance for convergence to use when finding a_norm
#define _FIND_ANORM_CONVERGENCE_TOL 0.1

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

/**
 * @brief This struct defines the Receptor struct, the base struct for
 * all receptor classes which are basic models of synaptic transmission.
 * Compare with: Rothman, J.S. and Silver, R.A. (2016), "Data-Driven Modeling
 * of Synaptic Transmission and Integration", Prog.Mol.Biol.Transl.Sci.
 * https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4748401/
 */

struct Receptor {
    //! Type of post-synaptic potential used in the receptor.
    enum PSPType { DOUBLE_EXPONENT = 0, MXH, SIMPLE_EXPONENT, ALPHA_FUNCTION };

    float GSyn_pS;  //! AMPAR conductance, picoSiemens
    float GPeak_pS; //! Receptor peak conductance, picoSiemens.
    float ESyn_mV;  //! AMPAR reverse potential, milliVolts
    float Vm_mV;    //! Membrane potential, milliVolts
    float ISyn_pA;  //! Synthetic membrane current, picoAmperes.
    float TauD_ms;  //! Receptor conductance decay time constant, milliseconds
    float TauR_ms;  //! Receptor conductance rise time constant, milliseconds

    float TauD2_ms;
    float TauD3_ms;
    float D1;
    float D2;
    float D3;
    float X;

    float ANorm =
        1.0; //! Normalizing scale factor so that peak Gsyn_t_pS == G_peak_pS

    size_t ID = 0;
    std::vector<float> ANorms{};
    std::vector<float> GDiffs{};

    float PostSynCurrent_pA();

    float Conductance_pS();

    float GSynT_pS(float t_ms, PSPType pspType);
    std::vector<float> GSynT_pS(std::vector<float> t_ms, PSPType pspType);

    //! Modeled with a simple exponential.
    float GSynT_pS_DecayZeroRiseTime(float t_ms);
    std::vector<float> GSynT_pS_DecayZeroRiseTime(std::vector<float> t_ms);

    //! Modeled with an alpha function.
    float GSynT_pS_RiseDecay(float t_ms);
    std::vector<float> GSynT_pS_RiseDecay(std::vector<float> t_ms);

    //! Modeled with a double exponential.
    float GSynT_pS_DoubleExponential(float t_ms);
    std::vector<float> GSynT_pS_DoubleExponential(std::vector<float> t_ms);

    //! Modeled with a multiexponential function with m^xh formalism to fit
    //! more complex waveforms.
    float GSynT_pS_MXH(float t_ms);
    std::vector<float> GSynT_pS_MXH(std::vector<float> t_ms);

    //! For analytical solution see Roth & van Rossum (2009).
    float FindANorm(bool verbose = false);
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
