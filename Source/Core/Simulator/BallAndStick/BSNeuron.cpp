#include <Simulator/BallAndStick/BSNeuron.h>

#include <algorithm>
#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

// *** TODO: We could clean up all of these this-> pointers. The virtual functions
//           should still work the same way.

//! Constructors
BSNeuron::BSNeuron(int ID, Geometries::Geometry* soma, Geometries::Geometry* axon) {
    this->ID = ID;
    this->Morphology["soma"] = soma;
    this->Morphology["axon"] = axon;
};

BSNeuron::BSNeuron(const CoreStructs::BSNeuronStruct & bsneuronstruct) {
    build_data = bsneuronstruct;

    ID = bsneuronstruct.ID;
    Vm_mV = bsneuronstruct.MembranePotential_mV;
    VRest_mV = bsneuronstruct.RestingPotential_mV;
    VAct_mV = bsneuronstruct.SpikeThreshold_mV;
    VAHP_mV = bsneuronstruct.AfterHyperpolarizationAmplitude_mV;
    TauAHP_ms = bsneuronstruct.DecayTime_ms;
    TauPSPr_ms = bsneuronstruct.PostsynapticPotentialRiseTime_ms;
    TauPSPd_ms = bsneuronstruct.PostsynapticPotentialDecayTime_ms;
    IPSP_nA = bsneuronstruct.PostsynapticPotentialAmplitude_nA;

    Morphology["soma"] = build_data.SomaCompartmentPtr->ShapePtr;
    Morphology["axon"] = build_data.AxonCompartmentPtr->ShapePtr;
}

//! Returns the geometric center of the neuron.
Geometries::Vec3D &BSNeuron::GetCellCenter() {
    return this->Morphology["soma"]->Center_um;
};

//! Records the time of direct stimulation for every occurrence
//! of a direct stimulation.
// *** WARNING: This does the same as AddSpecificAPTime(), probably delete this one.
void BSNeuron::AttachDirectStim(float t_ms) {
    assert(t_ms >= 0.0);
    this->TDirectStim_ms.push_back(t_ms);
};

//! Set the distribution for delta t spontaneous (time changed
//! since last spontaneous activity).
void BSNeuron::SetSpontaneousActivity(float mean, float stdev, int Seed) {
    this->TauSpont_ms.mean = mean;
    this->TauSpont_ms.stdev = stdev;
    float a = 0.0;
    float b = 2.0*mean;
    this->DtSpontDist = std::make_shared<Distributions::TruncNorm>(a, b, mean, stdev);
    this->DtSpontDist->SetSeed(Seed);
};

//! Keeps track of the membrane potential and the time of update.
void BSNeuron::Record(float t_ms) {
    assert(t_ms >= 0.0);
    this->TRecorded_ms.emplace_back(t_ms);
    this->VmRecorded_mV.emplace_back(this->Vm_mV);
};

//! Returns the recorded membrane potentials.
CoreStructs::NeuronRecording BSNeuron::GetRecording() {
    std::unordered_map<std::string, std::vector<float>> recording;
    recording["Vm_mV"] = this->VmRecorded_mV;
    return recording;
};

nlohmann::json BSNeuron::GetRecordingJSON() const {
    nlohmann::json recording;
    recording["Vm_mV"] = nlohmann::json(this->VmRecorded_mV);
    return recording;
}

//! Tells if the action potential threshold has been crossed.
bool BSNeuron::HasSpiked() {
    this->_has_spiked = this->TAct_ms.size() > 0;
    return this->_has_spiked;
};

//! Returns the time since the action potential threshold was crossed last.
float BSNeuron::DtAct_ms(float t_ms) {
    assert(t_ms >= 0.0);
    if (this->_has_spiked) {
        this->_dt_act_ms = t_ms - this->TAct_ms.back();
        return this->_dt_act_ms;
    }
    return _NO_SPIKE_DT_ms;
};

//! Tells if the time since the latest spike is within the absolute refractory
//! period threshold.

// bool BSNeuron::InAbsRef(float dtAct_ms) {
//     if (dtAct_ms == _NO_SPIKE_DT_ms)
//         return false;

//     assert(dtAct_ms >= 0.0);

//     if (!this->HasSpiked())
//         return false;
//     return dtAct_ms <= _TAU_ABS_mS;
// };

