#include <Simulator/Manager.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/bgStatusCode.h>
#include <Simulator/Geometries/VecTools.h>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <cpp-base64/base64.h>

#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

// Handy class for standard handler data.
class HandlerData {
protected:
    Manager & Man;
    std::vector<std::unique_ptr<Simulation>>* SimVec = nullptr;

    nlohmann::json RequestJSON;
    API::bgStatusCode Status = API::bgStatusCode::bgStatusSuccess;

    int SimulationID = -1;
    Simulation * ThisSimulation = nullptr;

public:
    HandlerData(Manager* _Man, const std::string & _JSONRequest, const std::string & Source, bool PermitBusy = false): Man(*_Man) {
        SimVec = Man.GetSimulationVectorPtr();
        RequestJSON = nlohmann::json::parse(_JSONRequest);
        if (!GetParInt("SimulationID", SimulationID)) {
            return;
        }
        if (SimulationID >= SimVec->size() || SimulationID < 0) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return;
        }
        ThisSimulation = SimVec->at(SimulationID).get();
        if (PermitBusy) {
            return;
        }
        if (Man.IsSimulationBusy(ThisSimulation)) {
            Status = API::bgStatusCode::bgStatusSimulationBusy;
            return;
        }
        Man.Logger()->Log(Source+" called, on Sim " + SimIDStr(), 3);
    }

    bool HasError() const { return (Status != API::bgStatusCode::bgStatusSuccess); }

    std::string ErrResponse(int _Status) {
        Status = API::bgStatusCode(_Status);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = _Status;
        return ResponseJSON.dump();
    }
    std::string ErrResponse(API::bgStatusCode _Status) { return ErrResponse(int(_Status)); }
    std::string ErrResponse() { return ErrResponse(int(Status)); }
    std::string ResponseWithID(const std::string & IDName, int IDValue) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(Status);
        ResponseJSON[IDName] = IDValue;
        return ResponseJSON.dump();
    }
    std::string StringResponse(std::string _Key, std::string _Value) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(Status);
        ResponseJSON[_Key] = _Value;
        return ResponseJSON.dump();
    }

    int SimID() const { return SimulationID; }
    std::string SimIDStr() const { return std::to_string(SimulationID); }

    Simulation * Sim() const { return ThisSimulation; }

    const nlohmann::json & ReqJSON() const { return RequestJSON; }

    bool FindPar(const std::string & ParName, nlohmann::json::iterator & Iterator) {
        Iterator = RequestJSON.find(ParName);
        if (Iterator == RequestJSON.end()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        return true;
    }

    bool FindPar(const std::string & ParName, nlohmann::json::iterator & Iterator, nlohmann::json& _JSON) {
        Iterator = _JSON.find(ParName);
        if (Iterator == _JSON.end()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        return true;
    }

    bool GetParInt(const std::string & ParName, int & Value) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it)) {
            return false;
        }
        if (!it.value().is_number()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<int>();
        return true;
    }

    bool GetParInt(const std::string & ParName, int & Value, nlohmann::json& _JSON) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it, _JSON)) {
            return false;
        }
        if (!it.value().is_number()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<int>();
        return true;
    }

    bool GetParFloat(const std::string & ParName, float & Value) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it)) {
            return false;
        }
        if (!it.value().is_number()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<float>();
        return true;
    }

    bool GetParFloat(const std::string & ParName, float & Value, nlohmann::json& _JSON) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it, _JSON)) {
            return false;
        }
        if (!it.value().is_number()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<float>();
        return true;
    }


    bool GetParString(const std::string & ParName, std::string & Value) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it)) {
            return false;
        }
        if (!it.value().is_string()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<std::string>();
        return true;
    }

    bool GetParString(const std::string & ParName, std::string & Value, nlohmann::json& _JSON) {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it, _JSON)) {
            return false;
        }
        if (!it.value().is_string()) {
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return false;
        }
        Value = it.value().template get<std::string>();
        return true;
    }


    bool GetParVec3(const std::string & ParName, Geometries::Vec3D & Value, const std::string & Units = "um") {
        if (!GetParFloat(ParName+"X_"+Units, Value.x)) {
            return false;
        }
        if (!GetParFloat(ParName+"Y_"+Units, Value.y)) {
            return false;
        }
        if (!GetParFloat(ParName+"Z_"+Units, Value.z)) {
            return false;
        }
        return true;
    }

    bool GetParVec3FromJSON(const std::string & ParName, Geometries::Vec3D & Value, nlohmann::json& _JSON, const std::string & Units = "um") {
        if (!GetParFloat(ParName+"X_"+Units, Value.x, _JSON)) {
            return false;
        }
        if (!GetParFloat(ParName+"Y_"+Units, Value.y, _JSON)) {
            return false;
        }
        if (!GetParFloat(ParName+"Z_"+Units, Value.z, _JSON)) {
            return false;
        }
        return true;
    }

};

Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::Manager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);
    assert(_RPCManager != nullptr);
    assert(_RenderPool != nullptr);
    assert(_VisualizerPool != nullptr);


    Config_ = _Config;
    Logger_ = _Logger;
    RenderPool_ = _RenderPool;
    VisualizerPool_ = _VisualizerPool;



    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", Logger_, [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Simulation/Reset", Logger_, [this](std::string RequestJSON){ return SimulationReset(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RunFor", Logger_, [this](std::string RequestJSON){ return SimulationRunFor(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RecordAll", Logger_, [this](std::string RequestJSON){ return SimulationRecordAll(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetRecording", Logger_, [this](std::string RequestJSON){ return SimulationGetRecording(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetStatus", Logger_, [this](std::string RequestJSON){ return SimulationGetStatus(RequestJSON);});
    _RPCManager->AddRoute("Simulation/BuildMesh", Logger_, [this](std::string RequestJSON){ return SimulationBuildMesh(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/Create", Logger_, [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkSphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/Create", Logger_, [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkCylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/Create", Logger_, [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkBoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/Create", Logger_, [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkBSCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Staple/Create", Logger_, [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Receptor/Create", Logger_, [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});
    _RPCManager->AddRoute("Neuron/BSNeuron/Create", Logger_, [this](std::string RequestJSON){ return BSNeuronCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/Create", Logger_, [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/SetOutputList", Logger_, [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/Create", Logger_, [this](std::string RequestJSON){ return PatchClampADCCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/SetSampleRate", Logger_, [this](std::string RequestJSON){ return PatchClampADCSetSampleRate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/GetRecordedData", Logger_, [this](std::string RequestJSON){ return PatchClampADCGetRecordedData(RequestJSON);});
    _RPCManager->AddRoute("NES", Logger_, [this](std::string RequestJSON){ return NESRequest(RequestJSON);});


    _RPCManager->AddRoute("Debug", Logger_, [this](std::string RequestJSON){ return Debug(RequestJSON);});

    // Start SE Thread
    StopThreads_ = false;

}

Manager::~Manager() {

    std::cout<<"[Info] Signaling To Worker Threads To Stop\n";
    StopThreads_ = true;

    std::cout<<"[Info] Joining Simulation Worker Threads\n";
    for (unsigned int i = 0; i < SimulationThreads_.size(); i++) {
        SimulationThreads_[i].join();
    }
}

std::string Manager::SimulationCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    auto it = RequestJSON.find("Name");
    if (it == RequestJSON.end()) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(API::bgStatusCode::bgStatusInvalidParametersPassed);
        return ResponseJSON.dump();
    }
    if (!it.value().is_string()) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(API::bgStatusCode::bgStatusInvalidParametersPassed);
        return ResponseJSON.dump();
    }
    std::string SimulationName = it.value().template get<std::string>();
    Logger_->Log("Create Simulation Called, New Sim Has Name " + SimulationName, 3);

    // Build New Simulation Object
    Simulations_.push_back(std::make_unique<Simulation>(Logger_));
    int SimID = Simulations_.size()-1;
    Simulation* Sim = Simulations_[SimID].get();
    Sim->Name = SimulationName;
    Sim->ID = SimID;
    Sim->CurrentTask = SIMULATION_NONE;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, VisualizerPool_, &StopThreads_));

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["SimulationID"] = SimID;
    return ResponseJSON.dump();
}

std::string Manager::SimulationReset(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationReset");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Handle.Sim()->CurrentTask = SIMULATION_RESET; // request a reset be done
    // Handle.Sim()->WorkRequested = true;

    // Return Result ID
    return Handle.ErrResponse(); // ok
}


std::string Manager::SimulationRunFor(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationRunFor");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    float RunTime = -1.0;
    if (!Handle.GetParFloat("Runtime_ms", RunTime)) {
        return Handle.ErrResponse();
    }
    Handle.Sim()->RunTimes_ms = RunTime;
    Handle.Sim()->CurrentTask = SIMULATION_RUNFOR; // request work be done
    Handle.Sim()->WorkRequested = true;

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string Manager::SimulationRecordAll(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationRecordAll");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    float MaxRecordTime = -1.0;
    if (!Handle.GetParFloat("MaxRecordTime_ms", MaxRecordTime)) {
        return Handle.ErrResponse();
    }
    //Handle.Sim()->MaxRecordTime_ms = MaxRecordTime;
    Handle.Sim()->SetRecordAll(MaxRecordTime);

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string Manager::SimulationGetRecording(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationGetRecording");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Recording"] = Handle.Sim()->GetRecordingJSON(); //Handle.Sim()->RecordingBlob;
    return ResponseJSON.dump();
}

std::string Manager::SimulationGetStatus(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationGetStatus", true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["IsSimulating"] = (bool)(Handle.Sim()->IsProcessing || Handle.Sim()->WorkRequested || Handle.Sim()->IsRendering);
    ResponseJSON["RealWorldTimeRemaining_ms"] = 0.0;
    ResponseJSON["RealWorldTimeElapsed_ms"] = 0.0;
    ResponseJSON["InSimulationTime_ms"] = 0.0;
    ResponseJSON["InSimulationTimeRemaining_ms"] = 0.0;
    ResponseJSON["PercentComplete"] = 0.0;
    return ResponseJSON.dump();
}

std::string Manager::SimulationBuildMesh(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SimulationBuildMesh");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // if (!BuildMeshFromSimulation(Logger_, Renderer_, Handle.Sim())) {
    //     return Handle.ErrResponse(999); // general failure
    // }
    std::cout<<"WARNING THE SIMULATION BUILD MESH FUNCTION DOESNT DO AYNTHING!!!!\n";


    // Return Result ID
    return Handle.ErrResponse(); // ok
}


std::string Manager::SphereCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SphereCreate");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    
    // Build New Sphere Object
    Geometries::Sphere S;
    if ((!Handle.GetParFloat("Radius_um", S.Radius_um))
        || (!Handle.GetParVec3("CenterPos", S.Center_um))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size();
    Handle.Sim()->Collection.Geometries.push_back(S);
    //Handle.Sim()->Collection.append(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}

std::string Manager::CylinderCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "CylinderCreate");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Cylinder Object
    Geometries::Cylinder S;
    if ((!Handle.GetParFloat("Point1Radius_um", S.End0Radius_um))
        || (!Handle.GetParVec3("Point1Pos", S.End0Pos_um))
        || (!Handle.GetParFloat("Point2Radius_um", S.End1Radius_um))
        || (!Handle.GetParVec3("Point2Pos", S.End1Pos_um))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    Handle.Sim()->Collection.Geometries.push_back(S);
    //Handle.Sim()->Collection.append(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}

std::string Manager::BoxCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "BoxCreate");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Box Object
    Geometries::Box S;
    if ((!Handle.GetParVec3("CenterPos", S.Center_um))
        || (!Handle.GetParVec3("Scale", S.Dims_um))
        || (!Handle.GetParVec3("Rotation", S.Rotations_rad, "rad"))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    Handle.Sim()->Collection.Geometries.push_back(S);
    //Handle.Sim()->Collection.append(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}

std::string Manager::BSCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "BSCreate");
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

    C.ID = Handle.Sim()->BSCompartments.size();
    Handle.Sim()->BSCompartments.push_back(C);

    // Return Result ID
    return Handle.ResponseWithID("CompartmentID", C.ID);
}

std::string Manager::StapleCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "StapleCreate");
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

std::string Manager::ReceptorCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "ReceptorCreate");
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
        || (!Handle.GetParVec3("ReceptorPos", C.ReceptorPos_um))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->Receptors.size();
    Handle.Sim()->Receptors.push_back(C);

    // *** TODO: Are these receptor structs that are kept in the list here actually
    //           used anywhere? Do they create a functional receptor object? Are
    //           they checked during Neuron updates?
    //           This might be where a functional object has to be created.

    // Return Result ID
    return Handle.ResponseWithID("ReceptorID", C.ID);
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

std::string Manager::BSNeuronCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "BSNeuronCreate");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New BSNeuron Object
    CoreStructs::BSNeuronStruct C;
    if ((!Handle.GetParInt("SomaID", C.SomaShapeID))
        || (!Handle.GetParInt("AxonID", C.AxonShapeID))
        || (!Handle.GetParFloat("MembranePotential_mV", C.MembranePotential_mV))
        || (!Handle.GetParFloat("RestingPotential_mV", C.RestingPotential_mV))
        || (!Handle.GetParFloat("SpikeThreshold_mV", C.SpikeThreshold_mV))
        || (!Handle.GetParFloat("DecayTime_ms", C.DecayTime_ms))
        || (!Handle.GetParFloat("AfterHyperpolarizationAmplitude_mV", C.AfterHyperpolarizationAmplitude_mV))
        || (!Handle.GetParFloat("PostsynapticPotentialRiseTime_ms", C.PostsynapticPotentialRiseTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialDecayTime_ms", C.PostsynapticPotentialDecayTime_ms))
        || (!Handle.GetParFloat("PostsynapticPotentialAmplitude_mV", C.PostsynapticPotentialAmplitude_mV))
        || (!Handle.GetParString("Name", C.Name))) {
        return Handle.ErrResponse();
    }

    C.ID = Handle.Sim()->Neurons.size();
    // NOTE: At this time, we're keeping both a list of these data structs (in the Neurons list)
    //       and a list of functional neuron objects. For more info, see Log 202401121525.
    Geometries::Geometry * soma_ptr = Handle.Sim()->Collection.GetGeometry(C.SomaShapeID);
    Geometries::Geometry * axon_ptr = Handle.Sim()->Collection.GetGeometry(C.AxonShapeID);
    if ((!axon_ptr) || (!soma_ptr)) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }
    Handle.Sim()->Neurons.push_back(std::make_shared<BallAndStick::BSNeuron>(C, soma_ptr, axon_ptr));

    // Return Result ID
    return Handle.ResponseWithID("NeuronID", C.ID);
}

std::string Manager::PatchClampDACCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "PatchClampDACCreate");
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
std::string Manager::PatchClampDACSetOutputList(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "PatchClampDACSetOutputList");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdDACID
    int PatchClampDACID = -1;
    if (!Handle.GetParInt("PatchClampDACID", PatchClampDACID)) {
        return Handle.ErrResponse();
    }

    if (PatchClampDACID >= Handle.Sim()->PatchClampDACs.size() || PatchClampDACID < 0) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }
    Tools::PatchClampDAC* ThisDAC = &Handle.Sim()->PatchClampDACs[PatchClampDACID];

    // Set Params
    nlohmann::json::iterator ControlDataJSON_it;
    if (!Handle.FindPar("ControlData", ControlDataJSON_it)) {
        return Handle.ErrResponse();
    }

    ThisDAC->ControlData.clear();
    for (const auto & value_pair: ControlDataJSON_it.value()) {
        if (value_pair.size() < 2) {
            return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
        }
        if ((!value_pair[0].is_number()) || (!value_pair[1].is_number())) {
            return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
        }
        float t_ms = value_pair[0].template get<float>();
        float v_mV = value_pair[1].template get<float>();
        ThisDAC->ControlData.emplace_back(t_ms, v_mV);
    }

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string Manager::PatchClampADCCreate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "PatchClampADCCreate");
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

