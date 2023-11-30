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
                           float _K, float _KBinding, float _KUnbinding,
                           float _KDissociation_0mV) {
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
    this->KBinding = _KBinding;
    this->KUnbinding = _KUnbinding;
    this->KDissociation_0mV = _KDissociation_0mV;
};

float NMDAReceptor::PhiV(PhiVType phiVType, float Mg2plus_0, float delta,
                         float voltage_V, float temperature_K) {
    float answer = 0.0;

    switch (phiVType) {
    case PhiVType::BOLZMANN:
        answer = this->PhiVBolzmann();
        break;
    case PhiVType::WOODHULL_1:
        answer =
            this->PhiVWoodhull1(Mg2plus_0, delta, voltage_V, temperature_K);
        break;
    case PhiVType::WOODHULL_2:
        answer =
            this->PhiVWoodhull2(Mg2plus_0, delta, voltage_V, temperature_K);
        break;
    default:
        break;
    }
    return answer;
};

//! Modeled with a Bolzmann function. Easy to use, but not directly
//! related to physical aspects of Mg2+ blocking mechanism.
float NMDAReceptor::PhiVBolzmann() {
    if (this->K == 0.0)
        throw std::overflow_error("Cannot divide by zero.(K)");
    return 1.0 / (1.0 + exp(-(this->Vm_mV - this->VHalfBlocked_mV) / this->K));
};

//! temperature_K - absolute temperatue
//! z - valence of blocking ion (e.g. +2 for Mg2+)
float NMDAReceptor::Phi(float temperature_K, int z) {
    if (temperature_K == 0.0)
        throw std::overflow_error("Cannot divide by zero.(temperature_K)");
    return z * _F / (_R * temperature_K);
};

//! Mg2plus_0 - Mg2+ concentration outside membrane.
//! delta - Fraction of membrane voltage that Mg2+ experiences at the
//! blocking site. temperature_K - absolute temperature.
//! voltage_V - voltage.
float NMDAReceptor::KBindingRate(float Mg2plus_0, float delta, float voltage_V,
                                 float temperature_K) {
    return Mg2plus_0 * this->KBinding *
           exp(-delta * this->Phi(temperature_K) * voltage_V / 2.0);
};

//! delta - Fraction of membrane voltage that Mg2+
//!         experiences at the blocking site.
float NMDAReceptor::KUnbindingRate(float delta, float voltage_V,
                                   float temperature_K) {
    return this->KUnbinding *
           exp(delta * this->Phi(temperature_K) * voltage_V / 2.0);
};

//! Modeled with a two-state Woodhull formalism derived from a kinetic
//! model of an extracellular Mg2+ block. An ion channel is blocked
//! when an ion species (here, Mg2+) is bound to a binding site
//! inside the channel, open when the ion species is unbound.
//! Mg2plus_0 - Mg2+ concentration outside membrane.
//! delta - Fraction of membrane voltage that Mg2+ experiences at the
//! blocking site.

float NMDAReceptor::PhiVWoodhull1(float Mg2plus_0, float delta, float voltage_V,
                                  float temperature_K) {
    if (this->KUnbinding == 0.0)
        throw std::overflow_error("Cannot divide by zero.(KUnbinding)");
    float KBindingRate =
        this->KBindingRate(Mg2plus_0, delta, temperature_K, voltage_V);
    float KUnbindingRate =
        this->KUnbindingRate(delta, temperature_K, voltage_V);

    return 1.0 / (1.0 + KBindingRate / KUnbindingRate);
};

//! Modeled with a two-state Woodhull formalism derived from a kinetic
//! model of extracellular Mg2+ block. An ion channel is blocked when an
//! ion species (here, Mg2+)	is bound to a binding site inside the
//! channel, open when the ion species is unbound.  K_dissociation_0mV
//! quantifies the strength or affinity of Mg2+ binding and delta quantifies
//! the location of the Mg2+ binding site within the channel.
//! Mg2plus_0 - Mg2+ concentration outside membrane.
//! delta - Fraction of membrane voltage that Mg2+
//! experiences at the blocking site.
float NMDAReceptor::PhiVWoodhull2(float Mg2plus_0, float delta, float voltage_V,
                                  float temperature_K) {
    if (this->KDissociation_0mV == 0.0)
        throw std::overflow_error("Cannot divide by zero.(KDissociation_0mV)");
    float KD = this->KDissociation_0mV *
               exp(delta * this->Phi(temperature_K) * voltage_V);

    return 1.0 / (1.0 + Mg2plus_0 / KD);
};

//! Strong voltage dependence.
float NMDAReceptor::PostSynCurrentStrongVoltageDep_pA(PhiVType phiVType,
                                                      float Mg2plus_0,
                                                      float delta,
                                                      float voltage_V,
                                                      float temperature_K) {
    return this->PhiV(phiVType, Mg2plus_0, delta, temperature_K, voltage_V) *
           this->PostSynCurrent_pA();
    ;
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
