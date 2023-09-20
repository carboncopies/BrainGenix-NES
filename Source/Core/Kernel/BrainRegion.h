//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the main entity that emulates a neural circuit.
    Additional Notes: None
    Date Created: 2023-09-19
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <vector>
#include <cmath>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace API {


/**
 * @brief This class defines the main entity that emulates a neural circuit.
 * 
 */
class BrainRegion {
    /**
     * @brief 
     * 
     * @param  
     */

// private:
//     int dimension{};
//     BSAlignedNeuralCircuit neural_circuit;
// 
// public:
//     /**
//      * @brief 
//      * 
//      * @param  
//      */
// 
//     BrainRegion();
//     ~BrainRegion();
};

/**
 * @brief This class defines the soma morphology of a ball-and-stick neural circuit.
 * 
 */
class BSSoma {
    /**
     * @brief 
     * 
     * @param  
     */

// public:
//     float dimension{};
//     std::vector<float> location;
// 
//     BSSoma();
//     ~BSSoma();
}

/**
 * @brief This class defines the axon morphology of a ball-and-stick neural circuit.
 * 
 */
class BSAxon {
    /**
     * @brief 
     * 
     * @param  
     */

// public:
//     float dimension{};
//     std::vector<float> location;
// 
//     BSAxon();
//     ~BSAxon();
}

/**
 * @brief This class defines a ball-and-stick neural circuit.
 * 
 */
class BSAlignedNeuralCircuit {
    /**
     * @brief 
     * 
     * @param  
     */

// public:
//     float w_syn = 1.0;
//     BSNeuron pre_synaptic, post_synaptic;
// 
//     BSAlignedNeuralCircuit();
//     ~BSAlignedNeuralCircuit();
// 
//     void UpdateNeurons() {
//         // 1. Update the momentary membrane potential, V_m
// 
//         // 2. Detect threshold crossing and possibly generate a spike.
//         // This step is ignored if the neuron is still within the 
//         // absolute refractory period, tau_abs , of its most
//         // recent action potential.
//     }
}


/**
 * @brief This class defines a single ball-and-stick neuron.
 * 
 */
class BSNeuron {
    /**
     * @brief 
     * 
     * @param  
     */

// private:
//     const float v_m_init = -60;   // mV
//     const float v_rest = -60; // mV
//     const float v_spike_init = 60; // mV
//     const float v_act = -50;  // mV
//     const float v_ahp_init = -20;  // mV
//     const float tau_abs = 1;  // ms
//     const float tau_ahp = 30; // ms
// 
// public:
//     float v_m{};  // mV
//     float v_spike{};  // mV
//     float v_ahp{};  // mV
//                    
//     BSReceptor receptor;
// 
//     BSNeuron();
//     ~BSNeuron();
// 
//     void UpdateMomentaryMembranePotential() {
//         this.v_m = this.v_rest + this.v_spike + this.v_ahp;
//         for (float val: v_psp)
//             this.v_m += val;
//     }
// 
//     void UpdateSpikePotential(float delta_t_act) {
//         if (delta_t_act <= this.tau_abs) 
//             this.v_spike = this.v_spike_init;
//         else
//             this.v_spike = 0;
//     }
// 
//     void UpdateAHPPotential(float delta_t_act) {
//         if (delta_t_act > this.tau_abs)
//             this.v_ahp = this.v_ahp_init * exp(-delta_t_act / this.tau_ahp);
//         else
//             this.v_ahp = 0;
//     }
};

/**
 * @brief This class defines a single ball-and-stick neural receptor.
 * 
 */
class BSReceptor {
    /**
     * @brief 
     * 
     * @param  
     **/

// private:
//     const float tau_psp_r = 5;    // ms
//     const float tau_psp_d = 25;   // ms
//     const float v_psp_init = 20;   // mV
// 
// public:
//     std::vector<float> v_psp_vec;
//     std::vector<float> delta_t_act_vec;
// 
//     BSReceptor();
//     ~BSReceptor();
// 
//     void UpdatePostSynapticPotentials(float w_syn, bool spiked) {
//         for (size_t i=0; i<this.v_psp_vec.size(); ++i) {
//             if (spiked)
//                 this.v_psp_vec[i] = w_syn * this.v_psp_init * exp(tau_psp_r / tau_psp_d);
//             else
//                 this.v_psp_vec[i] = 0;
//         }
//     }

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG
