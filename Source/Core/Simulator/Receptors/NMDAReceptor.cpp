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
                           float _K)
    : ID(_ID), GSyn_pS(_GSyn_pS), GPeak_pS(_GPeak_pS), ESyn_mV(_ESyn_mV),
      Vm_mV(_Vm_mV), ISyn_pA(_ISyn_pA), TauD_ms(_TauD_ms), TauR_ms(_TauR_ms),
      TauD2_ms(_TauD2_ms), TauD3_ms(_TauD3_ms), D1(_D1), D2(_D2), D3(_D3),
      X(_X), ANorm(_ANorm), VHalfBlocked_mV(_VHalfBlocked_mV), K(_K){};

float NMDAReceptor::PostSynCurrent_pA() { return 0.0; };

float NMDAReceptor::Conductance_pS() { return 0.0; };

float NMDAReceptor::GSynT_pS(float t_ms, NMDAReceptor::PSPType pspType) {
    return 0.0;
};
std::vector<float> NMDAReceptor::GSynT_pS(std::vector<float> t_ms,
                                          NMDAReceptor::PSPType pspType) {
    return std::vector<float>();
};

//! Modeled with a simple exponential.
float NMDAReceptor::GSynT_pS_DecayZeroRiseTime(float t_ms) { return 0.0; };
std::vector<float>
NMDAReceptor::GSynT_pS_DecayZeroRiseTime(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with an alpha function.
float NMDAReceptor::GSynT_pS_RiseDecay(float t_ms) { return 0.0; };
std::vector<float> NMDAReceptor::GSynT_pS_RiseDecay(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with a double exponential.
float NMDAReceptor::GSynT_pS_DoubleExponential(float t_ms) { return 0.0; };
std::vector<float>
NMDAReceptor::GSynT_pS_DoubleExponential(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with a multiexponential function with m^xh formalism to fit
//! more complex waveforms.
float NMDAReceptor::GSynT_pS_MXH(float t_ms) { return 0.0; };
std::vector<float> NMDAReceptor::GSynT_pS_MXH(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! For analytical solution see Roth & van Rossum (2009).
float NMDAReceptor::FindANorm(bool verbose) { return 0.0; };

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
