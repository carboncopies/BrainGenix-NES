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

    _RPCManager->AddRoute("Simulation/Neuron/BS/Create",              std::bind(&ModelRPCInterface::BSNeuronCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Compartments/BS/Create",        std::bind(&ModelRPCInterface::BSCreate, this, std::placeholders::_1));
   
    _RPCManager->AddRoute("Simulation/Neuron/SC/Create",              std::bind(&ModelRPCInterface::SCNeuronCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Compartments/SC/Create",        std::bind(&ModelRPCInterface::SCCreate, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/PatchClampDAC/Create",          std::bind(&ModelRPCInterface::PatchClampDACCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampDAC/SetOutputList",   std::bind(&ModelRPCInterface::PatchClampDACSetOutputList, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/Create",          std::bind(&ModelRPCInterface::PatchClampADCCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/SetSampleRate",   std::bind(&ModelRPCInterface::PatchClampADCSetSampleRate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/PatchClampADC/GetRecordedData", std::bind(&ModelRPCInterface::PatchClampADCGetRecordedData, this, std::placeholders::_1));

    _RPCManager->AddRoute("Simulation/SetSpecificAPTimes",            std::bind(&ModelRPCInterface::SetSpecificAPTimes, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/SetSpontaneousActivity",        std::bind(&ModelRPCInterface::SetSpontaneousActivity, this, std::placeholders::_1));


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
 * (*** TODO: This does not yet link to the shape of the receptor!!)
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
 */
std::string ModelRPCInterface::ReceptorCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Receptor/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Receptor Object
    Connections::Receptor C;
    if ((!Handle.GetParInt("SourceCompartmentID", C.SourceCompartmentID))
        || (!Handle.GetParInt("DestinationCompartmentID", C.DestinationCompartmentID))
        || (!Handle.GetParFloat("Conductance_nS", C.Conductance_nS))
        || (!Handle.GetParFloat("TimeConstantRise_ms", C.TimeConstantRise_ms))
        || (!Handle.GetParFloat("TimeConstantDecay_ms", C.TimeConstantDecay_ms))
        || (!Handle.GetParInt("ReceptorMorphology", C.ShapeID))
        //|| (!Handle.GetParVec3("ReceptorPos", C.ReceptorPos_um))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->Receptors.size();
    Handle.Sim()->Receptors.push_back(std::make_unique<Connections::Receptor>(C));

    // Inform destination neuron of its new input receptor.
    CoreStructs::Neuron* SrcNeuronPtr = Handle.Sim()->FindNeuronByCompartment(C.SourceCompartmentID);
    CoreStructs::Neuron* DstNeuronPtr = Handle.Sim()->FindNeuronByCompartment(C.DestinationCompartmentID);
    if ((SrcNeuronPtr==nullptr) || (DstNeuronPtr==nullptr)) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    CoreStructs::ReceptorData RData(C.ID, Handle.Sim()->Receptors.back().get(), SrcNeuronPtr);
    DstNeuronPtr->InputReceptorAdded(RData);

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

    // We cache the pointer to the shape in the compartment data, so that it
    // does not need to reach back to the Simulation to search for it.
    C.ShapePtr = Handle.Sim()->Collection.GetGeometry(C.ShapeID);
    if (!C.ShapePtr) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->BSCompartments.size();
    Handle.Sim()->BSCompartments.push_back(C);

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
    C.SomaCompartmentPtr = Handle.Sim()->FindCompartmentByID(C.SomaCompartmentID);
    C.AxonCompartmentPtr = Handle.Sim()->FindCompartmentByID(C.AxonCompartmentID);
    if ((!C.SomaCompartmentPtr) || (!C.AxonCompartmentPtr)) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->Neurons.size();
    
    Handle.Sim()->Neurons.push_back(std::make_shared<BallAndStick::BSNeuron>(C));
    Handle.Sim()->NeuronByCompartment.emplace(C.SomaCompartmentID, C.ID);
    Handle.Sim()->NeuronByCompartment.emplace(C.AxonCompartmentID, C.ID);

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

    // We cache the pointer to the shape in the compartment data, so that it
    // does not need to reach back to the Simulation to search for it.
    C.ShapePtr = Handle.Sim()->Collection.GetGeometry(C.ShapeID);
    if (!C.ShapePtr) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->BSCompartments.size();
    Handle.Sim()->BSCompartments.push_back(C);

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

    // // We cache the pointers to the compartments in the neuron data, so that it
    // // does not need to reach back to the Simulation to search for it.
    // C.SomaCompartmentPtr = Handle.Sim()->FindCompartmentByID(C.SomaCompartmentID);
    // C.AxonCompartmentPtr = Handle.Sim()->FindCompartmentByID(C.AxonCompartmentID);
    // if ((!C.SomaCompartmentPtr) || (!C.AxonCompartmentPtr)) {
    //     return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    // }

    C.ID = Handle.Sim()->Neurons.size();
    
    Handle.Sim()->Neurons.push_back(std::make_shared<SCNeuron>(C, *(Handle.Sim())));
    for (const auto & SomaID : C.SomaCompartmentIDs) {
        Handle.Sim()->NeuronByCompartment.emplace(SomaID, C.ID);
    }
    for (const auto & DendriteID : C.DendriteCompartmentIDs) {
        Handle.Sim()->NeuronByCompartment.emplace(DendriteID, C.ID);
    }
    for (const auto & AxonID : C.AxonCompartmentIDs) {
        Handle.Sim()->NeuronByCompartment.emplace(AxonID, C.ID);
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


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
