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

#include <vector>
#include <tuple>
#include <unordered_map>
#include <memory>

#include <Simulator/Structs/Neuron.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/VecTools.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

/**
 * @brief This struct provides the structure representing a basic building block for a ball-and-stick neural circuit.
 * 
 */
struct BSNeuron: CoreStructs::Neuron {

    std::unordered_map<std::string, 
        std::shared_ptr<Geometries::Geometry>> morphology;

    float Vm_mV = -60.0;      //! Membrane potential
    float VRest_mV = -60.0;   //! Resting membrane potential
    float VAct_mV = -50.0;    //! Action potential firing threshold

    float VAHP_mV = -20.0;    //! After HyperPolarization potential
    float tauAHP_ms = 30.0;   //! AfterHyperPolarization decay time constant

    float tauPSPr_ms = 5.0;   //! PostSynaptic Potential rise time constant
    float tauPSPd_ms = 25.0;  //! PostSynaptic Potential decay time constant
    float VPSP_mV = 20.0;     //! PostSynaptic Potential amplitude

    std::tuple<float, float> tauSpontMeanStdev_ms = {0.0, 0.0}; //! 0 means no spontaneous activity
    
    float t_ms = 0.0;
    float tSpontNext_ms = -1.0;
    float _dt_act_ms = -999999999.0;

    bool _hasSpiked = false;
    bool inAbsref = false;

    std::vector<float> tAct_ms;
    std::vector<float> tDirectStim_ms;
    std::vector<float> CaSamples;
    std::vector<float> tCaSamples_ms;

    std::vector<float> tRecorded_ms;
    std::vector<float> VmRecorded_mV;
    std::vector<float> FIFO;
    std::vector<float> convolved_FIFO;

    // std::vector<std::shared_ptr<BG::NES::Receptors::Receptor>> receptors;
    // float dt_spont_dist = None
    
    //! Constructors
    BSNeuron(int ID, 
            std::shared_ptr<Geometries::Sphere> soma,
            std::shared_ptr<Geometries::Cylinder> axon
    );

    //! Returns the geometric center of the neuron.
    Geometries::Vec3D& GetCellCenter();

    //! Records the time of direct stimulation for every occurrence
    //! of a direct stimulation.
    void AttachDirectStim(float t_ms);

    //!
    // void SetSpontaneousActivity(std::tuple<float, float> meanStdev);
    
    //! Keeps track of the membrane potential and the time of update.
    void Record(float t_ms);

    //! Returns the recorded membrane potentials.
    std::unordered_map<std::string, std::vector<float>> GetRecording();
    
    //! Tells if the action potential threshold has been crossed.
    bool HasSpiked();

    //! Returns the time since the action potential threshold was
    //! crossed last.
    float DtAct_ms(float t_ms);

    //! Updates V_spike_t.
    float VSpikeT_mV(float t_ms);

    //! Updates V_AHP_t.
    float VAHPT_mV(float t_ms);

    //! Updates V_PSP_t.
    float VPSPT_mV(float t_ms);

    //! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
    //! Compares Vm with Vact.
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

}; // Close Namespace BallAndStick
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
