#include <cassert>
#include <memory>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Distributions/TruncNorm.h>

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
    this->tDirectStim_ms.push_back(t_ms);
};

//! Set the distribution for delta t spontaneous (time changed
//! since last spontaneous activity).
void BSNeuron::SetSpontaneousActivity(float mean, float stdev) {
    this->tauSpontMeanStdev_ms = std::make_tuple(mean, stdev);
    this->dtSpontDist = std::make_unique<Distributions::TruncNorm>(
            -mean / stdev, mean / stdev, mean, stdev);
};

//! Keeps track of the membrane potential and the time of update.
void BSNeuron::Record(float t_ms) {
    assert(t_ms >= 0.0);
    this->tRecorded_ms.emplace_back(t_ms);
    this->VmRecorded_mV.emplace_back(this->Vm_mV);
};

//! Returns the recorded membrane potentials.
std::unordered_map<std::string, std::vector<float>> BSNeuron::GetRecording() {
    std::unordered_map<std::string, std::vector<float>> recording;
    return recording;
};

//! Tells if the action potential threshold has been crossed.
bool BSNeuron::HasSpiked() {
    return (this->tAct_ms).size() > 0;
};

//! Returns the time since the action potential threshold was
//! crossed last.
float BSNeuron::DtAct_ms(float t_ms) {
    assert(t_ms >= 0.0);
    if (this->HasSpiked())
        return t_ms - this->tAct_ms.back();
    return _NO_SPIKE_DT_mS;
};


//! Tells if the time since the latest spike is within the absolute refractory period threshold.
bool BSNeuron::InAbsRef(float dtAct_ms) {
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

    return this->VAHP_mV * exp(-this->DtAct_ms(t_ms) / this->tauAHP_ms);
};

//! Updates V_PSP_t.
float BSNeuron::VPSPT_mV(float t_ms) {
    return -99999999.0;
};

//! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
//! Compares Vm with Vact.
void BSNeuron::UpdateVm(float t_ms, bool recording) {
    return;
};

//! Compares Vm with Vact.
void BSNeuron::DetectThreshold(float t_ms) {
    return;
};

//! Checks for possible spontaneous activity.
void BSNeuron::SpontaneousActivity(float t_ms) {
    return;
};

//! Updates all potential components, the membrane potential
//! and the time of update.
void BSNeuron::Update(float t_ms, bool recording) {
    return;
};

//! We have to flip the signal FIFO, because the most recent is in [0].
//! We need this, because the kernel has a specific time order.
//! Alternatively, when we prepare the kernel we can flip it and
//! remember to view [0] as most recent in the convolution result.
void BSNeuron::UpdateConvolvedFIFO(std::vector<float> kernel) {
    return;
};

//! Sets the initial value of the FIFO.
void BSNeuron::SetFIFO(float FIFO_ms, float dt_ms) {
    return;
};


}; // Close Namespace BallAndStick
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
