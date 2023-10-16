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

#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <Simulator/Distributions/Distribution.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/Neuron.h>

//! DtAct_ms to return when there has not been any spike.
#define _NO_SPIKE_DT_mS -1.0
//! Spike potential in mV within absolute refractory period
#define _VSPIKE_ABS_REF_mV 60.0
//! Time span of absolute refractory period in ms
#define _TAU_ABS_mS 1.0
//! Initial value of TSpontNext_ms before any spontaneous activity.
#define _T_SPONT_NEXT_mS_INIT -99.0

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! ReceptorData is a tuple containing a pointer to the source (pre-synaptic)
//! neuron of a connection and its weight.
typedef std::tuple<std::shared_ptr<CoreStructs::Neuron>, float> ReceptorData;

//! NeuronRecording is an unordered map containing data from simulation in a
//! neuron.
typedef std::unordered_map<std::string, std::vector<float>> NeuronRecording;

/**
 * @brief This struct provides the structure representing a basic building block
 * for a ball-and-stick neural circuit.
 *
 */
struct BSNeuron : CoreStructs::Neuron {

    std::unordered_map<std::string, std::shared_ptr<Geometries::Geometry>>
        Morphology;

    float Vm_mV = -60.0;    //! Membrane potential
    float VRest_mV = -60.0; //! Resting membrane potential
    float VAct_mV = -50.0;  //! Action potential firing threshold

    float VAHP_mV = -20.0;  //! After HyperPolarization potential
    float TauAHP_ms = 30.0; //! AfterHyperPolarization decay time constant

    float TauPSPr_ms = 5.0;  //! PostSynaptic Potential rise time constant
    float TauPSPd_ms = 25.0; //! PostSynaptic Potential decay time constant
    float VPSP_mV = 20.0;    //! PostSynaptic Potential amplitude

    std::tuple<float, float> TauSpontMeanStdev_ms = {
        0.0, 0.0}; //! 0 means no spontaneous activity

    float T_ms = 0.0;
    float TSpontNext_ms = _T_SPONT_NEXT_mS_INIT;

    std::vector<float> TAct_ms{};
    std::vector<float> TDirectStim_ms{};
    std::vector<float> CaSamples{};
    std::vector<float> TCaSamples_ms{};

    std::vector<float> TRecorded_ms{};
    std::vector<float> VmRecorded_mV{};
    std::vector<float> FIFO{};
    std::vector<float> ConvolvedFIFO{};
    std::vector<ReceptorData> ReceptorDataVec{};

    std::shared_ptr<Distributions::Distribution>
        DtSpontDist{}; //! Distribution for delta t spontaneous
                       //! (time changed since last spontaneous activity).

    //! Constructors
    BSNeuron(int ID, std::shared_ptr<Geometries::Sphere> soma,
             std::shared_ptr<Geometries::Cylinder> axon);

    //! Returns the geometric center of the neuron.
    Geometries::Vec3D &GetCellCenter();

    //! Records the time of direct stimulation for every occurrence
    //! of a direct stimulation.
    void AttachDirectStim(float t_ms);

    //! Set the distribution for delta t spontaneous (time changed
    //! since last spontaneous activity).
    void SetSpontaneousActivity(float mean, float stdev);

    //! Keeps track of the membrane potential and the time of update.
    void Record(float t_ms);

    //! Returns the recorded membrane potentials.
    NeuronRecording GetRecording();

    //! Tells if the action potential threshold has been crossed.
    bool HasSpiked();

    //! Tells if the time since the latest spike is within the
    //! absolute refractory period threshold.
    bool InAbsRef(float dtAct_ms);

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

    //! We have to flip the signal FIFO, because the most recent is in [0].
    //! We need this, because the kernel has a specific time order.
    //! Alternatively, when we prepare the kernel we can flip it and
    //! remember to view [0] as most recent in the convolution result.
    void UpdateConvolvedFIFO(std::vector<float> kernel);

    //! Sets the initial value of the FIFO.
    void SetFIFO(float FIFO_ms, float dt_ms);
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG