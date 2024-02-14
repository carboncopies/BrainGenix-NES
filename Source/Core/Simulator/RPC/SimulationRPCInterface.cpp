#include <Simulator/RPC/SimulationRPCInterface.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <RPC/APIStatusCode.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <cpp-base64/base64.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

namespace BG {
namespace NES {
namespace Simulator {







Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager) {
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
    // _RPCManager->AddRoute(NES_Request_handlers.at("SphereCreate").Route, Logger_, [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("BulkSphereCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkSphereCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("CylinderCreate").Route, Logger_, [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("BulkCylinderCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkCylinderCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("BoxCreate").Route, Logger_, [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("BulkBoxCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkBoxCreate(RequestJSON);});

    // _RPCManager->AddRoute(NES_Request_handlers.at("BSCreate").Route, Logger_, [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("BulkBSCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkBSCreate(RequestJSON);});

    // _RPCManager->AddRoute(NES_Request_handlers.at("StapleCreate").Route, Logger_, [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("ReceptorCreate").Route, Logger_, [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});

    // _RPCManager->AddRoute(NES_Request_handlers.at("BSNeuronCreate").Route, Logger_, [this](std::string RequestJSON){ return BSNeuronCreate(RequestJSON);});

    // _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampDACCreate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampDACSetOutputList").Route, Logger_, [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});

    // _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCCreate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCCreate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCSetSampleRate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCSetSampleRate(RequestJSON);});
    // _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCGetRecordedData").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCGetRecordedData(RequestJSON);});


    // _RPCManager->AddRoute(NES_Request_handlers.at("ManTaskStatus").Route, Logger_, [this](std::string RequestJSON){ return ManTaskStatus(RequestJSON);});


    _RPCManager->AddRoute("Simulation/Create",       std::bind(&Manager::SimulationCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Reset",        std::bind(&Manager::SimulationReset, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/RunFor",       std::bind(&Manager::SimulationRunFor, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/RecordAll",    std::bind(&Manager::SimulationRecordAll, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetRecording", std::bind(&Manager::SimulationGetRecording, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetStatus",    std::bind(&Manager::SimulationGetStatus, this, std::placeholders::_1));
    // _RPCManager->AddRoute("SimulationSave",         std::bind(&Manager::SimulationSave, this, std::placeholders::_1));
    // _RPCManager->AddRoute("SimulationLoad",         std::bind(&Manager::SimulationLoad, this, std::placeholders::_1));

    // Start SE Thread
    StopThreads_ = false;

}

Manager::~Manager() {

    Logger_->Log("Signaling To Worker Threads To Stop", 1);
    StopThreads_ = true;

    Logger_->Log("Joining Simulation Worker Threads", 2);
    for (unsigned int i = 0; i < SimulationThreads_.size(); i++) {
        SimulationThreads_[i].join();
    }
}

bool LoadFileIntoString(const std::string& FilePath, std::string& FileContents) {
    std::ifstream LoadFile(FilePath);
    if (!LoadFile.is_open()) return false;

    // Reserve all the necessary space in the string up-front.
    LoadFile.seekg(0, std::ios::end);   
    FileContents.reserve(LoadFile.tellg());
    LoadFile.seekg(0, std::ios::beg);

    FileContents.assign((std::istreambuf_iterator<char>(LoadFile)), std::istreambuf_iterator<char>());
    return true;    
}

// // A API::ManagerTaskData struct must have been prepared and the thread already launched.
// int Manager::AddManagerTask(std::unique_ptr<API::ManagerTaskData>& TaskData) {
//     // Get Task ID
//     int TaskID = NextManTaskID;
//     NextManTaskID++;
//     TaskData->ID = TaskID;

//     // Place task data into ManagerTasks
//     ManagerTasks[TaskID].reset(TaskData.release()); // Release task data object pointer into unique pointer in map.

//     return TaskID;
// }

// *** WE CAN PROBABLY DO WITHOUT THIS CONSTRAINT NOW (AS WE USE LOCAL PARAMS)!
// Only one thread at a time, others wait for lock release.
// void Manager::LoadingSimSetter(bool SetTo) {
//     std::lock_guard<std::mutex> guard(LoadingSimSetterMutex);
//     LoadingSim = SetTo;
// }
//
// We can run only one loading task at a time, because we are using some Manager-global
// flags and variables to modify the behavior of NESRequest and HandleData, namely to
// establish the new Simulation ID and to replace loaded SimIDs with that.
// Before proceeding, we wait for other loading tasks in the queue to finish.
// void Manager::SimLoadingTask(API::ManagerTaskData& TaskData) {
//     // Wait for any concurrent loading tasks that happen to be running to finish:
//     // unsigned long timeout_ms = 10000;
//     while (LoadingSim) {
//         timeout_ms--;
//         if (timeout_ms==0) {
//             Logger_->Log("SimLoadingTask timed out waiting for other loading taks to finish!", 8);
//             TaskData.SetStatus(API::ManagerTaskStatus::TimeOut);
//             return;
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }
//     LoadingSimSetter(true);  // Elicits special behavior in NESRequest to replace SimID, etc.
//     LoadingSimReplaceID = -1;
//     // *** Not sure if we should prepend with "std::string loadresponse = " to keep the full
//     //     record of the loading requests in the task output JSON.
//     NESRequest(TaskData.InputData, &Simulations_=&TaskData);
//     TaskData.OutputData["SimulationID"] = API::ManagerTaskData.ReplaceSimulationID;
//     //TaskData.NewSimulationID = GetSimReplaceID();
//     TaskData.SetStatus(API::ManagerTaskStatus::Success);
//     LoadingSimSetter(false);
// }

// void Manager::SimLoadingTask(API::ManagerTaskData& TaskData) {
//     // *** Not sure if we should prepend with "std::string loadresponse = " to keep the full
//     //     record of the loading requests in the task output JSON.
//     NESRequest(TaskData.InputData, &TaskData);
//     TaskData.OutputData["SimulationID"] = TaskData.ReplaceSimulationID;
//     TaskData.SetStatus(API::ManagerTaskStatus::Success);
//     Logger_->Log("Loading Simulation " + std::to_string(TaskData.ReplaceSimulationID) + " Completed", 2);
// }

// void SimLoadingTaskThread(API::ManagerTaskData* TaskData) {
//     if (!TaskData) return;
//     // TaskData->Man.SimLoadingTask(*TaskData); // Run the rest back in the Manager for full context.
// }

// // Mostly, this is called through API::HandlerData::NewSimulation().
// Simulation* Manager::MakeSimulation() {
//     Simulations_.push_back(std::make_unique<Simulation>(Logger_));
//     int SimID = Simulations_.size()-1;
//     Simulation* TheNewSimulation = Simulations_.at(SimID).get();
//     TheNewSimulation->ID = SimID;
//     return TheNewSimulation;
// }


std::string Manager::SimulationCreate(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, "SimulationCreate", &Simulations_, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Parse Request
    std::string SimulationName;
    if (!Handle.GetParString("Name", SimulationName)) {
        return Handle.ErrResponse();
    }
    Logger_->Log("New Sim Has Name " + SimulationName, 3);

    // Build New Simulation Object
    Simulations_.push_back(std::make_unique<Simulation>(Logger_));
    Simulation* Sim = Simulations_[Simulations_.size() - 1].get();
    assert(Sim != nullptr);
    Sim->Name = SimulationName;
    Sim->CurrentTask = SIMULATION_NONE;
    Sim->ID = Simulations_.size() - 1;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, VisualizerPool_, &StopThreads_));

    // Return Result ID
    return Handle.ResponseWithID("SimulationID", Sim->ID);
}

std::string Manager::SimulationReset(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, "SimulationReset", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Handle.Sim()->CurrentTask = SIMULATION_RESET; // request a reset be done
    // Handle.Sim()->WorkRequested = true;

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

// This request starts at Simulation Task.
std::string Manager::SimulationRunFor(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, "SimulationRunFor", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "SimulationRecordAll", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    float MaxRecordTime = -1.0;
    if (!Handle.GetParFloat("MaxRecordTime_ms", MaxRecordTime)) {
        return Handle.ErrResponse();
    }
    Handle.Sim()->SetRecordAll(MaxRecordTime);

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string Manager::SimulationGetRecording(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SimulationGetRecording", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Recording"] = Handle.Sim()->GetRecordingJSON(); //Handle.Sim()->RecordingBlob;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::SimulationGetStatus(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SimulationGetStatus", &Simulations_, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["IsSimulating"] = (bool)(Handle.Sim()->IsProcessing || Handle.Sim()->WorkRequested || Handle.Sim()->IsRendering);
    ResponseJSON["RealWorldTimeRemaining_ms"] = 0.0;
    ResponseJSON["RealWorldTimeElapsed_ms"] = 0.0;
    ResponseJSON["InSimulationTime_ms"] = Handle.Sim()->T_ms;
    ResponseJSON["InSimulationTimeRemaining_ms"] = 0.0;
    ResponseJSON["PercentComplete"] = 0.0;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}


// std::string Manager::SimulationSave(std::string _JSONRequest) {

//     API::HandlerData Handle(_JSONRequest, "SimulationSave", &Simulations_);
//     if (Handle.HasError()) {
//         return Handle.ErrResponse();
//     }

//     //std::cout << "Number of stored requests: " << Handle.Sim()->NumStoredRequests() << "\n\n";
//     //std::cout << Handle.Sim()->StoredRequestsToString() << '\n';

//     std::string SavedSimName = Handle.Sim()->StoredRequestsSave();
//     if (SavedSimName.empty()) {
//         return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
//     }
//     Logger_->Log("Saved Simulation with "+std::to_string(Handle.Sim()->NumStoredRequests())+" stored requets to "+SavedSimName, 3);

//     // Return Saved File Name
//     return Handle.ResponseWithID("SavedSimName", SavedSimName);
// }


/**
 * This can be a long task (from a computer's perspective) and can lead to
 * connection closing without a response if the handler response does not
 * come for a while.
 * 
 * Do the actual loading within a task, here just launch the task and
 * return the response that it was launched, and return the task ID.
 * 
 */
// // This request starts a Manager Task.
// std::string Manager::SimulationLoad(std::string _JSONRequest) {

//     API::HandlerData Handle(_JSONRequest, "SimulationCreate", &Simulations_, true, true);
//     if (Handle.HasError()) {
//         return Handle.ErrResponse();
//     }

//     // Get the Simulation File Name
//     std::string SavedSimName;
//     if (!Handle.GetParString("SavedSimName", SavedSimName)) {
//         return Handle.ErrResponse();
//     }
//     Logger_->Log("Loading Saved Simulation " + SavedSimName, 2);

//     // Prepare data structure to run the actual Simulation loading in a task
//     std::unique_ptr<API::ManagerTaskData> LoadTaskData = std::make_unique<API::ManagerTaskData>(*this);

//     // Check if save file exists and load its request contents into the task data
//     if (!LoadFileIntoString("SavedSimulations/"+SavedSimName+".NES", LoadTaskData->InputData)) {
//         Logger_->Log("Unable to Read Simulation Save File " + SavedSimName, 8);
//         return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
//     }

//     // Launch loading task thread
//     LoadTaskData->Task = std::make_unique<std::thread>(SimLoadingTaskThread, LoadTaskData.get());

//     // Add task with fresh task status and get task ID to be returned to requestor
//     int TaskID = AddManagerTask(LoadTaskData);
//     if (TaskID<0) {
//         Logger_->Log("Unable to launch Loading Task", 8);
//         return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
//     }

//     // Return Result ID
//     return Handle.ResponseWithID("TaskID", TaskID);
// }

std::string Manager::SimulationGetGeoCenter(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SimulationGetGeoCenter", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    
    Geometries::Vec3D GeoCenter_um = Handle.Sim()->GetGeoCenter();

    // Return GeoCenter vector
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    Util::SetVec3(ResponseJSON, GeoCenter_um, "GeoCenter");
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::SphereCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SphereCreate", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "CylinderCreate", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "BoxCreate", &Simulations_);
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

/**
 * Creates a BS Compartment with form and function.
 * Form: A shape.
 * Function: Some parameters.
 */
std::string Manager::BSCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "BSCreate", &Simulations_);
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

std::string Manager::StapleCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "StapleCreate", &Simulations_);
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
std::string Manager::ReceptorCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "ReceptorCreate", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "BSNeuronCreate", &Simulations_);
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

std::string Manager::PatchClampDACCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "PatchClampDACCreate", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "PatchClampDACSetOutputList", &Simulations_);
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

std::string Manager::PatchClampADCCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "PatchClampADCCreate", &Simulations_);
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
 
    API::HandlerData Handle(_JSONRequest, "PatchClampADCSetSampleRate", &Simulations_);
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