std::string Manager::PatchClampADCSetSampleRate(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "PatchClampADCSetSampleRate");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = -1;
    if (!Handle.GetParInt("PatchClampADCID", PatchClampADCID)) {
        return Handle.ErrResponse();
    }
    if (PatchClampADCID >= Handle.Sim()->PatchClampADCs.size() || PatchClampADCID < 0) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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

std::string Manager::PatchClampADCGetRecordedData(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "PatchClampADCGetRecordedData");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = -1;
    if (!Handle.GetParInt("PatchClampADCID", PatchClampADCID)) {
        return Handle.ErrResponse();
    }
    if (PatchClampADCID >= Handle.Sim()->PatchClampADCs.size() || PatchClampADCID < 0) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }
    Tools::PatchClampADC* ThisADC = &Handle.Sim()->PatchClampADCs[PatchClampADCID];
    
    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Timestep_ms"] = ThisADC->Timestep_ms;
    ResponseJSON["RecordedData_mV"] = ThisADC->RecordedData_mV;
    return ResponseJSON.dump();
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
std::string Manager::SetSpecificAPTimes(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "SetSpecificAPTimes");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Set Params
    nlohmann::json::iterator TimeNeuronPairJSON_it;
    if (!Handle.FindPar("TimeNeuronPairs", TimeNeuronPairJSON_it)) {
        return Handle.ErrResponse();
    }
    for (const auto & time_neuron_pair: TimeNeuronPairJSON_it.value()) {
        if (time_neuron_pair.size() < 2) {
            return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
        }
        if ((!time_neuron_pair[0].is_number()) || (!time_neuron_pair[1].is_number())) {
            return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
        }
        float t_ms = time_neuron_pair[0].template get<float>();
        unsigned int NeuronID = time_neuron_pair[1].template get<unsigned int>();
        if (NeuronID >= Handle.Sim()->Neurons.size()) {
            return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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
 *   "Locations": [
 *     {
 *          "CameraPositionX_um": float,
 *          "CameraPositionY_um": float,
 *          "CameraPositionz_um": float,
 *          "CameraLookAtPositionX_um": float,
 *          "CameraLookAtPositionY_um": float,
 *          "CameraLookAtPositionz_um": float,
 *          "CameraFOV_deg": float
 *      },
 *      ...
 *      ]
 *   "ImageWidth_px": unsigned int,
 *   "ImageHeight_px": unsigned int
 * }
 */
std::string Manager::VisualizerGenerateImage(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "VisualizerGenerateImage");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Create and Populate Parameters From Request
    nlohmann::json::iterator LocationIterator;
    if (!Handle.FindPar("Locations", LocationIterator)) {
        return Handle.ErrResponse();
    }

    for (auto& ThisLocation: LocationIterator.value()) {
        Geometries::Vec3D Position;
        Handle.GetParVec3FromJSON("CameraPosition", Position, ThisLocation);
        Handle.Sim()->VisualizerParams.CameraPositionList_um.push_back(vsg::dvec3(Position.x, Position.y, Position.z));

        Geometries::Vec3D LookAtPosition;
        Handle.GetParVec3FromJSON("CameraLookAtPosition", Position, ThisLocation);
        Handle.Sim()->VisualizerParams.CameraLookAtPositionList_um.push_back(vsg::dvec3(LookAtPosition.x, LookAtPosition.y, LookAtPosition.z));

        float FOV_deg;
        Handle.GetParFloat("CameraFOV_deg", FOV_deg, ThisLocation);
        Handle.Sim()->VisualizerParams.FOVList_deg.push_back(FOV_deg);
    }

    Handle.GetParInt("ImageWidth_px", Handle.Sim()->VisualizerParams.ImageWidth_px);
    Handle.GetParInt("ImageHeight_px", Handle.Sim()->VisualizerParams.ImageHeight_px);


    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->CurrentTask = SIMULATION_VISUALIZATION;
    Handle.Sim()->VisualizerParams.State = VISUALIZER_REQUESTED;
    Handle.Sim()->WorkRequested = true;


    // Return Result ID
    return Handle.ErrResponse(); // ok
}


