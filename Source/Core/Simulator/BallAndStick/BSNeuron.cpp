#include <cassert>
#include <memory>
#include <algorithm>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Distributions/TruncNorm.h>
#include <Simulator/Structs/SignalFunctions.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Constructors
BSNeuron::BSNeuron(int ID, 
            std::shared_ptr<Geometries::Sphere> soma,
            std::shared_ptr<Geometries::Cylinder> axon
    ) {
    this->ID = ID;
    this->morphology["soma"] = soma;
    this->morphology["axon"] = axon;
};

//! Returns the geometric center of the neuron.
Geometries::Vec3D& BSNeuron::GetCellCenter() {
    return this->morphology["soma"]->Center_um;
};

//! Records the time of direct stimulation for every occurrence
//! of a direct stimulation.
void BSNeuron::AttachDirectStim(float t_ms) {
    assert(t_ms >= 0.0);
    this->TDirectStim_ms.push_back(t_ms);
};

//! Set the distribution for delta t spontaneous (time changed
//! since last spontaneous activity).
void BSNeuron::SetSpontaneousActivity(float mean, float stdev) {
    this->TauSpontMeanStdev_ms = std::make_tuple(mean, stdev);
    this->DtSpontDist = std::make_unique<Distributions::TruncNorm>(
            -mean / stdev, mean / stdev, mean, stdev);
};

//! Keeps track of the membrane potential and the time of update.
void BSNeuron::Record(float t_ms) {
    assert(t_ms >= 0.0);
    this->TRecorded_ms.emplace_back(t_ms);
    this->VmRecorded_mV.emplace_back(this->Vm_mV);
};

//! Returns the recorded membrane potentials.
std::unordered_map<std::string, std::vector<float>> BSNeuron::GetRecording() {
    std::unordered_map<std::string, std::vector<float>> recording;
    recording["Vm_mV"] = this->VmRecorded_mV;
    return recording;
};

//! Tells if the action potential threshold has been crossed.
bool BSNeuron::HasSpiked() {
    return (this->TAct_ms).size() > 0;
};

//! Returns the time since the action potential threshold was
//! crossed last.
float BSNeuron::DtAct_ms(float t_ms) {
    assert(t_ms >= 0.0);
    if (this->HasSpiked())
        return t_ms - this->TAct_ms.back();
    return _NO_SPIKE_DT_mS;
};


//! Tells if the time since the latest spike is within the absolute refractory period threshold.
bool BSNeuron::InAbsRef(float dtAct_ms) {
    if (dtAct_ms==_NO_SPIKE_DT_mS) return false;

    assert(dtAct_ms >= 0.0);

    if (!this->HasSpiked()) return false;
    return dtAct_ms <= _TAU_ABS_mS;
};


//! Updates V_spike_t.
float BSNeuron::VSpikeT_mV(float t_ms){\
    assert(t_ms>=0.0);
    
    // If a spike has not occurred return 0.0
    if (!this->HasSpiked()) return 0.0;

    // if within absolute refractory period, return 
    // the spike potential during absolute refractory period.
    return this->InAbsRef(this->DtAct_ms(t_ms)) ? _VSPIKE_ABS_REF_mV : 0.0;
};

//! Updates V_AHP_t.
float BSNeuron::VAHPT_mV(float t_ms) {
    assert(t_ms>=0.0);
    
    if (!this->HasSpiked()) return 0.0;
    if (this->InAbsRef(this->DtAct_ms(t_ms))) return 0.0;

    return this->VAHP_mV * exp(-this->DtAct_ms(t_ms) / this->TauAHP_ms);
};

//! Updates V_PSP_t.
float BSNeuron::VPSPT_mV(float t_ms) {
    assert(t_ms>=0.0);
    float VPSPT_mV = 0.0;

    for (auto receptorData: this->ReceptorDataVec) {
        auto srcCell = std::get<0>(receptorData);
        if (!srcCell->HasSpiked()) 
            continue;
        
        float weight = std::get<1>(receptorData);
        float dtPSP_ms = srcCell->DtAct_ms(t_ms);
        
        VPSPT_mV += SignalFunctions::DoubleExponentExpr(weight * this->VPSP_mV, 
                this->TauPSPr_ms, this->TauPSPd_ms, dtPSP_ms);
    }
    return VPSPT_mV;
};