std::string Manager::PatchClampADCGetRecordedData(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "PatchClampADCGetRecordedData", &Simulations_);
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
std::string Manager::SetSpecificAPTimes(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SetSpecificAPTimes", &Simulations_);
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
std::string Manager::SetSpontaneousActivity(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SetSpontaneousActivity", &Simulations_);
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
            neuron->SetSpontaneousActivity(SpikeIntervalMean_ms, SpikeIntervalStDev_ms);
        }
    } else {
        for (const auto& neuron_id : NeuronIDs) {
            Handle.Sim()->Neurons.at(neuron_id)->SetSpontaneousActivity(SpikeIntervalMean_ms, SpikeIntervalStDev_ms);
        }
    }

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "ElectrodeSpecs": [
 *     {
 *       "name": <electrode-name>,
 *       "tip_position": <3D-vector>,
 *       "end_position": <3D-vector>,
 *       "sites": [
 *         [<x-ratio>, <y-ratio>, <z-ratio>],
 *         (more site xyz ratios)
 *       ]
 *       "noise_level": <float>,
 *     },
 *     (more electrode specs)
 *   ]
 * }
 * Responds:
 * {
 *   "StatusCode": <status-code>,
 *   "ElectrodeIDs": [ <id>, (more IDs) ]
 * }
 */
