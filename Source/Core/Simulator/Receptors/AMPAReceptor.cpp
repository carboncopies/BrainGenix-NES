#include <Simulator/Receptors/AMPAReceptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

//! Constructors
AMPAReceptor::AMPAReceptor(){};

AMPAReceptor::AMPAReceptor(size_t _ID, float _GSyn_pS, float _GPeak_pS,
                           float _ESyn_mV, float _Vm_mV, float _ISyn_pA,
                           float _TauD_ms, float _TauR_ms, float _TauD2_ms,
                           float _TauD3_ms, float _D1, float _D2, float _D3,
                           float _X, float _ANorm)
    : ID(_ID), GSyn_pS(_GSyn_pS), GPeak_pS(_GPeak_pS), ESyn_mV(_ESyn_mV),
      Vm_mV(_Vm_mV), ISyn_pA(_ISyn_pA), TauD_ms(_TauD_ms), TauR_ms(_TauR_ms),
      TauD2_ms(_TauD2_ms), TauD3_ms(_TauD3_ms), D1(_D1), D2(_D2), D3(_D3),
      X(_X), ANorm(_ANorm){};

float AMPAReceptor::PostSynCurrent_pA() { return 0.0; };

float AMPAReceptor::Conductance_pS() { return 0.0; };

float AMPAReceptor::GSynT_pS(float t_ms, AMPAReceptor::PSPType pspType) {
    return 0.0;
};
std::vector<float> AMPAReceptor::GSynT_pS(std::vector<float> t_ms,
                                          AMPAReceptor::PSPType pspType) {
    return std::vector<float>();
};

//! Modeled with a simple exponential.
float AMPAReceptor::GSynT_pS_DecayZeroRiseTime(float t_ms) { return 0.0; };
std::vector<float>
AMPAReceptor::GSynT_pS_DecayZeroRiseTime(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with an alpha function.
float AMPAReceptor::GSynT_pS_RiseDecay(float t_ms) { return 0.0; };
std::vector<float> AMPAReceptor::GSynT_pS_RiseDecay(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with a double exponential.
float AMPAReceptor::GSynT_pS_DoubleExponential(float t_ms) { return 0.0; };
std::vector<float>
AMPAReceptor::GSynT_pS_DoubleExponential(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! Modeled with a multiexponential function with m^xh formalism to fit
//! more complex waveforms.
float AMPAReceptor::GSynT_pS_MXH(float t_ms) { return 0.0; };
std::vector<float> AMPAReceptor::GSynT_pS_MXH(std::vector<float> t_ms) {
    return std::vector<float>();
};

//! For analytical solution see Roth & van Rossum (2009).
float AMPAReceptor::FindANorm(bool verbose) { return 0.0; };

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
