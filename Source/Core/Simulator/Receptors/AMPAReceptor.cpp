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
                           float _X, float _ANorm) {
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
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