/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 * }
 */
std::string Manager::VisualizerGetStatus(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "VisualizerGetStatus", true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return Result ID
    return Handle.ResponseWithID("VisualizerStatus", Handle.Sim()->VisualizerParams.State); // ok
}



/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 * }
 */
std::string Manager::VisualizerGetImageHandles(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "VisualizerGetImageHandles");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Return Result ID
    std::string ImageHandleStrings = nlohmann::json(Handle.Sim()->VisualizerParams.FileHandles).dump();
    return Handle.StringResponse("ImageHandles", ImageHandleStrings); // ok
}


/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "ImageHandle": <str>
 * }
 */
std::string Manager::VisualizerGetImage(std::string _JSONRequest) {

    HandlerData Handle(this, _JSONRequest, "VisualizerGetImage");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    std::string ImageHandle;
    Handle.GetParString("ImageHandle", ImageHandle);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Minor security feature (probably still exploitable, so be warned!)
    // We just remove .. from the incoming handle for the image, since they're just files right now
    // as such, if we didnt strip that, then people could read any files on the server!
    // Also, we prepend a '.' so people can't try and get to the root
    std::string Pattern = "..";
    std::string::size_type i = ImageHandle.find(Pattern);
    while (i != std::string::npos) {
        Logger_->Log("Detected '..' In ImageHandle, It's Possible That Someone Is Trying To Do Something Nasty", 8);
        ImageHandle.erase(i, Pattern.length());
        i = ImageHandle.find(Pattern, i);
    }
    std::string SafeHandle = "./" + ImageHandle;


    // Now Check If The Handle Is Valid, If So, Load It
    std::ifstream ImageStream(SafeHandle.c_str(), std::ios::binary);
    std::string RawData;
    if (ImageStream.good()) {
        std::stringstream Buffer;
        Buffer << ImageStream.rdbuf();
        RawData = Buffer.str();
        // ImageStream>>RawData;
        ImageStream.close();

    } else {
        Logger_->Log("An Invalid ImageHandle Was Provided " + SafeHandle, 6);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // error
        return ResponseJSON.dump();
    }

    // Now, Convert It To Base64
    std::string Base64Data = base64_encode(reinterpret_cast<const unsigned char*>(RawData.c_str()), RawData.length());


    // Return Result ID
    return Handle.StringResponse("ImageData", Base64Data); // ok
}







// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.

//typedef bool NESRequest_func_t(Simulation*, const nlohmann::json&);
typedef std::string NESRequest_func_t(Manager&, const nlohmann::json&);
typedef std::map<std::string, NESRequest_func_t*> NESRequest_map_t;

std::string SimulationCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationCreate(ReqParams.dump());
}
std::string SimulationResetHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationReset(ReqParams.dump());
}
std::string SimulationRunForHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationRunFor(ReqParams.dump());
}
std::string SimulationRecordAllHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationRecordAll(ReqParams.dump());
}
std::string SimulationGetRecordingHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationGetRecording(ReqParams.dump());
}
std::string SimulationGetStatusHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationGetStatus(ReqParams.dump());
}
std::string SimulationBuildMeshHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SimulationBuildMesh(ReqParams.dump());
}
std::string SphereCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SphereCreate(ReqParams.dump());
}
std::string CylinderCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.CylinderCreate(ReqParams.dump());
}
std::string BoxCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.BoxCreate(ReqParams.dump());
}
std::string BSCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.BSCreate(ReqParams.dump());
}
std::string StapleCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.StapleCreate(ReqParams.dump());
}
std::string ReceptorCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.ReceptorCreate(ReqParams.dump());
}
std::string BSNeuronCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.BSNeuronCreate(ReqParams.dump());
}
std::string PatchClampDACCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampDACCreate(ReqParams.dump());
}
std::string PatchClampADCCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCCreate(ReqParams.dump());
}
std::string PatchClampDACSetOutputListHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampDACSetOutputList(ReqParams.dump());
}
std::string PatchClampADCSetSampleRateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCSetSampleRate(ReqParams.dump());
}
std::string PatchClampADCGetRecordedDataHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCGetRecordedData(ReqParams.dump());
}
std::string SetSpecificAPTimesHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SetSpecificAPTimes(ReqParams.dump());
}
std::string VisualizerGenerateImage(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.VisualizerGenerateImage(ReqParams.dump());
}
std::string VisualizerGetImageHandles(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.VisualizerGetImageHandles(ReqParams.dump());
}
std::string VisualizerGetStatus(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.VisualizerGetStatus(ReqParams.dump());
}
std::string VisualizerGetImage(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.VisualizerGetImage(ReqParams.dump());
}