std::string Manager::AttachRecordingElectrodes(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "AttachRecordingElectrodes", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    std::vector<int> ElectrodeIDs;

    nlohmann::json::iterator ElectrodeSpecsIterator;
    if (!Handle.FindPar("ElectrodeSpecs", ElectrodeSpecsIterator, const_cast<nlohmann::json&>(Handle.ReqJSON()))) {
        Handle.ErrResponse();
    }
    auto ElectrodeList = ElectrodeSpecsIterator.value();
    if (!ElectrodeList.is_array()) {
        Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    for (auto& ElectodeData : ElectrodeList) {

        // Collect electrode parameters and make electrode
        Tools::RecordingElectrode E(Handle.Sim());
        if (!Handle.GetParString("name", E.Name, ElectodeData)) {
            Handle.ErrResponse();
        }
        std::vector<float> TipPosition;
        if (!Handle.GetParVecFloat("tip_position", TipPosition, ElectodeData)) {
            Handle.ErrResponse();
        }
        std::vector<float> EndPosition;
        if (!Handle.GetParVecFloat("end_position", EndPosition, ElectodeData)) {
            Handle.ErrResponse();
        }
        if ((TipPosition.size()<3) || (EndPosition.size()<3)) {
            Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        E.TipPosition_um = TipPosition;
        E.EndPosition_um = EndPosition;
        if (!Handle.GetParFloat("noise_level", E.NoiseLevel, ElectodeData)) {
            Handle.ErrResponse();
        }
        nlohmann::json::iterator SitesIterator;
        if (!Handle.FindPar("sites", SitesIterator, ElectodeData)) {
            Handle.ErrResponse();
        }
        if (!SitesIterator.value().is_array()) {
            Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
        }
        for (auto& SiteData : SitesIterator.value()) {
            std::vector<float> SitePosition;
            if (!Handle.GetParVecFloat("", SitePosition, SiteData)) {
                Handle.ErrResponse();
            }
            if (SitePosition.size()<3) {
                Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
            }
            E.Sites.emplace_back(SitePosition);
        }

        E.ID = Handle.Sim()->RecordingElectrodes.size();
        Handle.Sim()->RecordingElectrodes.push_back(std::make_unique<Tools::RecordingElectrode>(E));

        ElectrodeIDs.emplace_back(E.ID);
    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ElectrodeIDs"] = ElectrodeIDs;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "name": <Ca-imaging-name>,
 *   "fluorescing_neurons": [ <neuron-id>... ], -- Empty means all.
 *   "calcium_indicator": <indicator-type>,
 *   "indicator_rise_ms": <float>,
 *   "indicator_decay_ms": <float>,
 *   "indicator_interval_ms": <float>, -- Determines max trackable spike rate.
 *   "voxelspace_side_px": <num-pixels>,
 *   "imaged_subvolume": {
 *     "center": <3D-vector>,
 *     "half": <3D-vector>,
 *     "dx": <3D-vector>,
 *     "dy": <3D-vector>,
 *     "dz": <3D-vector>, -- Positive dz indicates most visible top surface.
 *   },
 *   "generate_during_sim": <bool>
 * }
 * Note: Possibly add "microscope_lensfront_position_um": <3D-vector>, "microscope_rear_position_um": <3D-vector>,
 * Responds:
 * {
 *   "StatusCode": <status-code>,
 * }
 */
std::string Manager::CalciumImagingAttach(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "CalciumImagingAttach", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Tools::CalciumImaging C(Handle.Sim());
    if (!Handle.GetParString("name", C.Name)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParVecInt("fluorescing_neurons", C.FluorescingNeurons)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParString("calcium_indicator", C.CalciumIndicator)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParFloat("indicator_rise_ms", C.IndicatorRise_ms)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParFloat("indicator_decay_ms", C.IndicatorDecay_ms)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParFloat("indicator_interval_ms", C.IndicatorInterval_ms)) {
        Handle.ErrResponse();
    }
    C.ImagingInterval_ms = C.IndicatorInterval_ms+10.0; // *** TODO: Could make this independently settable!
    if (!Handle.GetParInt("voxelspace_side_px", C.VoxelSpaceSide_px)) {
        Handle.ErrResponse();
    }
    if (!Handle.GetParBool("generate_during_sim", C.GenerateDuringSim)) {
        Handle.ErrResponse();
    }
    nlohmann::json::iterator SubVolumeIterator;
    if (!Handle.FindPar("imaged_subvolume", SubVolumeIterator)) {
        Handle.ErrResponse();
    }
    auto SubVolume = SubVolumeIterator.value();
    if (!SubVolume.is_object()) {
        Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    std::vector<float> Center;
    if (!Handle.GetParVecFloat("center", Center, SubVolume)) {
        Handle.ErrResponse();
    }
    std::vector<float> Half;
    if (!Handle.GetParVecFloat("half", Half, SubVolume)) {
        Handle.ErrResponse();
    }
    std::vector<float> Dx;
    if (!Handle.GetParVecFloat("dx", Dx, SubVolume)) {
        Handle.ErrResponse();
    }
    std::vector<float> Dy;
    if (!Handle.GetParVecFloat("dy", Dy, SubVolume)) {
        Handle.ErrResponse();
    }
    std::vector<float> Dz;
    if (!Handle.GetParVecFloat("dz", Dz, SubVolume)) {
        Handle.ErrResponse();
    }
    if ((Center.size()<3) || (Half.size()<3) || (Dx.size()<3) || (Dy.size()<3) || (Dz.size()<3)) {
        Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    C.Center_um = Center;
    C.Half = Half;
    C.Dx = Dx;
    C.Dy = Dy;
    C.Dz = Dz;

    C.ID = 0;
    Handle.Sim()->CaImaging = std::make_unique<Tools::CalciumImaging>(C);
    //Handle.Sim()->CaImaging->Init(); // automatically called by the explicit copy constructor

    return Handle.ErrResponse(); // ok
}

std::string Manager::CalciumImagingShowVoxels(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "CalciumImagingShowVoxels", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON = Handle.Sim()->GetCaImagingVoxelsJSON();
    ResponseJSON["StatusCode"] = 0; // ok
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::CalciumImagingRecordAposteriori(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "CalciumImagingRecordAposteriori", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->CalciumImagingRecordAposteriori();

    return Handle.ErrResponse(); // ok
}

std::string Manager::SetRecordInstruments(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "SetRecordInstruments", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    float MaxRecordTime = -1.0;
    if (!Handle.GetParFloat("MaxRecordTime_ms", MaxRecordTime)) {
        return Handle.ErrResponse();
    }
    Handle.Sim()->SetRecordInstruments(MaxRecordTime);

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

std::string Manager::GetInstrumentRecordings(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "GetInstrumentRecordings", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON = Handle.Sim()->GetInstrumentsRecordingJSON();
    ResponseJSON["StatusCode"] = 0; // ok
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

/**
 * Expects _JSONRequest:
 * {
 *   "TaskID": <Manager-Task-ID>
 * }
 * 
 * Responds:
 * {
 *   "StatusCode": <status-code>,
 *   "TaskStatus": <task-status-code>
 * }
 */
std::string Manager::ManTaskStatus(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, "ManTaskStatus", &Simulations_, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }



// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.
    // Set Params
    int ManTaskID = -1;
    if (!Handle.GetParInt("TaskID", ManTaskID)) {
        return Handle.ErrResponse();
    }

    // Get Manager Task Data
    auto it = ManagerTasks.find(ManTaskID);
    if (it == ManagerTasks.end()) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }
    API::ManagerTaskData* taskdata_ptr = it->second.get();
    if (!taskdata_ptr) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    }


    taskdata_ptr->OutputData["StatusCode"] = int(Handle.GetStatus());
    taskdata_ptr->IncludeStatusInOutputData();

    // Return Result ID
    return Handle.ResponseAndStoreRequest(taskdata_ptr->OutputData);
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

    API::HandlerData Handle(_JSONRequest, "VisualizerGenerateImage", &Simulations_);

  
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

    API::HandlerData Handle(_JSONRequest, "VisualizerGetStatus", &Simulations_, true);
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

    API::HandlerData Handle(_JSONRequest, "VisualizerGetImageHandles", &Simulations_);
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

    API::HandlerData Handle(_JSONRequest, "VisualizerGetImage", &Simulations_);
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

// bool Manager::IsSimulationBusy(Simulation* _Sim) {
//     return _Sim->IsProcessing || _Sim->WorkRequested;
// }

std::vector<std::unique_ptr<Simulation>>* Manager::GetSimulationVectorPtr() {
    return &Simulations_;
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
