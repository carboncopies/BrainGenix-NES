#include <Simulator/Receptors/NMDAReceptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

//! Constructors
NMDAReceptor::NMDAReceptor(){};

NMDAReceptor::NMDAReceptor(size_t _ID, float _GSyn_pS, float _GPeak_pS,
                           float _ESyn_mV, float _Vm_mV, float _ISyn_pA,
                           float _TauD_ms, float _TauR_ms, float _TauD2_ms,
                           float _TauD3_ms, float _D1, float _D2, float _D3,
                           float _X, float _ANorm, float _VHalfBlocked_mV,
                           float _K) {
    this->ID = _ID;
    this->GSyn_pS = _GSyn_pS;
    this->GPeak_pS = _GPeak_pS;
    this->ESyn_mV = _ESyn_mV;
    this->Vm_mV = _Vm_mV;
    this->ISyn_pA = _ISyn_pA;
    this->TauD_ms = _TauD_ms;
    this->TauR_ms = _TauR_ms;
    this->TauD2_ms = _TauD2_ms;
    this->TauD3_ms = _TauD3_ms;
    this->D1 = _D1;
    this->D2 = _D2;
    this->D3 = _D3;
    this->X = _X;
    this->ANorm = _ANorm;
    this->VHalfBlocked_mV = _VHalfBlocked_mV;
    this->K = _K;
};

void NMDAReceptor::SetPhiVType(PhiVType phiVType) { return; };

//! Modeled with a Bolzmann function. Easy to use, but not directly
//! related to physical aspects of Mg2+ blocking mechanism.
float NMDAReceptor::PhiVBolzmann() { return 0.0; };

//! temperature_K - absolute temperatue
//! z - valence of blocking ion (e.g. +2 for Mg2+)
float NMDAReceptor::Phi(float temperature_K, int z) { return 0.0; };

//! Mg2plus_0 - Mg2+ concentration outside membrane.
//! KBinding - Binding constant.
//! delta - Fraction of membrane voltage that Mg2+ experiences at the
//! blocking site. temperature_K - absolute temperature. voltage_V -
//! voltage.
float NMDAReceptor::KBindingRate(float Mg2plus_0, float KBinding, float delta,
                                 float V, float temperature_K) {
    return 0.0;
};

//! KUnbinding - Unbinding constant.
//! delta - Fraction of membrane voltage that Mg2+
//!         experiences at the blocking site.
float NMDAReceptor::KUnbindingRate(float KUnbinding, float delta,
                                   float voltage_V, float temperature_K) {
    return 0.0;
};

//! Modeled with a two-state Woodhull formalism derived from a kinetic
//! model of an extracellular Mg2+ block. An ion channel is blocked
//! when an ion species (here, Mg2+)	is bound to a binding site
//! inside the channel, open when the ion species is unbound.
//! Mg2plus_0 - Mg2+ concentration outside membrane.
//! KBinding - Binding constant.
//! KUnbinding - Unbinding constant.
//! delta - Fraction of membrane voltage that Mg2+ experiences at the
//! blocking site.

float NMDAReceptor::PhiVWoodhull_1(float Mg2plus_0, float KBinding,
                                   float KUnbinding, float delta,
                                   float voltage_V, float temperature_K) {
    return 0.0;
};

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
float NMDAReceptor::PhiVWoodhull_2(float Mg2plus_0, float K_dissociation_0mV,
                                   float delta, float voltage_V,
                                   float temperature_K) {
    return 0.0;
};

//! Strong voltage dependence.
float NMDAReceptor::Isyn_pA() { return 0.0; };

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
