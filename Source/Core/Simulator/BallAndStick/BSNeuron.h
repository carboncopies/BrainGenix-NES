//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BSNeuron struct, representing
                 a basic building block for a ball-and-stick neural circuit.
    Additional Notes: None
    Date Created: 2023-10-09
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <Simulator/Distributions/Distribution.h>
#include <Simulator/Distributions/TruncNorm.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/SignalFunctions.h>
//#include <Simulator/Structs/Receptor.h>
//#include <Simulator/Structs/Simulation.h>

//! DtAct_ms to return when there has not been any spike.
#define _NO_SPIKE_DT_ms -1.0
//! Spike potential in mV within absolute refractory period
#define _VSPIKE_ABS_REF_mV 60.0
//! Time span of absolute refractory period in ms
#define _TAU_ABS_ms 1.0
//! Initial value of TSpontNext_ms before any spontaneous activity.
#define _T_SPONT_NEXT_mS_INIT -99.0

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

struct SpontaneousActivityPars {
    float mean = 0.0;
    float stdev = 0.0; // 0.0 means no spontaneous activity
};

/**
 * @brief This struct provides the structure representing a basic building block
 * for a ball-and-stick neural circuit.
 *
 */
class BSNeuron : public CoreStructs::Neuron {
public:
    CoreStructs::BSNeuronStruct build_data; // Copy of the struct that was used to build this neuron.

    //std::unordered_map<std::string, std::shared_ptr<Geometries::Geometry>> Morphology;
    std::unordered_map<std::string, Geometries::Geometry*> Morphology; // Regular pointers, because the objects are maintained in Simulation.Collection.

    float Vm_mV = -60.0;    //! Membrane potential
    float VRest_mV = -60.0; //! Resting membrane potential
    float VAct_mV = -50.0;  //! Action potential firing threshold

    float VAHP_mV = -20.0;  //! After HyperPolarization potential
    float TauAHP_ms = 30.0; //! AfterHyperPolarization decay time constant

    float TauPSPr_ms = 5.0;  //! PostSynaptic Potential rise time constant
    float TauPSPd_ms = 25.0; //! PostSynaptic Potential decay time constant
    float IPSP_nA = 870.0;   //! PostSynaptic Potential current amplitude (at vPSP peak)

    SpontaneousActivityPars TauSpont_ms;

    float T_ms = 0.0;
    float TSpontNext_ms = _T_SPONT_NEXT_mS_INIT;

    bool _has_spiked = false;
    bool in_absref = false;
    float _dt_act_ms = 0.0;

    // Moved to Neuron.h: std::vector<float> TAct_ms{};
    // Moved to Neuron.h: std::vector<float> TDirectStim_ms{}; // Or should this be a deque?
    std::vector<float> CaSamples{};
    std::vector<float> TCaSamples_ms{};

    std::vector<float> TRecorded_ms{};
    std::vector<float> VmRecorded_mV{};
    std::deque<float> FIFO{};
    std::vector<float> ConvolvedFIFO{};
    std::vector<CoreStructs::ReceptorData> ReceptorDataVec{};

    std::shared_ptr<Distributions::Distribution> DtSpontDist{}; //! Distribution for delta t spontaneous (time changed since last spontaneous activity).

protected:
    BSNeuron(int _ID) { ID = _ID; } // Only used by derived classes that inherit this, e.g. SCNeuron. 

public:
    //! Constructors
    BSNeuron(int ID, Geometries::Geometry* soma, Geometries::Geometry* axon); // Geometries, as soma and axon may not be Sphere/Cylinder in future. //std::shared_ptr<Geometries::Geometry> soma, std::shared_ptr<Geometries::Geometry> axon);
    BSNeuron(const CoreStructs::BSNeuronStruct & bsneuronstruct);
    //BSNeuron(Simulation & sim, const CoreStructs::BSNeuronStruct & bsneuronstruct);

    //! Returns the geometric center of the neuron.
    virtual Geometries::Vec3D &GetCellCenter();

    //! Records the time of direct stimulation for every occurrence
    //! of a direct stimulation.
    void AttachDirectStim(float t_ms);

    //! Set the distribution for delta t spontaneous (time changed
    //! since last spontaneous activity).
    //! A stdev==0 means the Neuron does not experience spontaneous activity.
    virtual void SetSpontaneousActivity(float mean, float stdev, int Seed);

    //! Keeps track of the membrane potential and the time of update.
    void Record(float t_ms);

    //! Returns the recorded membrane potentials.
    CoreStructs::NeuronRecording GetRecording();
    nlohmann::json GetRecordingJSON() const;

    //! Tells if the action potential threshold has been crossed.
    bool HasSpiked();

    //! Tells if the time since the latest spike is within the
    //! absolute refractory period threshold.
    //bool InAbsRef(float dtAct_ms);

    //! Returns the time since the action potential threshold was
    //! crossed last.
    float DtAct_ms(float t_ms);

    //! Updates V_spike_t.
    float VSpikeT_mV(float t_ms);

    //! Updates V_AHP_t.
    float VAHPT_mV(float t_ms);

    //! Updates V_PSP_t.
    float VPSPT_mV(float t_ms);

    //! Updates the momentary membrane potential Vm according to
    //! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
    void UpdateVm(float t_ms, bool recording);

    //! Compares Vm with Vact.
    void DetectThreshold(float t_ms);

    //! Checks for possible spontaneous activity.
    void SpontaneousActivity(float t_ms);

    //! Updates all potential components, the membrane potential
    //! and the time of update.
    void Update(float t_ms, bool recording);

    //! NOTE: SetFIFO must be called first, otherwise the FIFO is not
    //!       updated in UpdateVm.
    //! NOTE: We flip signal FIFO, because most recent is in [0] and kernel
    //!       has specific time-order.
    //!       (Alternatively, we could flip the prepared kernel and take
    //!       care to view [0] as most recent in covolution result.)
    //! NOTE: For efficiency (see Convolve1D), we provide a reversed kernel
    //!       that was reversed and stored during initialization.
    void UpdateConvolvedFIFO(const std::vector<float> & reversed_kernel);

    //! Sets the initial value of the FIFO and prepares a buffer for convolvedFIFO.
    //! FIFO_dt_ms == 0.0 means used a FIFO of size 1.
    void SetFIFO(float FIFO_ms, float FIFO_dt_ms, size_t reversed_kernel_size);

    virtual void InputReceptorAdded(CoreStructs::ReceptorData RData);
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