//! Updates V_spike_t.
float BSNeuron::VSpikeT_mV(float t_ms) {
    assert(t_ms >= 0.0);

    // If a spike has not occurred return 0.0
    if (!this->_has_spiked) return 0.0;

    // Update whether the Neuron is in its absolute refractory period.
    this->in_absref = (this->_dt_act_ms <= _TAU_ABS_ms);

    // if within absolute refractory period, return
    // the spike potential during absolute refractory period.
    return this->in_absref ? _VSPIKE_ABS_REF_mV : 0.0;
    // if (this->in_absref) {
    //     return _VSPIKE_ABS_REF_mV;
    // }
    // return 0.0;
};

//! Updates V_AHP_t.
float BSNeuron::VAHPT_mV(float t_ms) {
    assert(t_ms >= 0.0);

    if (!this->_has_spiked) return 0.0;
    if (this->in_absref) return 0.0;

    float vAHPt = this->VAHP_mV * exp(-this->_dt_act_ms / this->TauAHP_ms);

    return vAHPt;
};

//! Updates V_PSP_t.
// Note on conventions: Using small letters (e.g. v) for variables and
// capital letter (e.g. V) for constants, as in the papers.
// (See description of magnitudes involved in flat ground-truth example script.)
/**
 * This postsynaptic update function cares only about information stored
 * in the list of receptorData for this neuron.
 * Morphology information is not inspected.
 * 
 * The double exponential multiplied with the constant IPSP_nA is an
 * approximation of the changing current flow through synaptic ion
 * channels during spike propagation. This is modulated by the
 * inverted conductance that represents the strength of the synaptic
 * connection.
 */
float BSNeuron::VPSPT_mV(float t_ms) {
    assert(t_ms >= 0.0);
    float vPSPt_mV = 0.0;

    for (auto receptorData : this->ReceptorDataVec) {
        auto srcCell = receptorData.SrcNeuronPtr;
        if (!srcCell->HasSpiked()) continue;

        float dtPSP_ms = srcCell->DtAct_ms(t_ms);
        float amp = this->IPSP_nA / receptorData.ReceptorPtr->Conductance_nS;
        //if (ID == 1) std::cout << "IPSP_nA=" << this->IPSP_nA << " Cond nS = " << receptorData.ReceptorPtr->Conductance_nS << " amp = " << amp << '\n';

        vPSPt_mV += SignalFunctions::DoubleExponentExpr(
            amp, 
            receptorData.ReceptorPtr->TimeConstantRise_ms, //this->TauPSPr_ms, 
            receptorData.ReceptorPtr->TimeConstantDecay_ms, //this->TauPSPd_ms,
            dtPSP_ms);
    }
    return vPSPt_mV;
};

//! Updates the momentary membrane potential Vm according to
//! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
void BSNeuron::UpdateVm(float t_ms, bool recording) {
    assert(t_ms >= 0.0);

    float VSpikeT_mV, VAHPT_mV, VPSPT_mV;

    // 1. Prepare data used by vSpike_t and vAHP_t:
    if (this->HasSpiked()) {
        this->DtAct_ms(t_ms);
    }

    // 2. Calculate contributions
    VSpikeT_mV = this->VSpikeT_mV(t_ms);
    VAHPT_mV = this->VAHPT_mV(t_ms);
    VPSPT_mV = this->VPSPT_mV(t_ms);

    // 3. Calculate membrane potential
    this->Vm_mV = this->VRest_mV + VSpikeT_mV + VAHPT_mV + VPSPT_mV;

    // 4. Add voltage elevation to FIFO buffer for phospherecence convolution
    if (!this->FIFO.empty()) {
        // We replace [0], i.e. drop the one at the end and push to front.
        // this->FIFO.pop_back();
        // this->FIFO.push_front(this->Vm_mV - this->VRest_mV);
        // *** TESTING: Newest at back of FIFO (as in SignalFunctions.py demo)
        this->FIFO.pop_front();
        // Flipping sign of Vdiff, clipping at 0, scaling by V_AHP (see SignalFunctions.py demo)
        float v = this->VRest_mV - this->Vm_mV;
        v = v < 0.0 ? 0.0 : v / (-this->VAHP_mV);
        this->FIFO.push_back(v);
    }

    if (recording)
        this->Record(t_ms);
};

//! Compares Vm with Vact.
void BSNeuron::DetectThreshold(float t_ms) {
    assert(t_ms >= 0.0);

    float dtAct_ms = this->DtAct_ms(t_ms);

    //if (dtAct_ms == _NO_SPIKE_DT_ms) return;
    if (this->in_absref) return;

    if (this->Vm_mV >= this->VAct_mV) this->TAct_ms.push_back(t_ms);
};