const NESRequest_map_t NES_Request_handlers = {
    {"SimulationCreate", SimulationCreateHandler},
    {"SimulationReset", SimulationResetHandler},
    {"SimulationRunFor", SimulationRunForHandler},
    {"SimulationRecordAll", SimulationRecordAllHandler},
    {"SimulationGetRecording", SimulationGetRecordingHandler},
    {"SimulationGetStatus", SimulationGetStatusHandler},
    {"SimulationBuildMesh", SimulationBuildMeshHandler},
    {"SphereCreate", SphereCreateHandler},
    {"CylinderCreate", CylinderCreateHandler},
    {"BoxCreate", BoxCreateHandler},
    {"BSCreate", BSCreateHandler},
    {"StapleCreate", StapleCreateHandler},
    {"ReceptorCreate", ReceptorCreateHandler},
    {"BSNeuronCreate", BSNeuronCreateHandler},
    {"PatchClampDACCreate", PatchClampDACCreateHandler},
    {"PatchClampADCCreate", PatchClampADCCreateHandler},
    {"PatchClampDACSetOutputList", PatchClampDACSetOutputListHandler},
    {"PatchClampADCSetSampleRate", PatchClampADCSetSampleRateHandler},
    {"PatchClampADCGetRecordedData", PatchClampADCGetRecordedDataHandler},
    {"SetSpecificAPTimes", SetSpecificAPTimesHandler},
    {"VisualizerGenerateImage", VisualizerGenerateImage},
    {"VisualizerGetImageHandles", VisualizerGetImageHandles},
    {"VisualizerGetImage", VisualizerGetImage},
    {"VisualizerGetStatus", VisualizerGetStatus}
};

