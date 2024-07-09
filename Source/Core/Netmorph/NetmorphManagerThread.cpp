//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <cstddef>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Include/Netmorph.h>
#include <Include/Embeddable.h>
#include <network.hh>

// Internal Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphManagerThread.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Structs/BS.h>


namespace BG {
namespace NES {
namespace Simulator {

class Nm2NESLogging: public Netmorph2NESLogging {
protected:
    BG::Common::Logger::LoggingSystem& logger;
public:
    Nm2NESLogging(BG::Common::Logger::LoggingSystem& _logger): logger(_logger) {}

    virtual void error(const std::string & msg) {
        logger.Log(msg, 7);
    }
    virtual void warning(const std::string & msg) {
        logger.Log(msg, 6);
    }
    virtual void report(const std::string & msg) {
        logger.Log(msg, 3);
    }
    virtual void progress(const std::string & msg) {
        logger.Log(msg, 5);
    }
};

// --- The following may be moved
// The following macros are used for easy access to linked lists built
// in the manner of 2009 Netmorph.
// #define PLL_LOOP_FORWARD(PLLType,Init_El,Extra_Test) \
//     for (PLLType * e = Init_El; ((e) && (Extra_Test)); e = e->Next())
// #define PLL_LOOP_FORWARD_NESTED(PLLType,Init_El,Extra_Test,RefVar) \
//     for (PLLType * RefVar = Init_El; ((RefVar) && (Extra_Test)); RefVar = RefVar->Next())

// If NetmorphParams.Status==true then the NetmorphParams.net unique_ptr now owns the
// generated Netmorph network object. We can now use this to build our model in the
// simulation referenced by Handle.Sim().
bool BuildFromNetmorphNetwork(NetmorphParameters& _Params) {
    assert(_Params.Sim != nullptr);

    // *** We may want to obtain these from somewhere else:
    float neuron_Vm_mV = -60.0;
    float neuron_Vrest_mV = -60.0;
    float neuron_Vact_mV = -50.0;
    float neuron_Vahp_mV = -20.0;
    float neuron_tau_AHP_ms = 30.0;
    float neuron_tau_PSPr = 5.0;
    float neuron_tau_PSPd = 25.0;
    float neuron_IPSP = 870.0 // nA

    size_t nnum = 0;
    PLL_LOOP_FORWARD(neuron, PLLRoot<neuron>::head(), 1) {
        // 1. Build soma spheres.
        Geometries::Sphere S;
        S.Radius_um = e->Radius();
        S.Center_um.x = e->P.x;
        S.Center_um.y = e->P.y;
        S.Center_um.z = e->P.z;
        S.Name = "sphere-"+std::to_string(uint64_t(e));
        _Params.Sim->AddSphere(S);

        // 2. Build soma compartments.
        Compartments::BS C;
        C.ShapeID = S.ID;
        C.MembranePotential_mV = neuron_Vm_mV;
        C.RestingPotential_mV = neuron_Vrest_mV;
        C.SpikeThreshold_mV = neuron_Vact_mV;
        C.AfterHyperpolarizationAmplitude_mV = neuron_Vahp_mV;
        C.DecayTime_ms = neuron_tau_AHP_ms;
        C.Name = "compartment-"+std::to_string(uint64_t(e));
        if (_Params.Sim->AddSCCompartment(C)<0) {
            _Params.Sim->Logger_->Log("BuildFromNetmorphNetwork failed: Missing Sphere for soma compartment build.", 7);
            return false;
        }

        nnum++;
    }

}
// ---
    

int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params) {

    // Create link to redirect messaging from Netmorph to the NES Logger.
    embedlog = std::make_unique<Nm2NESLogging>(*_Logger);

    _Logger->Log("Starting Netmorph Simulation", 5);
    _Params->Result = Netmorph(&_Params->Progress_percent, _Params->ModelFile);
    _Logger->Log("Netmorph Simulation Finished", 5);

    _Params->State = Netmorph_DONE;

    return 0;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