//! Updates the momentary membrane potential Vm according to
//! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
void BSNeuron::UpdateVm(float t_ms, bool recording) {
    assert(t_ms >= 0.0);

    float dtAct_ms = 0.0;
    float VSpikeT_mV, VAHPT_mV, VPSPT_mV;

    // 1. Prepare data used by VSpike_t and V_AHP_t
    if (this->HasSpiked())
        dtAct_ms = this->DtAct_ms(t_ms);

    // 2. Calculate contributions
    VSpikeT_mV = this->VSpikeT_mV(t_ms);
    VAHPT_mV = this->VAHPT_mV(t_ms);
    VPSPT_mV = this->VPSPT_mV(t_ms);

    // 3. Calculate membrane potential
    this->Vm_mV = this->VRest_mV + VSpikeT_mV + VAHPT_mV + VPSPT_mV;

    if (!this->FIFO.empty()) {
        this->FIFO.erase(this->FIFO.begin());
        this->FIFO.push_back(this->Vm_mV - this->VRest_mV);
    }

    if (recording)
        this->Record(t_ms);
};

//! Compares Vm with Vact.
void BSNeuron::DetectThreshold(float t_ms) {
    assert(t_ms >= 0.0);

    float dtAct_ms = this->DtAct_ms(t_ms);

    if (dtAct_ms == _NO_SPIKE_DT_mS) return;
    if (this->InAbsRef(dtAct_ms)) return;
    
    if (this->Vm_mV >= this->VAct_mV)
        this->TAct_ms.push_back(t_ms);
};

//! Checks for possible spontaneous activity.
void BSNeuron::SpontaneousActivity(float t_ms) {
    assert(t_ms>=0.0);

    float dtAct_ms = this->DtAct_ms(t_ms);
    
    if (this->InAbsRef(dtAct_ms)) return;
    if (std::get<0>(this->TauSpontMeanStdev_ms) == 0) return;
    
    if (t_ms >= this->TSpontNext_ms) {
        if (this->TSpontNext_ms >= 0)
            this->TAct_ms.push_back(t_ms);
        this->TSpontNext_ms = t_ms + this->DtSpontDist->RandomSample(1)[0];
    }
};

//! Updates all potential components, the membrane potential
//! and the time of update.
void BSNeuron::Update(float t_ms, bool recording) {
    assert(t_ms>=0.0);
    
    float tDiff_ms = t_ms - this->T_ms;
    if (tDiff_ms < 0) return;

    // 1. Has there been a directed stimulation?
    if (!(this->TDirectStim_ms.empty())) {
        float tFire_ms = this->TDirectStim_ms.front();
        if (tFire_ms <= t_ms) {
            this->TAct_ms.push_back(tFire_ms);
            this->TDirectStim_ms.erase(this->TDirectStim_ms.begin());
        }
    }

    // 2. Update variables.
    this->UpdateVm(t_ms, recording);
    this->DetectThreshold(t_ms);
    this->SpontaneousActivity(t_ms);

    // 3. Remember the update time.
    this->T_ms = t_ms;
};

//! Sets the initial value of the FIFO.
void BSNeuron::SetFIFO(float FIFO_ms, float dt_ms) {
    assert(FIFO_ms>=0.0 && dt_ms>=0.0);

    size_t fifoSize = dt_ms == 0.0? 1: FIFO_ms / dt_ms + 1;

    for (size_t i=0; i<fifoSize; ++i)
        this->FIFO.emplace_back(0.0);
};

//! We have to flip the signal FIFO, because the most recent is in [0].
//! We need this, because the kernel has a specific time order.
//! Alternatively, when we prepare the kernel we can flip it and
//! remember to view [0] as most recent in the convolution result.
void BSNeuron::UpdateConvolvedFIFO(std::vector<float> kernel) {
    assert(!kernel.empty());

    std::vector<float> CaSignal = std::vector<float>(this->FIFO);

    std::reverse(CaSignal.begin(), CaSignal.end());

    for (size_t i=0; i<CaSignal.size(); ++i) {
        CaSignal[i] *= -1.0;
        if (CaSignal[i] < 0.0)
            CaSignal[i] = 0.0;
    }

    this->ConvolvedFIFO = SignalFunctions::Convolve1D(CaSignal, kernel);
    
    this->CaSamples.emplace_back(this->ConvolvedFIFO.back() + 1.0);
    this->TCaSamples_ms.emplace_back(this->T_ms);
};

}; // Close Namespace BallAndStick
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