bool Manager::BadReqID(int ReqID) {
    // *** TODO: Add some rules here for ReqIDs that should be refused.
    //           For example, keep track of the largest ReqID received
    //           and reject if ReqID is smaller than that. This is
    //           useful if ReqIDs are made using clock time or something.
    return false;
}

/**
 * This expects requests of the following format:
 * [
 *   {
 *     "ReqID": <request-id>,
 *     "AddBSNeuron": {
 *       "SimulationID": <SimID>, // Logically, this could belong outside AddBSNeuron, but we wish to reuse backward compatible functions. 
 *       "Name": <name>,
 *       "SomaID": <soma-id>,
 *       "AxonID": <axon-id>,
 *       <etc... all parameters>
 *     }
 *   },
 *   <more requests>
 * ]
 */
std::string Manager::NESRequest(std::string _JSONRequest) { // Generic JSON-based NES requests.

    // Parse Request
    Logger_->Log(_JSONRequest, 3);
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

    // Build Response
    nlohmann::json ResponseJSON = nlohmann::json::array(); // Create empty array for the list of responses.

    // For each request in the JSON list:
    for (const auto & req : RequestJSON) {

        int ReqID = -1;
        int SimulationID = -1;
        std::string ReqFunc;
        nlohmann::json ReqParams;
        nlohmann::json ReqResponseJSON;
        //std::string Response;

        // Get the mandatory components of a request:
        for (const auto & [req_key, req_value]: req.items()) {
            if (req_key == "ReqID") {
                ReqID = req_value.template get<int>();
            //} else if (req_key == "SimID") {
            //    SimulationID = req_value.template get<int>();
            } else {
                ReqFunc = req_key;
                ReqParams = req_value;
            }
        }

        if (BadReqID(ReqID)) { // e.g. < highest request ID already handled
            ReqResponseJSON["ReqID"] = ReqID;
            ReqResponseJSON["StatusCode"] = 1; // bad request id
        } else {

            // Typically would call a specific handler from here, but let's just keep parsing.
            auto it = NES_Request_handlers.find(ReqFunc);
            if (it == NES_Request_handlers.end()) {
                ReqResponseJSON["ReqID"] = ReqID;
                ReqResponseJSON["StatusCode"] = 1; // unknown request *** TODO: use the right code
                //Response = ReqResponseJSON.dump();
            } else {
                std::string Response = it->second(*this, ReqParams); // Calls the handler.
                // *** TODO: Either:
                //     a) Convert handlers to return nlohmann::json objects so that we
                //        can easily add ReqResponseJSON["ReqID"] = ReqID here, or,
                //     b) Convert Response back to a ReqResponseJSON here in order to
                //        add that... (This is more work, lower performance...)
                //     Right now, we do b) (sadly...)
                ReqResponseJSON = nlohmann::json::parse(Response);
                ReqResponseJSON["ReqID"] = ReqID;
            }

        }
        ResponseJSON.push_back(ReqResponseJSON);

    }

    std::string Response = ResponseJSON.dump();
    if (Response.length() < 1024) { 
        Logger_->Log("DEBUG --> Responding: " + Response, 0); // For DEBUG
    } else {
        Logger_->Log("DEBUG --> Response Omitted Due To Length", 0); // For DEBUG
    }
    return Response;
}