//! Checks for possible spontaneous activity.
void BSNeuron::SpontaneousActivity(float t_ms) {
    assert(t_ms >= 0.0);

    if (this->in_absref) return;
    if (this->TauSpont_ms.stdev == 0) return;

    // Have we passed the next scheduled spontaneous event?
    if (t_ms >= this->TSpontNext_ms) {

        // Register a spike threshold crossing and the start of an action potential.
        if (this->TSpontNext_ms >= 0) this->TAct_ms.push_back(t_ms);

        // Obtain interval to the next spontaneous event from the distribution.
        float dt_spont = this->DtSpontDist->RandomSample(1)[0]; // Generate 1 random sample in a vector, take the 0th element.
        this->TSpontNext_ms = t_ms + dt_spont;
    }
};

//! Updates all potential components, the membrane potential
//! and the time of update.
void BSNeuron::Update(float t_ms, bool recording) {
    assert(t_ms >= 0.0);

    float tDiff_ms = t_ms - this->T_ms;
    if (tDiff_ms < 0) return;

    // 1. Has there been a directed stimulation?
    if (!(this->TDirectStim_ms.empty())) {
        float tFire_ms = this->TDirectStim_ms.front();
        if (tFire_ms <= t_ms) {
            this->TAct_ms.push_back(tFire_ms);
            //this->TDirectStim_ms.erase(this->TDirectStim_ms.begin());
            this->TDirectStim_ms.pop_front();
        }
    }

    // 2. Update variables.
    this->UpdateVm(t_ms, recording);
    this->DetectThreshold(t_ms);
    this->SpontaneousActivity(t_ms);

    // 3. Remember the update time.
    this->T_ms = t_ms;
};

//! Sets the initial value of the FIFO and prepares a buffer for convolvedFIFO.
//! FIFO_dt_ms == 0.0 means used a FIFO of size 1.
void BSNeuron::SetFIFO(float FIFO_ms, float FIFO_dt_ms, size_t reversed_kernel_size) {
    assert(FIFO_ms >= 0.0 && FIFO_dt_ms >= 0.0);

    size_t fifoSize = FIFO_dt_ms == 0.0 ? 1 : FIFO_ms / FIFO_dt_ms + 1;

    for (size_t i = 0; i < fifoSize; ++i) this->FIFO.emplace_back(0.0);

    size_t convolvedFIFO_size = this->FIFO.size() + reversed_kernel_size - 1;
    this->ConvolvedFIFO.resize(convolvedFIFO_size, 0.0);
};

//! NOTE: SetFIFO must be called first, otherwise the FIFO is not
//!       updated in UpdateVm.
//! NOTE: We flip signal FIFO, because most recent is in [0] and kernel
//!       has specific time-order.
//!       (Alternatively, we could flip the prepared kernel and take
//!       care to view [0] as most recent in covolution result.)
//! NOTE: For efficiency (see Convolve1D), we provide a reversed kernel
//!       that was reversed and stored during initialization.
//! The FIFO buffer contains a record of the difference between Vm and Vrest
//! at current and preceding time points.
void BSNeuron::UpdateConvolvedFIFO(const std::vector<float> & reversed_kernel) {
    assert(!reversed_kernel.empty());

    // *** TESTING non-reversal of anything (see SignalFuctions.py)
    // Reverse signal FIFO:
    // std::vector<float> CaSignal(this->FIFO.size(), 0.0);
    // std::reverse_copy(this->FIFO.begin(), this->FIFO.end(), CaSignal.begin());

    // Convolve with kernel:
    if (SignalFunctions::Convolve1D(FIFO, reversed_kernel, this->ConvolvedFIFO)) {

        // Record Ca value with offset and record time-point:
        // Convolution results applicable to recent Vdiff are at end of ConvolvedFIFO (see SignalFunctions.py)
        this->CaSamples.emplace_back(ConvolvedFIFO[ConvolvedFIFO.size()-10]); //+1.0);
// if (ID==0) {
//     std::cout << "DEBUG --> New Convolved value: " << this->CaSamples.back() << '\n';
//     std::cout << "Kernel: ";
//     for (auto & v : reversed_kernel) {
//         std::cout << v << ' ';
//     }
//     std::cout << "\nFIFO: ";
//     for (auto & v : FIFO) {
//         std::cout << v << ' ';
//     }
//     std::cout << "\nConvolved: ";
//     for (auto & v : ConvolvedFIFO) {
//         std::cout << v << ' ';
//     }
//     std::cout << '\n';
// }
        this->TCaSamples_ms.emplace_back(this->T_ms);
    }
};

void BSNeuron::InputReceptorAdded(CoreStructs::ReceptorData RData) {
    ReceptorDataVec.emplace_back(RData);

}

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
