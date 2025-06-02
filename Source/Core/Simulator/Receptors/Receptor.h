#//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/**
 * @file Receptor.h
 * @brief Provides the definition of the Receptor struct, which models synaptic transmission.
 * 
 * This file contains the Receptor struct, which serves as the base for all receptor classes
 * in the BrainGenix-NES Neuron Emulation System. It includes methods for calculating
 * synaptic conductance and currents based on various models.
 * 
 * @date Created: 2023-10-16
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
 * @brief Models synaptic transmission using various receptor types.
 * 
 * The Receptor struct provides methods to calculate synaptic conductance and currents
 * based on different mathematical models. It is inspired by the work of Rothman and Silver (2016).
 */
struct Receptor {
    /**
     * @brief Types of post-synaptic potentials (PSP) modeled by the receptor.
     */
    enum PSPType {
        DOUBLE_EXPONENT = 0, //!< Double exponential model.
        MXH,                //!< Multi-exponential with m^xh formalism.
        SIMPLE_EXPONENT,    //!< Simple exponential model.
        ALPHA_FUNCTION      //!< Alpha function model.
    };

    float GSyn_pS;  //!< AMPAR conductance in picoSiemens.
    float GPeak_pS; //!< Receptor peak conductance in picoSiemens.
    float ESyn_mV;  //!< AMPAR reverse potential in milliVolts.
    float Vm_mV;    //!< Membrane potential in milliVolts.
    float ISyn_pA;  //!< Synthetic membrane current in picoAmperes.
    float TauD_ms;  //!< Receptor conductance decay time constant in milliseconds.
    float TauR_ms;  //!< Receptor conductance rise time constant in milliseconds.

    float TauD2_ms; //!< Secondary decay time constant in milliseconds.
    float TauD3_ms; //!< Tertiary decay time constant in milliseconds.
    float D1;       //!< Weighting factor for the first decay component.
    float D2;       //!< Weighting factor for the second decay component.
    float D3;       //!< Weighting factor for the third decay component.
    float X;        //!< Scaling factor for the receptor model.

    float ANorm = 1.0; //!< Normalizing scale factor so that peak GSyn_t_pS equals GPeak_pS.

    size_t ID = 0; //!< Unique identifier for the receptor instance.
    std::vector<float> ANorms; //!< Vector of normalization factors for different time steps.
    std::vector<float> GDiffs; //!< Vector of conductance differences for debugging or analysis.

    /**
     * @brief Calculates the synthetic membrane current in picoAmperes.
     * @return The synthetic membrane current.
     */
    float PostSynCurrent_pA();

    /**
     * @brief Calculates the receptor's conductance in picoSiemens.
     * @return The receptor conductance.
     */
    float Conductance_pS();

    /**
     * @brief Calculates the synaptic conductance at a given time using a specified PSP model.
     * @param t_ms Time in milliseconds.
     * @param pspType The type of PSP model to use.
     * @return The synaptic conductance in picoSiemens.
     */
    float GSynT_pS(float t_ms, PSPType pspType);

    /**
     * @brief Calculates the synaptic conductance for a vector of time points using a specified PSP model.
     * @param t_ms Vector of time points in milliseconds.
     * @param pspType The type of PSP model to use.
     * @return A vector of synaptic conductance values in picoSiemens.
     */
    std::vector<float> GSynT_pS(std::vector<float> t_ms, PSPType pspType);

    /**
     * @brief Calculates synaptic conductance using a simple exponential model with zero rise time.
     * @param t_ms Time in milliseconds.
     * @return The synaptic conductance in picoSiemens.
     */
    float GSynT_pS_DecayZeroRiseTime(float t_ms);

    /**
     * @brief Calculates synaptic conductance for a vector of time points using a simple exponential model with zero rise time.
     * @param t_ms Vector of time points in milliseconds.
     * @return A vector of synaptic conductance values in picoSiemens.
     */
    std::vector<float> GSynT_pS_DecayZeroRiseTime(std::vector<float> t_ms);

    /**
     * @brief Calculates synaptic conductance using an alpha function model.
     * @param t_ms Time in milliseconds.
     * @return The synaptic conductance in picoSiemens.
     */
    float GSynT_pS_RiseDecay(float t_ms);

    /**
     * @brief Calculates synaptic conductance for a vector of time points using an alpha function model.
     * @param t_ms Vector of time points in milliseconds.
     * @return A vector of synaptic conductance values in picoSiemens.
     */
    std::vector<float> GSynT_pS_RiseDecay(std::vector<float> t_ms);

    /**
     * @brief Calculates synaptic conductance using a double exponential model.
     * @param t_ms Time in milliseconds.
     * @return The synaptic conductance in picoSiemens.
     */
    float GSynT_pS_DoubleExponential(float t_ms);

    /**
     * @brief Calculates synaptic conductance for a vector of time points using a double exponential model.
     * @param t_ms Vector of time points in milliseconds.
     * @return A vector of synaptic conductance values in picoSiemens.
     */
    std::vector<float> GSynT_pS_DoubleExponential(std::vector<float> t_ms);

    /**
     * @brief Calculates synaptic conductance using a multi-exponential model with m^xh formalism.
     * @param t_ms Time in milliseconds.
     * @return The synaptic conductance in picoSiemens.
     */
    float GSynT_pS_MXH(float t_ms);

    /**
     * @brief Calculates synaptic conductance for a vector of time points using a multi-exponential model with m^xh formalism.
     * @param t_ms Vector of time points in milliseconds.
     * @return A vector of synaptic conductance values in picoSiemens.
     */
    std::vector<float> GSynT_pS_MXH(std::vector<float> t_ms);

    /**
     * @brief Finds the normalization factor (ANorm) for the receptor.
     * 
     * This method iteratively adjusts ANorm to ensure that the peak synaptic conductance
     * matches the specified GPeak_pS value.
     * 
     * @param verbose If true, prints detailed information about the convergence process.
     * @return The calculated normalization factor.
     */
    float FindANorm(bool verbose = false);
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