std::string Manager::Debug(std::string _JSONRequest) {

    // Parse Request
    std::cout<<_JSONRequest<<std::endl;
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

    // if (Util::GetString(&RequestJSON, "Action") == "Render") {

    // }


    std::cout<<"This will break if you don't have at least one simulation. Trying to get sim with id >0\n";

    Simulation* ThisSimulation = Simulations_[Simulations_.size()-1].get();


    BoundingBox BB = BoundingBox();
    BB.bb_point1[0] = -1.;
    BB.bb_point1[1] = -1.;
    BB.bb_point1[2] = -1.;

    BB.bb_point2[0] = 9.;
    BB.bb_point2[1] = 6.;
    BB.bb_point2[2] = 6.;

    
    float VoxelSize = atof(Util::GetString(&RequestJSON, "Query").c_str());
    VoxelArray Arr(BB, VoxelSize);


    MicroscopeParameters MParams;
    MParams.VoxelResolution_um = VoxelSize;


    CreateVoxelArrayFromSimulation(Logger_, ThisSimulation, &MParams, &Arr);

    // for (unsigned int i = 0; i < Arr.GetZ(); i++) {
    //     Arr.SetVoxel(i, i, i, FILLED);
    // }

    // Renderer_->ResetScene();


    // // RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, 10);
    // for (unsigned int i = 0; i < Arr.GetZ(); i++) {
    //     RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, i);
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay so our mutex is easier to get by another thread if needed
    // }

    // Renderer_->ResetViewer();



    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

bool Manager::IsSimulationBusy(Simulation* _Sim) {
    return _Sim->IsProcessing || _Sim->WorkRequested;
}

std::vector<std::unique_ptr<Simulation>>* Manager::GetSimulationVectorPtr() {
    return &Simulations_;
}

// ------------ Are we keeping the following "Bulk" routes? --------

std::string Manager::BulkSphereCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Sphere Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> RadiusList_um;
    std::vector<float> CenterXList_um;
    std::vector<float> CenterYList_um;
    std::vector<float> CenterZList_um;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&RadiusList_um, &RequestJSON, "RadiusList_um");
    Util::GetFloatVector(&CenterXList_um, &RequestJSON, "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &RequestJSON, "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &RequestJSON, "CenterZList_um");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = RadiusList_um.size();
    LengthsEqual &= (CenterXList_um.size() == Length);
    LengthsEqual &= (CenterYList_um.size() == Length);
    LengthsEqual &= (CenterZList_um.size() == Length);
    LengthsEqual &= (NameList.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Spheres", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Sphere Object
        Geometries::Sphere S;
        S.Name = NameList[i];
        S.Radius_um = RadiusList_um[i];
        S.Center_um.x = CenterXList_um[i];
        S.Center_um.y = CenterYList_um[i];
        S.Center_um.z = CenterZList_um[i];


        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int SphereID = ThisSimulation->Collection.Geometries.size();
        S.ID = SphereID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}

