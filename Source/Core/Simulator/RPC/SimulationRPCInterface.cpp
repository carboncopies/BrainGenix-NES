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







SimulationRPCInterface::SimulationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);
    assert(_RPCManager != nullptr);
    assert(_RenderPool != nullptr);
    assert(_VisualizerPool != nullptr);


    Config_ = _Config;
    Logger_ = _Logger;
    RenderPool_ = _RenderPool;
    VisualizerPool_ = _VisualizerPool;
    RPCManager_ = _RPCManager;



    // Register Callback For CreateSim
    // _RPCManager->AddRoute(NES_Request_handlers.at("ManTaskStatus").Route, Logger_, [this](std::string RequestJSON){ return ManTaskStatus(RequestJSON);});

    _RPCManager->AddRoute("Simulation/Create",                    std::bind(&SimulationRPCInterface::SimulationCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Reset",                     std::bind(&SimulationRPCInterface::SimulationReset, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/SetRandomSeed",             std::bind(&SimulationRPCInterface::SimulationSetSeed, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/RunFor",                    std::bind(&SimulationRPCInterface::SimulationRunFor, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/RecordAll",                 std::bind(&SimulationRPCInterface::SimulationRecordAll, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetRecording",              std::bind(&SimulationRPCInterface::SimulationGetRecording, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetStatus",                 std::bind(&SimulationRPCInterface::SimulationGetStatus, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetGeoCenter",              std::bind(&SimulationRPCInterface::SimulationGetGeoCenter, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/AttachRecordingElectrodes", std::bind(&SimulationRPCInterface::AttachRecordingElectrodes, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/SetRecordInstruments",      std::bind(&SimulationRPCInterface::SetRecordInstruments, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetInstrumentRecordings",   std::bind(&SimulationRPCInterface::GetInstrumentRecordings, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Save",                      std::bind(&SimulationRPCInterface::SimulationSave, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/GetSave",                   std::bind(&SimulationRPCInterface::SimulationGetSave, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Load",                      std::bind(&SimulationRPCInterface::SimulationLoad, this, std::placeholders::_1));

    _RPCManager->AddRoute("ManTaskStatus",                        std::bind(&SimulationRPCInterface::ManTaskStatus, this, std::placeholders::_1));

    // _RPCManager->AddRoute("CalciumImagingAttach", std::bind(&SimulationRPCInterface::CalciumImagingAttach, this, std::placeholders::_1));
    // _RPCManager->AddRoute("CalciumImagingShowVoxels", std::bind(&SimulationRPCInterface::CalciumImagingShowVoxels, this, std::placeholders::_1));
    // _RPCManager->AddRoute("CalciumImagingRecordAposteriori", std::bind(&SimulationRPCInterface::CalciumImagingRecordAposteriori, this, std::placeholders::_1));

    // Start SE Thread
    StopThreads_ = false;

}

SimulationRPCInterface::~SimulationRPCInterface() {

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

// A API::ManagerTaskData struct must have been prepared and the thread already launched.
int SimulationRPCInterface::AddManagerTask(std::unique_ptr<API::ManagerTaskData>& TaskData) {
    // Get Task ID
    int TaskID = NextManTaskID;
    NextManTaskID++;
    TaskData->ID = TaskID;

    // Place task data into ManagerTasks
    ManagerTasks[TaskID].reset(TaskData.release()); // Release task data object pointer into unique pointer in map.

    return TaskID;
}

// // *** WE CAN PROBABLY DO WITHOUT THIS CONSTRAINT NOW (AS WE USE LOCAL PARAMS)!
// // Only one thread at a time, others wait for lock release.
// void SimulationRPCInterface::LoadingSimSetter(bool SetTo) {
//     std::lock_guard<std::mutex> guard(LoadingSimSetterMutex);
//     LoadingSim = SetTo;
// }

// // We can run only one loading task at a time, because we are using some Manager-global
// // flags and variables to modify the behavior of NESRequest and HandleData, namely to
// // establish the new Simulation ID and to replace loaded SimIDs with that.
// // Before proceeding, we wait for other loading tasks in the queue to finish.
// void SimulationRPCInterface::SimLoadingTask(API::ManagerTaskData& TaskData) {
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

void SimulationRPCInterface::SimLoadingTask(API::ManagerTaskData& TaskData) {
    // *** Not sure if we should prepend with "std::string loadresponse = " to keep the full
    //     record of the loading requests in the task output JSON.

    // Build New Simulation Object
    Simulations_.push_back(std::make_unique<Simulation>(Logger_));
    Simulation* Sim = Simulations_[Simulations_.size() - 1].get();
    assert(Sim != nullptr);
    Sim->Name = "Loaded Simulation";
    Sim->CurrentTask = SIMULATION_NONE;
    Sim->ID = Simulations_.size() - 1;
    Sim->SetRandomSeed(0);

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, VisualizerPool_, &StopThreads_));


    // Get New SimID
    size_t NewSimID = Sim->ID;
    TaskData.ReplaceSimulationID = NewSimID;

    RPCManager_->NESRequest(TaskData.InputData, NewSimID);
    TaskData.OutputData["SimulationID"] = TaskData.ReplaceSimulationID;
    TaskData.SetStatus(API::ManagerTaskStatus::Success);
    Logger_->Log("Loading Simulation " + std::to_string(TaskData.ReplaceSimulationID) + " Completed", 2);
}

void SimLoadingTaskThread(SimulationRPCInterface* _Manager, API::ManagerTaskData* TaskData) {
    if (!TaskData) return;
    _Manager->SimLoadingTask(*TaskData); // Run the rest back in the Manager for full context.
}

// // Mostly, this is called through API::HandlerData::NewSimulation().
// Simulation* SimulationRPCInterface::MakeSimulation() {
//     Simulations_.push_back(std::make_unique<Simulation>(Logger_));
//     int SimID = Simulations_.size()-1;
//     Simulation* TheNewSimulation = Simulations_.at(SimID).get();
//     TheNewSimulation->ID = SimID;
//     return TheNewSimulation;
// }

/**
 * Expects "Name" and "Seed" parameters.
 */
std::string SimulationRPCInterface::SimulationCreate(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Create", &Simulations_, true, true);
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
    Sim->SetRandomSeed(0);
    Sim->CurrentTask = SIMULATION_NONE;
    Sim->ID = Simulations_.size() - 1;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, VisualizerPool_, &StopThreads_));

    // Return Result ID
    return Handle.ResponseWithID("SimulationID", Sim->ID);
}

std::string SimulationRPCInterface::SimulationReset(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Reset", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->CurrentTask = SIMULATION_RESET; // request a reset be done
    Handle.Sim()->WorkRequested = true;

    // Return Result ID
    return Handle.ErrResponse(); // ok
}


std::string SimulationRPCInterface::SimulationSetSeed(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/SetRandomSeed", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    int Seed;
    Handle.GetParInt("Seed", Seed);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->SetRandomSeed(Seed);

    // Return Result ID
    return Handle.ErrResponse(); // ok
}


// This request starts at Simulation Task.
std::string SimulationRPCInterface::SimulationRunFor(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/RunFor", &Simulations_);
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

std::string SimulationRPCInterface::SimulationRecordAll(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/RecordAll", &Simulations_);
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

std::string SimulationRPCInterface::SimulationGetRecording(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/GetRecording", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Recording"] = Handle.Sim()->GetRecordingJSON(); //Handle.Sim()->RecordingBlob;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string SimulationRPCInterface::SimulationGetStatus(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/GetStatus", &Simulations_, true);
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


std::string SimulationRPCInterface::SimulationSave(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Save", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    //std::cout << "Number of stored requests: " << Handle.Sim()->NumStoredRequests() << "\n\n";
    //std::cout << Handle.Sim()->StoredRequestsToString() << '\n';

    std::string SavedSimName = Handle.Sim()->StoredRequestsSave();
    if (SavedSimName.empty()) {
        return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
    }
    Logger_->Log("Saved Simulation with "+std::to_string(Handle.Sim()->NumStoredRequests())+" stored requets to "+SavedSimName, 3);

    // Return Saved File Name
    return Handle.ResponseWithID("SavedSimName", SavedSimName);
}

std::string SimulationRPCInterface::SimulationGetSave(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/GetSave", &Simulations_, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    //std::cout << "Number of stored requests: " << Handle.Sim()->NumStoredRequests() << "\n\n";
    //std::cout << Handle.Sim()->StoredRequestsToString() << '\n';

    std::string SaveName;
    Handle.GetParString("SaveHandle", SaveName);

    // Minor security feature (probably still exploitable, so be warned!)
    // We just remove .. from the incoming handle for the image, since they're just files right now
    // as such, if we didnt strip that, then people could read any files on the server!
    // Also, we prepend a '.' so people can't try and get to the root
    std::string Pattern = "..";
    std::string::size_type i = SaveName.find(Pattern);
    while (i != std::string::npos) {
        Logger_->Log("Detected '..' In SaveName, It's Possible That Someone Is Trying To Do Something Nasty", 8);
        SaveName.erase(i, Pattern.length());
        i = SaveName.find(Pattern, i);
    }
    std::string SafeHandle = "SavedSimulations/" + SaveName + ".NES";


    // Now Check If The Handle Is Valid, If So, Load It
    std::ifstream DataStream(SafeHandle.c_str(), std::ios::binary);
    std::string RawData;
    if (DataStream.good()) {
        std::stringstream Buffer;
        Buffer << DataStream.rdbuf();
        RawData = Buffer.str();
        DataStream.close();

    } else {
        Logger_->Log("An Invalid SaveHandle Was Provided " + SafeHandle, 6);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // error
        return ResponseJSON.dump();
    }

    // Now, Convert It To Base64
    std::string Base64Data = base64_encode(reinterpret_cast<const unsigned char*>(RawData.c_str()), RawData.length());


    return Handle.StringResponse("SaveData", Base64Data);

    

}



/**
 * This can be a long task (from a computer's perspective) and can lead to
 * connection closing without a response if the handler response does not
 * come for a while.
 * 
 * Do the actual loading within a task, here just launch the task and
 * return the response that it was launched, and return the task ID.
 * 
 */
// This request starts a Manager Task.
std::string SimulationRPCInterface::SimulationLoad(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Load", &Simulations_, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get the Simulation File Name
    std::string SavedSimName;
    if (!Handle.GetParString("SavedSimName", SavedSimName)) {
        return Handle.ErrResponse();
    }
    Logger_->Log("Loading Saved Simulation " + SavedSimName, 2);

    // Prepare data structure to run the actual Simulation loading in a task
    std::unique_ptr<API::ManagerTaskData> LoadTaskData = std::make_unique<API::ManagerTaskData>();

    // Check if save file exists and load its request contents into the task data
    if (!LoadFileIntoString("SavedSimulations/"+SavedSimName+".NES", LoadTaskData->InputData)) {
        Logger_->Log("Unable to Read Simulation Save File " + SavedSimName, 8);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
    }

    // Launch loading task thread
    LoadTaskData->Task = std::make_unique<std::thread>(SimLoadingTaskThread, this, LoadTaskData.get());

    // Add task with fresh task status and get task ID to be returned to requestor
    int TaskID = AddManagerTask(LoadTaskData);
    if (TaskID<0) {
        Logger_->Log("Unable to launch Loading Task", 8);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusGeneralFailure);
    }

    // Return Result ID
    return Handle.ResponseWithID("TaskID", TaskID);
}

std::string SimulationRPCInterface::SimulationGetGeoCenter(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/GetGeoCenter", &Simulations_);
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
std::string SimulationRPCInterface::AttachRecordingElectrodes(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/AttachRecordingElectrodes", &Simulations_);
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
std::string SimulationRPCInterface::CalciumImagingAttach(std::string _JSONRequest) {
    return "";
    // API::HandlerData Handle(_JSONRequest, Logger_, "CalciumImagingAttach",  &Simulations_);
    // if (Handle.HasError()) {
    //     return Handle.ErrResponse();
    // }

    // Tools::CalciumImaging C(Handle.Sim());
    // if (!Handle.GetParString("name", C.Name)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParVecInt("fluorescing_neurons", C.FluorescingNeurons)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParString("calcium_indicator", C.CalciumIndicator)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParFloat("indicator_rise_ms", C.IndicatorRise_ms)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParFloat("indicator_decay_ms", C.IndicatorDecay_ms)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParFloat("indicator_interval_ms", C.IndicatorInterval_ms)) {
    //     Handle.ErrResponse();
    // }
    // C.ImagingInterval_ms = C.IndicatorInterval_ms+10.0; // *** TODO: Could make this independently settable!
    // if (!Handle.GetParInt("voxelspace_side_px", C.VoxelSpaceSide_px)) {
    //     Handle.ErrResponse();
    // }
    // if (!Handle.GetParBool("generate_during_sim", C.GenerateDuringSim)) {
    //     Handle.ErrResponse();
    // }
    // nlohmann::json::iterator SubVolumeIterator;
    // if (!Handle.FindPar("imaged_subvolume", SubVolumeIterator)) {
    //     Handle.ErrResponse();
    // }
    // auto SubVolume = SubVolumeIterator.value();
    // if (!SubVolume.is_object()) {
    //     Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    // }
    // std::vector<float> Center;
    // if (!Handle.GetParVecFloat("center", Center, SubVolume)) {
    //     Handle.ErrResponse();
    // }
    // std::vector<float> Half;
    // if (!Handle.GetParVecFloat("half", Half, SubVolume)) {
    //     Handle.ErrResponse();
    // }
    // std::vector<float> Dx;
    // if (!Handle.GetParVecFloat("dx", Dx, SubVolume)) {
    //     Handle.ErrResponse();
    // }
    // std::vector<float> Dy;
    // if (!Handle.GetParVecFloat("dy", Dy, SubVolume)) {
    //     Handle.ErrResponse();
    // }
    // std::vector<float> Dz;
    // if (!Handle.GetParVecFloat("dz", Dz, SubVolume)) {
    //     Handle.ErrResponse();
    // }
    // if ((Center.size()<3) || (Half.size()<3) || (Dx.size()<3) || (Dy.size()<3) || (Dz.size()<3)) {
    //     Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);
    // }
    // C.Center_um = Center;
    // C.Half = Half;
    // C.Dx = Dx;
    // C.Dy = Dy;
    // C.Dz = Dz;

    // C.ID = 0;
    // Handle.Sim()->CaImaging = std::make_unique<Tools::CalciumImaging>(C);
    // //Handle.Sim()->CaImaging->Init(); // automatically called by the explicit copy constructor

    // return Handle.ErrResponse(); // ok
}

std::string SimulationRPCInterface::CalciumImagingShowVoxels(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "CalciumImagingShowVoxels", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON = Handle.Sim()->GetCaImagingVoxelsJSON();
    ResponseJSON["StatusCode"] = 0; // ok
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string SimulationRPCInterface::CalciumImagingRecordAposteriori(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "CalciumImagingRecordAposteriori", &Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->CalciumImagingRecordAposteriori();

    return Handle.ErrResponse(); // ok
}

std::string SimulationRPCInterface::SetRecordInstruments(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/SetRecordInstruments", &Simulations_);
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

std::string SimulationRPCInterface::GetInstrumentRecordings(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/GetInstrumentRecordings", &Simulations_);
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
std::string SimulationRPCInterface::ManTaskStatus(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "ManTaskStatus", &Simulations_, true, true);
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










// bool SimulationRPCInterface::IsSimulationBusy(Simulation* _Sim) {
//     return _Sim->IsProcessing || _Sim->WorkRequested;
// }

std::vector<std::unique_ptr<Simulation>>* SimulationRPCInterface::GetSimulationVectorPtr() {
    return &Simulations_;
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
