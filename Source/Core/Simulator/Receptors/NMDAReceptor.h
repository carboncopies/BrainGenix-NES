//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the NMDAReceptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#pragma once

#include <vector>

#include <Simulator/Receptors/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

/**
 * @brief This struct defines the NMDA receptor, a basic model of synaptic
 * transmission. Compare with: Rothman, J.S. and Silver, R.A. (2016),
 * "Data-Driven Modeling of Synaptic Transmission and Integration",
 * Prog.Mol.Biol.Transl.Sci.
 * https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4748401/
 */

struct NMDAReceptor : Receptor {
    //! Type of Phi potential used in the receptor.
    enum PhiVType { BOLZMANN = 0, WOODHULL_1, WOODHULL_2 };

    float GSyn_pS = 0;    //! AMPAR conductance, picoSiemens
    float GPeak_pS = 39;  //! Receptor peak conductance, picoSiemens.
    float ESyn_mV = 0;    //! AMPAR reverse potential, milliVolts
    float Vm_mV = 0;      //! Membrane potential, milliVolts
    float ISyn_pA = 0;    //! Synthetic membrane current, picoAmperes.
    float TauD_ms = 50.0; //! Receptor conductance decay time constant,
                          //! milliseconds (Typically 30-70 ms, sometimes
                          //! as long as 500 ms.)
    float TauR_ms = 10.0; //! Receptor conductance rise time constant,
                          //! milliseconds (Typically 10 ms.)

    float TauD2_ms = 70.0;
    float TauD3_ms = 300.0;
    float D1 = 0.5;
    float D2 = 0.3;
    float D3 = 0.2;
    float X = 1.5;

    float ANorm =
        1.0; //! Normalizing scale factor so that peak Gsyn_t_pS == g_peak_pS
    float VHalfBlocked_mV =
        20.0;      //! Voltage at which half the NMDAR channels are blocked.
    float K = 2.0; //! Slope factor that determines steepness of voltage
                   //! dependence around V_halfblocked.
    float Phi_V{};

    size_t ID = 0;

    //! Constructors
    NMDAReceptor();
    NMDAReceptor(size_t _ID, float _Gsyn_pS, float _Gpeak_pS, float _Esyn_mV,
                 float _Vm_mV, float _Isyn_pA, float _TauD_ms, float _TauR_ms,
                 float _TauD2_ms, float _TauD3_ms, float _D1, float _D2,
                 float _D3, float _X, float _ANorm, float _VHalfBlocked_mV,
                 float _K);

    void SetPhiVType(PhiVType phiVType);

    //! Modeled with a Bolzmann function. Easy to use, but not directly
    //! related to physical aspects of Mg2+ blocking mechanism.
    float PhiVBolzmann();

    //! temperature_K - absolute temperatue
    //! z - valence of blocking ion (e.g. +2 for Mg2+)
    float Phi(float temperature_K, int z = 2);

    //! Mg2plus_0 - Mg2+ concentration outside membrane.
    //! KBinding - Binding constant.
    //! delta - Fraction of membrane voltage that Mg2+ experiences at the
    //! blocking site. temperature_K - absolute temperature. voltage_V -
    //! voltage.
    float KBindingRate(float Mg2plus_0, float KBinding, float delta, float V,
                       float temperature_K);

    //! KUnbinding - Unbinding constant.
    //! delta - Fraction of membrane voltage that Mg2+
    //!         experiences at the blocking site.
    float KUnbindingRate(float KUnbinding, float delta, float voltage_V,
                         float temperature_K);

    //! Modeled with a two-state Woodhull formalism derived from a kinetic
    //! model of an extracellular Mg2+ block. An ion channel is blocked
    //! when an ion species (here, Mg2+)	is bound to a binding site
    //! inside the channel, open when the ion species is unbound.
    //! Mg2plus_0 - Mg2+ concentration outside membrane.
    //! KBinding - Binding constant.
    //! KUnbinding - Unbinding constant.
    //! delta - Fraction of membrane voltage that Mg2+ experiences at the
    //! blocking site.

    float PhiVWoodhull_1(float Mg2plus_0, float KBinding, float KUnbinding,
                         float delta, float voltage_V, float temperature_K);

    //! Modeled with a two-state Woodhull formalism derived from a kinetic
    //! model of extracellular Mg2+ block. An ion channel is blocked when an
    //! ion species (here, Mg2+)	is bound to a binding site inside the
    //! channel, open when the ion species is unbound.  K_dissociation_0mV
    //! quantifies the strength or affinity of Mg2+ binding and delta quantifies
    //! the location of the Mg2+ binding site within the channel.
    //! Mg2plus_0 - Mg2+ concentration outside membrane.
    //! KDissociation_0mV - Dissociation constant at 0 mV, equal to
    //! K_unbinding/K_binding. delta - Fraction of membrane voltage that Mg2+
    //! experiences at the blocking site.
    float PhiVWoodhull_2(float Mg2plus_0, float K_dissociation_0mV, float delta,
                         float voltage_V, float temperature_K);

    //! Strong voltage dependence.
    float Isyn_pA();
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