std::string Manager::BulkCylinderCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Cylinder Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> Point1RadiusList_um;
    std::vector<float> Point2RadiusList_um;
    std::vector<float> Point1XList_um;
    std::vector<float> Point1YList_um;
    std::vector<float> Point1ZList_um;
    std::vector<float> Point2XList_um;
    std::vector<float> Point2YList_um;
    std::vector<float> Point2ZList_um;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&Point1RadiusList_um, &RequestJSON, "Point1RadiusList_um");
    Util::GetFloatVector(&Point2RadiusList_um, &RequestJSON, "Point2RadiusList_um");
    Util::GetFloatVector(&Point1XList_um, &RequestJSON, "Point1XList_um");
    Util::GetFloatVector(&Point1YList_um, &RequestJSON, "Point1YList_um");
    Util::GetFloatVector(&Point1ZList_um, &RequestJSON, "Point1ZList_um");
    Util::GetFloatVector(&Point2XList_um, &RequestJSON, "Point2XList_um");
    Util::GetFloatVector(&Point2YList_um, &RequestJSON, "Point2YList_um");
    Util::GetFloatVector(&Point2ZList_um, &RequestJSON, "Point2ZList_um");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = Point1RadiusList_um.size();
    LengthsEqual &= (Point2RadiusList_um.size() == Length);
    LengthsEqual &= (Point1XList_um.size() == Length);
    LengthsEqual &= (Point1YList_um.size() == Length);
    LengthsEqual &= (Point1ZList_um.size() == Length);
    LengthsEqual &= (Point2XList_um.size() == Length);
    LengthsEqual &= (Point2YList_um.size() == Length);
    LengthsEqual &= (Point2ZList_um.size() == Length);
    LengthsEqual &= (NameList.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Cylinders", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Cylinder Object
        Geometries::Cylinder S;
        S.Name = NameList[i];
        S.End0Radius_um = Point1RadiusList_um[i];
        S.End1Radius_um = Point2RadiusList_um[i];
        S.End0Pos_um.x = Point1XList_um[i];
        S.End0Pos_um.y = Point1YList_um[i];
        S.End0Pos_um.z = Point1ZList_um[i];
        S.End1Pos_um.x = Point2XList_um[i];
        S.End1Pos_um.y = Point2YList_um[i];
        S.End1Pos_um.z = Point2ZList_um[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int CylinderID = ThisSimulation->Collection.Geometries.size();
        S.ID = CylinderID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}

std::string Manager::BulkBoxCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Box Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> CenterXList_um;
    std::vector<float> CenterYList_um;
    std::vector<float> CenterZList_um;
    std::vector<float> DimensionsXList_um;
    std::vector<float> DimensionsYList_um;
    std::vector<float> DimensionsZList_um;
    std::vector<float> RotationXList_rad;
    std::vector<float> RotationYList_rad;
    std::vector<float> RotationZList_rad;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&CenterXList_um, &RequestJSON, "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &RequestJSON, "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &RequestJSON, "CenterZList_um");
    Util::GetFloatVector(&DimensionsXList_um, &RequestJSON, "DimensionsXList_um");
    Util::GetFloatVector(&DimensionsYList_um, &RequestJSON, "DimensionsYList_um");
    Util::GetFloatVector(&DimensionsZList_um, &RequestJSON, "DimensionsZList_um");
    Util::GetFloatVector(&RotationXList_rad, &RequestJSON, "RotationXList_rad");
    Util::GetFloatVector(&RotationYList_rad, &RequestJSON, "RotationYList_rad");
    Util::GetFloatVector(&RotationZList_rad, &RequestJSON, "RotationZList_rad");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = NameList.size();
    LengthsEqual &= (CenterXList_um.size() == Length);
    LengthsEqual &= (CenterYList_um.size() == Length);
    LengthsEqual &= (CenterZList_um.size() == Length);
    LengthsEqual &= (DimensionsXList_um.size() == Length);
    LengthsEqual &= (DimensionsYList_um.size() == Length);
    LengthsEqual &= (DimensionsZList_um.size() == Length);
    LengthsEqual &= (RotationXList_rad.size() == Length);
    LengthsEqual &= (RotationYList_rad.size() == Length);
    LengthsEqual &= (RotationZList_rad.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Boxes", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Box Object
        Geometries::Box S;
        S.Name = NameList[i];
        S.Center_um.x = CenterXList_um[i];
        S.Center_um.y = CenterYList_um[i];
        S.Center_um.z = CenterZList_um[i];
        S.Dims_um.x = DimensionsXList_um[i];
        S.Dims_um.y = DimensionsYList_um[i];
        S.Dims_um.z = DimensionsZList_um[i];
        S.Rotations_rad.x = RotationXList_rad[i];
        S.Rotations_rad.y = RotationYList_rad[i];
        S.Rotations_rad.z = RotationZList_rad[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int BoxID = ThisSimulation->Collection.Geometries.size();
        S.ID = BoxID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}

std::string Manager::BulkBSCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create BS Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> MembranePotentialList_mV;
    std::vector<float> SpikeThresholdList_mV;
    std::vector<float> DecayTimeList_ms;
    std::vector<float> RestingPotentialList_mV;
    std::vector<float> AfterHyperpolarizationAmplitudeList_mV;
    std::vector<std::string> NameList;
    std::vector<int> ShapeIDList;
    Util::GetFloatVector(&MembranePotentialList_mV, &RequestJSON, "MembranePotentialList_mV");
    Util::GetFloatVector(&SpikeThresholdList_mV, &RequestJSON, "SpikeThresholdList_mV");
    Util::GetFloatVector(&DecayTimeList_ms, &RequestJSON, "DecayTimeList_ms");
    Util::GetFloatVector(&RestingPotentialList_mV, &RequestJSON, "RestingPotentialList_mV");
    Util::GetFloatVector(&AfterHyperpolarizationAmplitudeList_mV, &RequestJSON, "AfterHyperpolarizationAmplitudeList_mV");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");
    Util::GetIntVector(&ShapeIDList, &RequestJSON, "ShapeIDList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = NameList.size();
    LengthsEqual &= (ShapeIDList.size() == Length);
    LengthsEqual &= (MembranePotentialList_mV.size() == Length);
    LengthsEqual &= (SpikeThresholdList_mV.size() == Length);
    LengthsEqual &= (DecayTimeList_ms.size() == Length);
    LengthsEqual &= (RestingPotentialList_mV.size() == Length);
    LengthsEqual &= (AfterHyperpolarizationAmplitudeList_mV.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> CompartmentIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " BS Compartments", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Box Object
        Compartments::BS S;
        S.Name = NameList[i];
        S.AfterHyperpolarizationAmplitude_mV = AfterHyperpolarizationAmplitudeList_mV[i];
        S.DecayTime_ms = DecayTimeList_ms[i];
        S.ShapeID = ShapeIDList[i];
        S.MembranePotential_mV = MembranePotentialList_mV[i];
        S.RestingPotential_mV = RestingPotentialList_mV[i];
        S.SpikeThreshold_mV = SpikeThresholdList_mV[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int CompartmentID = ThisSimulation->BSCompartments.size();
        S.ID = CompartmentID;
        ThisSimulation->BSCompartments.push_back(S);


        CompartmentIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["CompartmentIDs"] = CompartmentIDs;
    return ResponseJSON.dump();
}

// -----------------------------------------------------------------

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
