#include <Simulator/RPC/ModelRPCInterface.h>
#include <Simulator/Distributions/Generic.h>
#include <RPC/APIStatusCode.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>


// Third-Party Libraries (BG convention: use <> instead of "")

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>



namespace BG {
namespace NES {
namespace Simulator {


ModelRPCInterface::ModelRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<Simulation>>* _Simulations, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;
    Simulations_ = _Simulations;

    // Register Callbacks
    _RPCManager->AddRoute("Simulation/Staple/Create",                 std::bind(&ModelRPCInterface::StapleCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Receptor/Create",               std::bind(&ModelRPCInterface::ReceptorCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/LIFCReceptor/Create",           std::bind(&ModelRPCInterface::LIFCReceptorCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/NetmorphLIFCReceptor/Create",   std::bind(&ModelRPCInterface::NetmorphLIFCReceptorCreate, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/Compartments/BS/Create",        std::bind(&ModelRPCInterface::BSCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Neuron/BS/Create",              std::bind(&ModelRPCInterface::BSNeuronCreate, this, std::placeholders::_1));
   
    _RPCManager->AddRoute("Simulation/Compartments/SC/Create",        std::bind(&ModelRPCInterface::SCCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Neuron/SC/Create",              std::bind(&ModelRPCInterface::SCNeuronCreate, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/Compartments/LIFC/Create",        std::bind(&ModelRPCInterface::LIFCCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Neuron/LIFC/Create",              std::bind(&ModelRPCInterface::LIFCNeuronCreate, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/PatchClampDAC/Create",          std::bind(&ModelRPCInterface::PatchClampDACCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampDAC/SetOutputList",   std::bind(&ModelRPCInterface::PatchClampDACSetOutputList, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/Create",          std::bind(&ModelRPCInterface::PatchClampADCCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/SetSampleRate",   std::bind(&ModelRPCInterface::PatchClampADCSetSampleRate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/GetRecordedData", std::bind(&ModelRPCInterface::PatchClampADCGetRecordedData, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/SetSpecificAPTimes",            std::bind(&ModelRPCInterface::SetSpecificAPTimes, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/SetSpontaneousActivity",        std::bind(&ModelRPCInterface::SetSpontaneousActivity, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/OptoModifyInjection",           std::bind(&ModelRPCInterface::OptoModifyInjection, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/OptoActivation",                std::bind(&ModelRPCInterface::OptoActivation, this, std::placeholders::_1));

}

ModelRPCInterface::~ModelRPCInterface() {

}

std::string ModelRPCInterface::StapleCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Staple/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Staple Object
    Connections::Staple C;
    if ((!Handle.GetParInt("SourceCompartmentID", C.SourceCompartmentID))
        || (!Handle.GetParInt("DestinationCompartmentID", C.DestinationCompartmentID))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->Staples.size();
    Handle.Sim()->Staples.push_back(C);

    // Return Result ID
    return Handle.ResponseWithID("StapleID", C.ID);
}

/**
 * This receptor create handler creates an object that contains information
 * about source and destination compartments and the physical location of
 * the receptor, as well as receptor paramters.
 * (*** TODO: This does not yet link to the shape of the receptor!! -- or does it now? See ReceptorMorphology.)
 * 
 * The receptor object is maintained in a list at the Simulation level,
 * and the target neuron is found (via the destination compartment), and
 * is informed of its new receptor.
 * 
 * How that is handled by the neuron depends on the neuron type.
 * In the case of the simplest BSNeuron, connections are treated as in
 * simple integrate and fire spiking neurons. The BSNeuron uses the
 * source neuron information (found via the source compartment) to
 * set up input connections to watch.
 * 
 * *** TODO:
 * 1. Add receptor type information to the Receptor class and receive that (stored in list of receptors).
 * 2. Add the DstNeuronPtr to RData.
 * 3. Connect RData with the SrcNeuronPtr as well.
 * 4. Update the SrcNeuron type by receptor type.
 */
std::string ModelRPCInterface::ReceptorCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Receptor/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Receptor Object
    Connections::Receptor C;
    std::string neurotransmitter_cache;
    if ((!Handle.GetParInt("SourceCompartmentID", C.SourceCompartmentID))
        || (!Handle.GetParInt("DestinationCompartmentID", C.DestinationCompartmentID))
        || (!Handle.GetParFloat("Conductance_nS", C.Conductance_nS))
        || (!Handle.GetParFloat("TimeConstantRise_ms", C.TimeConstantRise_ms))
        || (!Handle.GetParFloat("TimeConstantDecay_ms", C.TimeConstantDecay_ms))
        || (!Handle.GetParString("Neurotransmitter", neurotransmitter_cache))
        || (!Handle.GetParInt("ReceptorMorphology", C.ShapeID))
        //|| (!Handle.GetParVec3("ReceptorPos", C.ReceptorPos_um))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }
    C.safeset_Neurotransmitter(neurotransmitter_cache.c_str());

    C.ID = Handle.Sim()->AddReceptor(C);
    if (C.ID<0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("ReceptorID", C.ID);
}

const std::map<std::string, Connections::LIFCSTDPMethodEnum> LIFCSTDPMethodStrToEnum = {
    { "Hebbian", Connections::STDPHEBBIAN },
    { "Anti-Hebbian", Connections::STDPANTIHEBBIAN },
    { "None", Connections::STDPNONE }
};

Connections::LIFCSTDPMethodEnum LIFCSTDPMethod(const std::string& stdpmethod_cache) {
    auto it = LIFCSTDPMethodStrToEnum.find(stdpmethod_cache);
    if (it == LIFCSTDPMethodStrToEnum.end()) {
        return Connections::NUMLIFCSTDPMethodEnum;
    }
    return it->second;
}

const std::map<std::string, Connections::NeurotransmitterType> LIFCNeurotransmitterStrToEnum = {
    { "AMPA", Connections::AMPA },
    { "GABA", Connections::GABA },
    { "NMDA", Connections::NMDA }
};

Connections::NeurotransmitterType LIFCNeurotransmitter(const std::string& neurotransmitter_cache) {
    auto it = LIFCNeurotransmitterStrToEnum.find(neurotransmitter_cache);
    if (it == LIFCNeurotransmitterStrToEnum.end()) {
        return Connections::NUMNeurotransmitterType;
    }
    return it->second;
}

std::string ModelRPCInterface::LIFCReceptorCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/LIFCReceptor/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Receptor Object
    Connections::LIFCReceptor C;
    std::string neurotransmitter_cache;
    std::string stdpmethod_cache;
    if ((!Handle.GetParInt("SourceCompartmentID", C.SourceCompartmentID))
        || (!Handle.GetParInt("DestinationCompartmentID", C.DestinationCompartmentID))

        || (!Handle.GetParFloat("ReversalPotential_mV", C.ReversalPotential_mV))
        || (!Handle.GetParFloat("PSPRise_ms", C.PSPRise_ms))
        || (!Handle.GetParFloat("PSPDecay_ms", C.PSPDecay_ms))
        || (!Handle.GetParFloat("PeakConductance_nS", C.PeakConductance_nS))
        || (!Handle.GetParFloat("Weight", C.Weight))
        || (!Handle.GetParFloat("OnsetDelay_ms", C.OnsetDelay_ms))
        || (!Handle.GetParString("Neurotransmitter", neurotransmitter_cache))
        || (!Handle.GetParBool("voltage_gated", C.voltage_gated))

        || (!Handle.GetParString("STDP_Method", stdpmethod_cache))
        || (!Handle.GetParFloat("STDP_A_pos", C.STDP_A_pos))
        || (!Handle.GetParFloat("STDP_A_neg", C.STDP_A_neg))
        || (!Handle.GetParFloat("STDP_Tau_pos", C.STDP_Tau_pos))
        || (!Handle.GetParFloat("STDP_Tau_neg", C.STDP_Tau_neg))
        || (!Handle.GetParFloat("STDP_Shift", C.STDP_Shift))

        || (!Handle.GetParInt("ReceptorMorphology", C.ShapeID))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }
    C.Neurotransmitter = LIFCNeurotransmitter(neurotransmitter_cache);
    if (C.Neurotransmitter >= Connections::NUMNeurotransmitterType) {
        Logger_->Log("Error: Unrecognized LIFC Neurotransmitter type", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.STDP_Method = LIFCSTDPMethod(stdpmethod_cache);
    if (C.STDP_Method >= Connections::NUMLIFCSTDPMethodEnum) {
        Logger_->Log("Error: Unrecognized LIFC STDP Method", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->AddLIFCReceptor(C);
    if (C.ID<0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("ReceptorID", C.ID);
}

std::string ModelRPCInterface::NetmorphLIFCReceptorCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/NetmorphLIFCReceptor/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Receptor Object
    Connections::LIFCReceptor C;
    Connections::NetmorphLIFCReceptorRaw CDataRaw; // additional data
    std::string neurotransmitter_cache;
    std::string stdpmethod_cache;
    if ((!Handle.GetParInt("SourceCompartmentID", C.SourceCompartmentID))
        || (!Handle.GetParInt("DestinationCompartmentID", C.DestinationCompartmentID))

        || (!Handle.GetParFloat("ReversalPotential_mV", C.ReversalPotential_mV))
        || (!Handle.GetParFloat("PSPRise_ms", C.PSPRise_ms))
        || (!Handle.GetParFloat("PSPDecay_ms", C.PSPDecay_ms))

        || (!Handle.GetParFloat("ReceptorPeakConductance_nS", CDataRaw.ReceptorPeakConductance_nS))
        || (!Handle.GetParInt("ReceptorQuantity", CDataRaw.ReceptorQuantity))

        || (!Handle.GetParFloat("HillocDistance_um", CDataRaw.HillocDistance_um))
        || (!Handle.GetParFloat("Velocity_mps", CDataRaw.Velocity_mps))
        || (!Handle.GetParFloat("SynapticDelay_ms", CDataRaw.SynapticDelay_ms))
        || (!Handle.GetParString("Neurotransmitter", neurotransmitter_cache))
        || (!Handle.GetParBool("voltage_gated", C.voltage_gated))

        || (!Handle.GetParFloat("Weight", C.Weight))
        || (!Handle.GetParString("STDP_Method", stdpmethod_cache))
        || (!Handle.GetParFloat("STDP_A_pos", C.STDP_A_pos))
        || (!Handle.GetParFloat("STDP_A_neg", C.STDP_A_neg))
        || (!Handle.GetParFloat("STDP_Tau_pos", C.STDP_Tau_pos))
        || (!Handle.GetParFloat("STDP_Tau_neg", C.STDP_Tau_neg))
        || (!Handle.GetParFloat("STDP_Shift", C.STDP_Shift))

        || (!Handle.GetParInt("ReceptorMorphology", C.ShapeID))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }
    C.Neurotransmitter = LIFCNeurotransmitter(neurotransmitter_cache);
    if (C.Neurotransmitter >= Connections::NUMNeurotransmitterType) {
        Logger_->Log("Error: Unrecognized LIFC Neurotransmitter type", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.STDP_Method = LIFCSTDPMethod(stdpmethod_cache);
    if (C.STDP_Method >= Connections::NUMLIFCSTDPMethodEnum) {
        Logger_->Log("Error: Unrecognized LIFC STDP Method", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->AddNetmorphLIFCReceptor(C, CDataRaw);
    if (C.ID<0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("ReceptorID", C.ID);
}

/**
 * Creates a BS Compartment with form and function.
 * Form: A shape.
 * Function: Some parameters.
 */
std::string ModelRPCInterface::BSCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Compartments/BS/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New BS Object
    Compartments::BS C;
    if ((!Handle.GetParInt("ShapeID", C.ShapeID))
        || (!Handle.GetParFloat("MembranePotential_mV", C.MembranePotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("DecayTime_ms", C.DecayTime_ms))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->AddSCCompartment(C, BSNEURONS);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("CompartmentID", C.ID);
}

/*
As of 2024-01-12 the method to add a neuron that will be run in a
simulation is:

1. Create the soma and axon shapes for the neuron (remember their IDs).
2. Create a BSNeuron (which creates compartments using the shapes provided).
3. Add the BSNeuron to a NeuralCircuit.
OR
1. Create a NeuralCircuit.
2. Tell the NeuralCircuit to create a neuron.
*/
std::string ModelRPCInterface::BSNeuronCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Neuron/BS/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New BSNeuron Object
    CoreStructs::BSNeuronStruct C;
    if ((!Handle.GetParInt("SomaID", C.SomaCompartmentID))
        || (!Handle.GetParInt("AxonID", C.AxonCompartmentID))
        || (!Handle.GetParFloat("MembranePotential_mV", C.MembranePotential_mV))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("DecayTime_ms", C.DecayTime_ms))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParFloat("PostsynapticPotentialRiseTime_ms", C.PostsynapticPotentialRiseTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialDecayTime_ms", C.PostsynapticPotentialDecayTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialAmplitude_nA", C.PostsynapticPotentialAmplitude_nA))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    // We cache the pointers to the compartments in the neuron data, so that it
    // does not need to reach back to the Simulation to search for it.
    C.SomaCompartmentPtr = Handle.Sim()->FindBSCompartmentByID(C.SomaCompartmentID);
    if (!C.SomaCompartmentPtr) {
        Handle.Sim()->Logger_->Log("Soma compartment with ID "+std::to_string(C.SomaCompartmentID)+" not found", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    C.AxonCompartmentPtr = Handle.Sim()->FindBSCompartmentByID(C.AxonCompartmentID);
    if (!C.AxonCompartmentPtr) {
        Handle.Sim()->Logger_->Log("Axon compartment with ID "+std::to_string(C.AxonCompartmentID)+" not found", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->AddBSNeuron(C);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("NeuronID", C.ID);
}

/**
 * Creates a SC Compartment with form and function.
 * Form: A shape.
 * Function: Some parameters.
 */
std::string ModelRPCInterface::SCCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Compartments/SC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New SC Object
    Compartments::SC C;
    if ((!Handle.GetParInt("ShapeID", C.ShapeID))
        || (!Handle.GetParFloat("MembranePotential_mV", C.MembranePotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("DecayTime_ms", C.DecayTime_ms))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->AddSCCompartment(C, SCNEURONS);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("CompartmentID", C.ID);
}

std::string ModelRPCInterface::SCNeuronCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Neuron/SC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New SCNeuron Object
    CoreStructs::SCNeuronStruct C;
    if ((!Handle.GetParVecInt("SomaIDs", C.SomaCompartmentIDs))
        || (!Handle.GetParVecInt("DendriteIDs", C.DendriteCompartmentIDs))
        || (!Handle.GetParVecInt("AxonIDs", C.AxonCompartmentIDs))
        || (!Handle.GetParFloat("MembranePotential_mV", C.MembranePotential_mV))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("DecayTime_ms", C.DecayTime_ms))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParFloat("PostsynapticPotentialRiseTime_ms", C.PostsynapticPotentialRiseTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialDecayTime_ms", C.PostsynapticPotentialDecayTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialAmplitude_nA", C.PostsynapticPotentialAmplitude_nA))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->AddSCNeuron(C);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("NeuronID", C.ID);
}

/**
 * Creates a LIFC Compartment with form and function.
 * Form: A shape.
 * Function: Some parameters.
 */
std::string ModelRPCInterface::LIFCCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Compartments/LIFC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New LIFC Object
    Compartments::LIFC C;
    if ((!Handle.GetParInt("ShapeID", C.ShapeID))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("ResetPotential_mV", C.ResetPotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("MembraneResistance_MOhm", C.MembraneResistance_MOhm))
        || (!Handle.GetParFloat("MembraneCapacitance_pF", C.MembraneCapacitance_pF))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->AddLIFCCompartment(C);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("CompartmentID", C.ID);
}

const std::map<std::string, CoreStructs::LIFCUpdateMethodEnum> UpdateMethodStrToEnum = {
    { "ExpEulerCm", CoreStructs::EXPEULER_CM },
    { "ExpEulerRm", CoreStructs::EXPEULER_RM },
    { "ForwardEuler", CoreStructs::FORWARD_EULER },
    { "Classical", CoreStructs::CLASSICAL }
};

CoreStructs::LIFCUpdateMethodEnum LIFCUpdateMethod(const std::string& UpdateMethodStr) {
    auto it = UpdateMethodStrToEnum.find(UpdateMethodStr);
    if (it == UpdateMethodStrToEnum.end()) {
        return CoreStructs::NUMLIFCUpdateMethodEnum;
    }
    return it->second;
}

const std::map<std::string, CoreStructs::LIFCResetMethodEnum> ResetMethodStrToEnum = {
    { "ToVm", CoreStructs::TOVM },
    { "Onset", CoreStructs::ONSET },
    { "After", CoreStructs::AFTER }
};

CoreStructs::LIFCResetMethodEnum LIFCResetMethod(const std::string& ResetMethodStr) {
    auto it = ResetMethodStrToEnum.find(ResetMethodStr);
    if (it == ResetMethodStrToEnum.end()) {
        return CoreStructs::NUMLIFCResetMethodEnum;
    }
    return it->second;
}

const std::map<std::string, CoreStructs::LIFCAHPSaturationModelEnum> AHPSaturationModelStrToEnum = {
    { "clip", CoreStructs::AHPCLIP },
    { "sigmoid", CoreStructs::AHPSIGMOID }
};

CoreStructs::LIFCAHPSaturationModelEnum LIFCAHPSaturationModel(const std::string& AHPSaturationModelStr) {
    auto it = AHPSaturationModelStrToEnum.find(AHPSaturationModelStr);
    if (it == AHPSaturationModelStrToEnum.end()) {
        return CoreStructs::NUMLIFCAHPSaturationModelEnum;
    }
    return it->second;
}

const std::map<std::string, CoreStructs::LIFCADPSaturationModelEnum> ADPSaturationModelStrToEnum = {
    { "clip", CoreStructs::ADPCLIP },
    { "resource", CoreStructs::ADPRESOURCE }
};

CoreStructs::LIFCADPSaturationModelEnum LIFCADPSaturationModel(const std::string& ADPSaturationModelStr) {
    auto it = ADPSaturationModelStrToEnum.find(ADPSaturationModelStr);
    if (it == ADPSaturationModelStrToEnum.end()) {
        return CoreStructs::NUMLIFCADPSaturationModelEnum;
    }
    return it->second;
}

std::string ModelRPCInterface::LIFCNeuronCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Neuron/LIFC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New LIFCNeuron Object
    CoreStructs::LIFCNeuronStruct C;
    std::string UpdateMethodStr;
    std::string ResetMethodStr;
    std::string AHPSaturationModelStr;
    std::string ADPSaturationModelStr;
    if ((!Handle.GetParVecInt("SomaIDs", C.SomaCompartmentIDs))
        || (!Handle.GetParVecInt("DendriteIDs", C.DendriteCompartmentIDs))
        || (!Handle.GetParVecInt("AxonIDs", C.AxonCompartmentIDs))

        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("ResetPotential_mV", C.ResetPotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("MembraneResistance_MOhm", C.MembraneResistance_MOhm))
        || (!Handle.GetParFloat("MembraneCapacitance_pF", C.MembraneCapacitance_pF))
        || (!Handle.GetParFloat("RefractoryPeriod_ms", C.RefractoryPeriod_ms))
        || (!Handle.GetParFloat("SpikeDepolarization_mV", C.SpikeDepolarization_mV))

        || (!Handle.GetParString("UpdateMethod", UpdateMethodStr))
        || (!Handle.GetParString("ResetMethod", ResetMethodStr))

        || (!Handle.GetParFloat("AfterHyperpolarizationReversalPotential_mV", C.AfterHyperpolarizationReversalPotential_mV))

        || (!Handle.GetParFloat("FastAfterHyperpolarizationRise_ms", C.FastAfterHyperpolarizationRise_ms))
        || (!Handle.GetParFloat("FastAfterHyperpolarizationDecay_ms", C.FastAfterHyperpolarizationDecay_ms))
        || (!Handle.GetParFloat("FastAfterHyperpolarizationPeakConductance_nS", C.FastAfterHyperpolarizationPeakConductance_nS))
        || (!Handle.GetParFloat("FastAfterHyperpolarizationMaxPeakConductance_nS", C.FastAfterHyperpolarizationMaxPeakConductance_nS))
        || (!Handle.GetParFloat("FastAfterHyperpolarizationHalfActConstant", C.FastAfterHyperpolarizationHalfActConstant))

        || (!Handle.GetParFloat("SlowAfterHyperpolarizationRise_ms", C.SlowAfterHyperpolarizationRise_ms))
        || (!Handle.GetParFloat("SlowAfterHyperpolarizationDecay_ms", C.SlowAfterHyperpolarizationDecay_ms))
        || (!Handle.GetParFloat("SlowAfterHyperpolarizationPeakConductance_nS", C.SlowAfterHyperpolarizationPeakConductance_nS))
        || (!Handle.GetParFloat("SlowAfterHyperpolarizationMaxPeakConductance_nS", C.SlowAfterHyperpolarizationMaxPeakConductance_nS))
        || (!Handle.GetParFloat("SlowAfterHyperpolarizationHalfActConstant", C.SlowAfterHyperpolarizationHalfActConstant))

        || (!Handle.GetParString("AfterHyperpolarizationSaturationModel", AHPSaturationModelStr))

        || (!Handle.GetParFloat("FatigueThreshold", C.FatigueThreshold))
        || (!Handle.GetParFloat("FatigueRecoveryTime_ms", C.FatigueRecoveryTime_ms))

        || (!Handle.GetParFloat("AfterDepolarizationReversalPotential_mV", C.AfterDepolarizationReversalPotential_mV))
        || (!Handle.GetParFloat("AfterDepolarizationRise_ms", C.AfterDepolarizationRise_ms))
        || (!Handle.GetParFloat("AfterDepolarizationDecay_ms", C.AfterDepolarizationDecay_ms))
        || (!Handle.GetParFloat("AfterDepolarizationPeakConductance_nS", C.AfterDepolarizationPeakConductance_nS))
        || (!Handle.GetParFloat("AfterDepolarizationSaturationMultiplier", C.AfterDepolarizationSaturationMultiplier))
        || (!Handle.GetParFloat("AfterDepolarizationRecoveryTime_ms", C.AfterDepolarizationRecoveryTime_ms))
        || (!Handle.GetParFloat("AfterDepolarizationDepletion", C.AfterDepolarizationDepletion))
        || (!Handle.GetParString("AfterDepolarizationSaturationModel", ADPSaturationModelStr))

        || (!Handle.GetParFloat("AdaptiveThresholdDiffPerSpike", C.AdaptiveThresholdDiffPerSpike))
        || (!Handle.GetParFloat("AdaptiveTresholdRecoveryTime_ms", C.AdaptiveTresholdRecoveryTime_ms))
        || (!Handle.GetParFloat("AdaptiveThresholdDiffPotential_mV", C.AdaptiveThresholdDiffPotential_mV))
        || (!Handle.GetParFloat("AdaptiveThresholdFloor_mV", C.AdaptiveThresholdFloor_mV))
        || (!Handle.GetParFloat("AdaptiveThresholdFloorDeltaPerSpike_mV", C.AdaptiveThresholdFloorDeltaPerSpike_mV))
        || (!Handle.GetParFloat("AdaptiveThresholdFloorRecoveryTime_ms", C.AdaptiveThresholdFloorRecoveryTime_ms))

        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.UpdateMethod = LIFCUpdateMethod(UpdateMethodStr);
    if (C.UpdateMethod >= CoreStructs::NUMLIFCUpdateMethodEnum) {
        Logger_->Log("Error: Unrecognized LIFC UpdateMethod", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    C.ResetMethod = LIFCResetMethod(ResetMethodStr);
    if (C.ResetMethod >= CoreStructs::NUMLIFCResetMethodEnum) {
        Logger_->Log("Error: Unrecognized LIFC ResetMethod", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    C.AfterHyperpolarizationSaturationModel = LIFCAHPSaturationModel(AHPSaturationModelStr);
    if (C.AfterHyperpolarizationSaturationModel >= CoreStructs::NUMLIFCAHPSaturationModelEnum) {
        Logger_->Log("Error: Unrecognized LIFC AHP Saturation Model", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    C.AfterDepolarizationSaturationModel = LIFCADPSaturationModel(ADPSaturationModelStr);
    if (C.AfterDepolarizationSaturationModel >= CoreStructs::NUMLIFCADPSaturationModelEnum) {
        Logger_->Log("Error: Unrecognized LIFC ADP Saturation Model", 7);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->AddLIFCNeuron(C);
    if (C.ID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    // Return Result ID
    return Handle.ResponseWithID("NeuronID", C.ID);
}

std::string ModelRPCInterface::PatchClampDACCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/OatchClampDAC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New DAC Object
    Tools::PatchClampDAC T;
    if ((!Handle.GetParInt("DestinationCompartmentID", T.DestinationCompartmentID))
        || (!Handle.GetParVec3("ClampPos", T.ClampPos_um))
        || (!Handle.GetParString("Name", T.Name))) {
        return Handle.ErrResponse();
    }

    T.ID = Handle.Sim()->PatchClampDACs.size();
    Handle.Sim()->PatchClampDACs.push_back(T);


    // Return Result ID
    return Handle.ResponseWithID("PatchClampDACID", T.ID);
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "PatchClampDACID": <DAC-ID>,
 *   "ControlData": [
 *      [ <t_ms>, <v_mV> ],
 *      (more pairs)
 *   ]
 * }
 */
std::string ModelRPCInterface::PatchClampDACSetOutputList(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/PatchClampDAC/SetOutputList", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdDACID
    int PatchClampDACID = -1;
    if (!Handle.GetParInt("PatchClampDACID", PatchClampDACID)) {
        return Handle.ErrResponse();
    }

    if (PatchClampDACID >= Handle.Sim()->PatchClampDACs.size() || PatchClampDACID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    Tools::PatchClampDAC* ThisDAC = &Handle.Sim()->PatchClampDACs[PatchClampDACID];

    // Set Params
    nlohmann::json::iterator ControlDataJSON_it;
    if (!Handle.FindPar("ControlData", ControlDataJSON_it)) {
        return Handle.ErrResponse();
    }

    ThisDAC->ControlData.clear();
    for (const auto& value_pair: ControlDataJSON_it.value()) {
        if (value_pair.size() < 2) {
            return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        if ((!value_pair[0].is_number()) || (!value_pair[1].is_number())) {
            return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        float t_ms = value_pair[0].template get<float>();
        float v_mV = value_pair[1].template get<float>();
        ThisDAC->ControlData.emplace_back(t_ms, v_mV);
    }

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string ModelRPCInterface::PatchClampADCCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/PatchClampADC/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New ADC Object
    Tools::PatchClampADC T;
    if ((!Handle.GetParInt("SourceCompartmentID", T.SourceCompartmentID))
        || (!Handle.GetParVec3("ClampPos", T.ClampPos_um))
        || (!Handle.GetParString("Name", T.Name))) {
        return Handle.ErrResponse();
    }
    T.Timestep_ms = 0.0f;

    T.ID = Handle.Sim()->PatchClampADCs.size();
    Handle.Sim()->PatchClampADCs.push_back(T);

    // Return Result ID
    return Handle.ResponseWithID("PatchClampADCID", T.ID);
}

std::string ModelRPCInterface::PatchClampADCSetSampleRate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/PatchClampADC/SetSampleRate", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = -1;
    if (!Handle.GetParInt("PatchClampADCID", PatchClampADCID)) {
        return Handle.ErrResponse();
    }
    if (PatchClampADCID >= Handle.Sim()->PatchClampADCs.size() || PatchClampADCID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    Tools::PatchClampADC* ThisADC = &Handle.Sim()->PatchClampADCs[PatchClampADCID];
    
    // Set Params
    if (!Handle.GetParFloat("Timestep_ms", ThisADC->Timestep_ms)) {
        return Handle.ErrResponse();
    }
    ThisADC->RecordedData_mV.clear(); // clear recorded data as it is now invalid (the timestep is not the same anymore)

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string ModelRPCInterface::PatchClampADCGetRecordedData(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/PatchClampADC/GetRecordedData", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = -1;
    if (!Handle.GetParInt("PatchClampADCID", PatchClampADCID)) {
        return Handle.ErrResponse();
    }
    if (PatchClampADCID >= Handle.Sim()->PatchClampADCs.size() || PatchClampADCID < 0) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    Tools::PatchClampADC* ThisADC = &Handle.Sim()->PatchClampADCs[PatchClampADCID];
    
    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Timestep_ms"] = ThisADC->Timestep_ms;
    ResponseJSON["RecordedData_mV"] = ThisADC->RecordedData_mV;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "TimeNeuronPairs": [
 *      [ <t_ms>, <neuron-id> ],
 *      (more pairs)
 *   ]
 * }
 */
std::string ModelRPCInterface::SetSpecificAPTimes(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/SetSpecificAPTimes", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Set Params
    nlohmann::json::iterator TimeNeuronPairJSON_it;
    if (!Handle.FindPar("TimeNeuronPairs", TimeNeuronPairJSON_it)) {
        return Handle.ErrResponse();
    }
    for (const auto& time_neuron_pair: TimeNeuronPairJSON_it.value()) {
        if (time_neuron_pair.size() < 2) {
            return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        if ((!time_neuron_pair[0].is_number()) || (!time_neuron_pair[1].is_number())) {
            return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        float t_ms = time_neuron_pair[0].template get<float>();
        unsigned int NeuronID = time_neuron_pair[1].template get<unsigned int>();
        if (NeuronID >= Handle.Sim()->Neurons.size()) {
            return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }

        Handle.Sim()->Neurons.at(NeuronID)->AddSpecificAPTime(t_ms);
    }

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "SpikeIntervalMean_ms": <float>,
 *   "SpikeIntervalStDev_ms": <float>,
 *   "NeuronIDs": [
 *      (neuron-id,)
 *   ]
 * }
 * Notes:
 * - Zero neuron_ids means all Neurons.
 * - SpikeIntervalStDev_ms 0 means no spontaneous activity.
 * Responds:
 * {
 *   "StatusCode": <status-code>,
 * }
 */
std::string ModelRPCInterface::SetSpontaneousActivity(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/SetSpontaneousActivity", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Set Params
    float SpikeIntervalMean_ms = -1.0;
    if (!Handle.GetParFloat("SpikeIntervalMean_ms", SpikeIntervalMean_ms)) {
        return Handle.ErrResponse();
    }
    float SpikeIntervalStDev_ms = -1.0;
    if (!Handle.GetParFloat("SpikeIntervalStDev_ms", SpikeIntervalStDev_ms)) {
        return Handle.ErrResponse();
    }
    if ((SpikeIntervalMean_ms <= 0.0) || (SpikeIntervalStDev_ms < 0.0)) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    std::vector<int> NeuronIDs; // Empty list means all neurons.
    if (!Handle.GetParVecInt("NeuronIDs", NeuronIDs)) {
        return Handle.ErrResponse();
    }

    // Modify spontaneous activity settings of specified neurons
    if (NeuronIDs.empty()) {
        for (auto& neuron : Handle.Sim()->Neurons) {
            neuron->SetSpontaneousActivity(SpikeIntervalMean_ms, SpikeIntervalStDev_ms, Handle.Sim()->MasterRandom_->UniformRandomInt());
        }
    } else {
        for (const auto& neuron_id : NeuronIDs) {
            Handle.Sim()->Neurons.at(neuron_id)->SetSpontaneousActivity(SpikeIntervalMean_ms, SpikeIntervalStDev_ms, Handle.Sim()->MasterRandom_->UniformRandomInt());
        }
    }

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string ModelRPCInterface::OptoModifyInjection(std::string _JSONRequest) {
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/OptoModifyInjection", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    Geometries::Vec3D inject_pos;
    if (!Handle.GetParVec3("InjectLocation", inject_pos)) {
        return Handle.ErrResponse();
    }

    float radius = -1.f;
    if (!Handle.GetParFloat("Radius", radius)) {
        return Handle.ErrResponse();
    }

    int desired_wavelength = 0;
    if (!Handle.GetParInt("Wavelength", desired_wavelength)) {
        return Handle.ErrResponse();
    }

    int type = CoreStructs::UnknownNeuron;
    if (!Handle.GetParInt("Type", type)) {
        return Handle.ErrResponse();
    }

    for (auto& neuron : Handle.Sim()->Neurons) {
        if (neuron->Type_ != type) {
            continue;
        }

        if (neuron->GetCellCenter().Distance(inject_pos) <= radius) {
            neuron->opto_wavelength_nm = desired_wavelength;
        }
    }

    return Handle.ErrResponse(); // ok
}

std::string ModelRPCInterface::OptoActivation(std::string _JSONRequest) {
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/OptoActivation", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    std::vector<float> laser_activation_times;
    if (!Handle.GetParVecFloat("LaserActivationTimes", laser_activation_times)) {
        return Handle.ErrResponse();
    }

    int target_wavelength = 0;
    if (!Handle.GetParInt("TargetWavelength", target_wavelength)) {
        return Handle.ErrResponse();
    }

    for (float time : laser_activation_times) {
        for (auto& neuron : Handle.Sim()->Neurons) {
            if (neuron->opto_wavelength_nm == target_wavelength) {
                neuron->AddSpecificAPTime(time);
            }
        }
    }

    return Handle.ErrResponse(); // ok
}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
