#include <Simulator/Manager.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/bgStatusCode.h>
#include <Simulator/Geometries/VecTools.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

namespace BG {
namespace NES {
namespace Simulator {

// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.

typedef std::string NESRequest_func_t(Manager&, const nlohmann::json&, ManagerTaskData*);

struct RouteAndHandler {
    std::string Route;
    NESRequest_func_t* Handler;

    RouteAndHandler(const std::string & _Route, NESRequest_func_t* _Handler): Route(_Route), Handler(_Handler) {}
};

//typedef std::map<std::string, NESRequest_func_t*> NESRequest_map_t;
typedef std::map<std::string, RouteAndHandler> NESRequest_map_t;

std::string SimulationCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationCreate(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationResetHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationReset(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationRunForHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationRunFor(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationRecordAllHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationRecordAll(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationGetRecordingHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationGetRecording(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationGetStatusHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationGetStatus(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationBuildMeshHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationBuildMesh(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationSaveHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationSave(ReqParams.dump(), called_by_manager_task);
}
std::string SimulationLoadHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SimulationLoad(ReqParams.dump(), called_by_manager_task);
}
std::string SphereCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SphereCreate(ReqParams.dump(), called_by_manager_task);
}
std::string CylinderCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.CylinderCreate(ReqParams.dump(), called_by_manager_task);
}
std::string BoxCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.BoxCreate(ReqParams.dump(), called_by_manager_task);
}
std::string BSCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.BSCreate(ReqParams.dump(), called_by_manager_task);
}
std::string StapleCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.StapleCreate(ReqParams.dump(), called_by_manager_task);
}
std::string ReceptorCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.ReceptorCreate(ReqParams.dump(), called_by_manager_task);
}
std::string BSNeuronCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.BSNeuronCreate(ReqParams.dump(), called_by_manager_task);
}
std::string PatchClampDACCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.PatchClampDACCreate(ReqParams.dump(), called_by_manager_task);
}
std::string PatchClampADCCreateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.PatchClampADCCreate(ReqParams.dump(), called_by_manager_task);
}
std::string PatchClampDACSetOutputListHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.PatchClampDACSetOutputList(ReqParams.dump(), called_by_manager_task);
}
std::string PatchClampADCSetSampleRateHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.PatchClampADCSetSampleRate(ReqParams.dump(), called_by_manager_task);
}
std::string PatchClampADCGetRecordedDataHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.PatchClampADCGetRecordedData(ReqParams.dump(), called_by_manager_task);
}
std::string SetSpecificAPTimesHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.SetSpecificAPTimes(ReqParams.dump(), called_by_manager_task);
}
std::string ManTaskStatusHandler(Manager& Man, const nlohmann::json& ReqParams, ManagerTaskData* called_by_manager_task) {
    return Man.ManTaskStatus(ReqParams.dump(), called_by_manager_task);
}

const NESRequest_map_t NES_Request_handlers = {
    {"SimulationCreate", {"Simulation/Create", SimulationCreateHandler} },
    {"SimulationReset", {"Simulation/Reset", SimulationResetHandler} },
    {"SimulationRunFor", {"Simulation/RunFor", SimulationRunForHandler} },
    {"SimulationRecordAll", {"Simulation/RecordAll", SimulationRecordAllHandler} },
    {"SimulationGetRecording", {"Simulation/GetRecording", SimulationGetRecordingHandler} },
    {"SimulationGetStatus", {"Simulation/GetStatus", SimulationGetStatusHandler} },
    {"SimulationBuildMesh", {"Simulation/BuildMesh", SimulationBuildMeshHandler} },
    {"SimulationSave", {"Simulation/Save", SimulationSaveHandler} },
    {"SimulationLoad", {"Simulation/Load", SimulationLoadHandler} },

    {"SphereCreate", {"Geometry/Shape/Sphere/Create", SphereCreateHandler} },
    {"BulkSphereCreate", {"Geometry/Shape/Sphere/BulkCreate", nullptr} },
    {"CylinderCreate", {"Geometry/Shape/Cylinder/Create", CylinderCreateHandler} },
    {"BulkCylinderCreate", {"Geometry/Shape/Cylinder/BulkCreate", nullptr} },
    {"BoxCreate", {"Geometry/Shape/Box/Create", BoxCreateHandler} },
    {"BulkBoxCreate", {"Geometry/Shape/Box/BulkCreate", nullptr}},

    {"BSCreate", {"Compartment/BS/Create", BSCreateHandler} },
    {"BulkBSCreate", {"Compartment/BS/BulkCreate", nullptr}},

    {"StapleCreate", {"Connection/Staple/Create", StapleCreateHandler} },
    {"ReceptorCreate", {"Connection/Receptor/Create", ReceptorCreateHandler} },

    {"BSNeuronCreate", {"Neuron/BSNeuron/Create", BSNeuronCreateHandler} },

    {"PatchClampDACCreate", {"Tool/PatchClampDAC/Create", PatchClampDACCreateHandler} },
    {"PatchClampDACSetOutputList", {"Tool/PatchClampDAC/SetOutputList", PatchClampDACSetOutputListHandler} },

    {"PatchClampADCCreate", {"Tool/PatchClampADC/Create", PatchClampADCCreateHandler} },
    {"PatchClampADCSetSampleRate", {"Tool/PatchClampADC/SetSampleRate", PatchClampADCSetSampleRateHandler} },
    {"PatchClampADCGetRecordedData", {"Tool/PatchClampADC/GetRecordedData", PatchClampADCGetRecordedDataHandler} },

    {"SetSpecificAPTimes", {"", SetSpecificAPTimesHandler} },

    {"NESRequest", {"NES", nullptr}},

    {"ManTaskStatus", {"ManTaskStatus", ManTaskStatusHandler}},
};

// Handy class for standard handler data.
class HandlerData {
protected:
    Manager & Man;
    std::string Source;
    ManagerTaskData * ManTaskData;
    std::vector<std::unique_ptr<Simulation>>* SimVec = nullptr;

    std::string JSONRequestStr;
    nlohmann::json RequestJSON;
    API::bgStatusCode Status = API::bgStatusCode::bgStatusSuccess;

    int SimulationID = -1;
    Simulation* ThisSimulation = nullptr;

public:
    HandlerData(
        Manager* _Man,
        const std::string & _JSONRequest,
        const std::string & _Source,
        ManagerTaskData * called_by_manager_task,
        bool PermitBusy = false,
        bool NoSimulation = false
    ): Man(*_Man), Source(_Source), ManTaskData(called_by_manager_task), JSONRequestStr(_JSONRequest) {

        SimVec = Man.GetSimulationVectorPtr();
        RequestJSON = nlohmann::json::parse(_JSONRequest);

        bool isloadingsim = (ManTaskData != nullptr); // Man.IsLoadingSim();
        if (isloadingsim && (_Source == "SimulationLoad")) { // *** PERHAPS WE CAN ALLOW THIS (AS WE USE LOCAL PARAMS NOW)?
            Man.Logger()->Log("Recursive SimulationLoad attempted.", 8);
            Status = API::bgStatusCode::bgStatusGeneralFailure;
            return; // Prevents recursion.
        }

        // If this Handler was called while loading and a Simulation was not created
        // yet then only allow requests that do not need one ("NoSimulation"), such
        // as "SimulationCreate" and "NESRequest".
        if (isloadingsim && (!ManTaskData->HasReplacementSimID()) && (!NoSimulation)) {
            Man.Logger()->Log(Source+" needs a Sim, but we are still Loading and have not come across SimulationCreate yet.", 8);
            Status = API::bgStatusCode::bgStatusInvalidParametersPassed;
            return; // When loading, the first valid request must be SimulationCreate.
        }

        if (NoSimulation) {
            return;
        }

        if (isloadingsim) {
            SimulationID = ManTaskData->ReplaceSimulationID;
        } else {
            if (!GetParInt("SimulationID", SimulationID)) {
                return;
            }
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

    // See how this is used in Manager::SimulationCreate().
    Simulation* NewSimulation() {
        ThisSimulation = Man.MakeSimulation();
        SimulationID = ThisSimulation->ID;
        Man.Logger()->Log("New Sim " + SimIDStr(), 3);
        return ThisSimulation;
    }

    bool HasError() const { return (Status != API::bgStatusCode::bgStatusSuccess); }

    API::bgStatusCode GetStatus() const { return Status; }

    // Note: This is purposely NOT const nlohmann::json &, because we do NOT
    //       want to permit calls with rvalues that default-construct a
    //       nlohmann::json, as that is a way to accidentally a JSON object
    //       into JSON containing a single string, e.g. by accidentally
    //       passing ResponseJSON.dump() instead of ResponseJSON.
    // Note: Calling this from NESRequest uses store==false, because we only
    //       want to store the calls from their actual handlers, not from the
    //       NESRequest batch handler. We don't want to double-count the calls,
    //       and we want to store the individual ones, because they may be
    //       intended for different simulations (dependeing on their SimulationID).
    std::string ResponseAndStoreRequest(nlohmann::json & ResponseJSON, bool store = true) {
        if (store && (Status == API::bgStatusCode::bgStatusSuccess)) {
            if (ThisSimulation != nullptr) {
                ThisSimulation->StoreRequestHandled(Source, NES_Request_handlers.at(Source).Route, JSONRequestStr);
            }
        }
        return ResponseJSON.dump();
    }
    std::string ErrResponse(int _Status) {
        Status = API::bgStatusCode(_Status);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = _Status;
        return ResponseAndStoreRequest(ResponseJSON);
    }
    std::string ErrResponse(API::bgStatusCode _Status) { return ErrResponse(int(_Status)); }
    std::string ErrResponse() { return ErrResponse(int(Status)); }

    std::string ResponseWithID(const std::string & IDName, int IDValue) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(Status);
        ResponseJSON[IDName] = IDValue;
        return ResponseAndStoreRequest(ResponseJSON);
    }
    std::string ResponseWithID(const std::string & IDName, const std::string & IDValue) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = int(Status);
        ResponseJSON[IDName] = IDValue;
        return ResponseAndStoreRequest(ResponseJSON);
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
};

//#define SET_HANDLER_ID(handler_id_str) const std::string HandlerID(handler_id_str)

Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, API::Manager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);
    assert(_RPCManager != nullptr);
    assert(_RenderPool != nullptr);


    Config_ = _Config;
    Logger_ = _Logger;
    RenderPool_ = _RenderPool;



    // Register Callback For CreateSim
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationCreate").Route, Logger_, [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationReset").Route, Logger_, [this](std::string RequestJSON){ return SimulationReset(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationRunFor").Route, Logger_, [this](std::string RequestJSON){ return SimulationRunFor(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationRecordAll").Route, Logger_, [this](std::string RequestJSON){ return SimulationRecordAll(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationGetRecording").Route, Logger_, [this](std::string RequestJSON){ return SimulationGetRecording(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationGetStatus").Route, Logger_, [this](std::string RequestJSON){ return SimulationGetStatus(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationBuildMesh").Route, Logger_, [this](std::string RequestJSON){ return SimulationBuildMesh(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationSave").Route, Logger_, [this](std::string RequestJSON){ return SimulationSave(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("SimulationLoad").Route, Logger_, [this](std::string RequestJSON){ return SimulationLoad(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("SphereCreate").Route, Logger_, [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("BulkSphereCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkSphereCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("CylinderCreate").Route, Logger_, [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("BulkCylinderCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkCylinderCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("BoxCreate").Route, Logger_, [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("BulkBoxCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkBoxCreate(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("BSCreate").Route, Logger_, [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("BulkBSCreate").Route, Logger_, [this](std::string RequestJSON){ return BulkBSCreate(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("StapleCreate").Route, Logger_, [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("ReceptorCreate").Route, Logger_, [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("BSNeuronCreate").Route, Logger_, [this](std::string RequestJSON){ return BSNeuronCreate(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampDACCreate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampDACSetOutputList").Route, Logger_, [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCCreate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCCreate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCSetSampleRate").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCSetSampleRate(RequestJSON);});
    _RPCManager->AddRoute(NES_Request_handlers.at("PatchClampADCGetRecordedData").Route, Logger_, [this](std::string RequestJSON){ return PatchClampADCGetRecordedData(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("NESRequest").Route, Logger_, [this](std::string RequestJSON){ return NESRequest(RequestJSON);});

    _RPCManager->AddRoute(NES_Request_handlers.at("ManTaskStatus").Route, Logger_, [this](std::string RequestJSON){ return ManTaskStatus(RequestJSON);});

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

bool LoadFileIntoString(const std::string & FilePath, std::string & FileContents) {
    std::ifstream LoadFile(FilePath);
    if (!LoadFile.is_open()) return false;

    // Reserve all the necessary space in the string up-front.
    LoadFile.seekg(0, std::ios::end);   
    FileContents.reserve(LoadFile.tellg());
    LoadFile.seekg(0, std::ios::beg);

    FileContents.assign((std::istreambuf_iterator<char>(LoadFile)), std::istreambuf_iterator<char>());
    return true;    
}

// A ManagerTaskData struct must have been prepared and the thread already launched.
int Manager::AddManagerTask(std::unique_ptr<ManagerTaskData> & TaskData) {
    // Get Task ID
    int TaskID = NextManTaskID;
    NextManTaskID++;
    TaskData->ID = TaskID;

    // Place task data into ManagerTasks
    ManagerTasks[TaskID].reset(TaskData.release()); // Release task data object pointer into unique pointer in map.

    return TaskID;
}

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
// void Manager::SimLoadingTask(ManagerTaskData & TaskData) {
//     // Wait for any concurrent loading tasks that happen to be running to finish:
//     // unsigned long timeout_ms = 10000;
//     while (LoadingSim) {
//         timeout_ms--;
//         if (timeout_ms==0) {
//             Logger_->Log("SimLoadingTask timed out waiting for other loading taks to finish!", 8);
//             TaskData.SetStatus(ManagerTaskStatus::TimeOut);
//             return;
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }
//     LoadingSimSetter(true);  // Elicits special behavior in NESRequest to replace SimID, etc.
//     LoadingSimReplaceID = -1;
//     // *** Not sure if we should prepend with "std::string loadresponse = " to keep the full
//     //     record of the loading requests in the task output JSON.
//     NESRequest(TaskData.InputData, called_by_manager_task=&TaskData);
//     TaskData.OutputData["SimulationID"] = ManagerTaskData.ReplaceSimulationID;
//     //TaskData.NewSimulationID = GetSimReplaceID();
//     TaskData.SetStatus(ManagerTaskStatus::Success);
//     LoadingSimSetter(false);
// }

void Manager::SimLoadingTask(ManagerTaskData & TaskData) {
    // *** Not sure if we should prepend with "std::string loadresponse = " to keep the full
    //     record of the loading requests in the task output JSON.
    NESRequest(TaskData.InputData, &TaskData);
    TaskData.OutputData["SimulationID"] = TaskData.ReplaceSimulationID;
    TaskData.SetStatus(ManagerTaskStatus::Success);
    Logger_->Log("Loading Simulation " + std::to_string(TaskData.ReplaceSimulationID) + " Completed", 2);
}

void SimLoadingTaskThread(ManagerTaskData* TaskData) {
    if (!TaskData) return;
    TaskData->Man.SimLoadingTask(*TaskData); // Run the rest back in the Manager for full context.
}

// Mostly, this is called through HandlerData::NewSimulation().
Simulation* Manager::MakeSimulation() {
    Simulations_.push_back(std::make_unique<Simulation>(Logger_));
    int SimID = Simulations_.size()-1;
    Simulation* TheNewSimulation = Simulations_.at(SimID).get();
    TheNewSimulation->ID = SimID;
    return TheNewSimulation;
}

std::string Manager::SimulationCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "SimulationCreate", called_by_manager_task, true, true);
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
    Simulation* Sim = Handle.NewSimulation();
    assert(Sim != nullptr);
    Sim->Name = SimulationName;
    Sim->CurrentTask = SIMULATION_NONE;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, &StopThreads_));

    if (called_by_manager_task != nullptr) {
        called_by_manager_task->ReplaceSimulationID = Sim->ID;
    }

    // Return Result ID
    return Handle.ResponseWithID("SimulationID", Sim->ID);
}

std::string Manager::SimulationReset(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "SimulationReset", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Handle.Sim()->CurrentTask = SIMULATION_RESET; // request a reset be done
    // Handle.Sim()->WorkRequested = true;

    // Return Result ID
    return Handle.ErrResponse(); // ok
}

// This request starts at Simulation Task.
std::string Manager::SimulationRunFor(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "SimulationRunFor", called_by_manager_task);
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

std::string Manager::SimulationRecordAll(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SimulationRecordAll", called_by_manager_task);
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

std::string Manager::SimulationGetRecording(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SimulationGetRecording", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Recording"] = Handle.Sim()->GetRecordingJSON(); //Handle.Sim()->RecordingBlob;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::SimulationGetStatus(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SimulationGetStatus", called_by_manager_task, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["IsSimulating"] = (bool)Handle.Sim()->IsProcessing;
    ResponseJSON["RealWorldTimeRemaining_ms"] = 0.0;
    ResponseJSON["RealWorldTimeElapsed_ms"] = 0.0;
    ResponseJSON["InSimulationTime_ms"] = Handle.Sim()->T_ms;
    ResponseJSON["InSimulationTimeRemaining_ms"] = 0.0;
    ResponseJSON["PercentComplete"] = 0.0;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::SimulationBuildMesh(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SimulationBuildMesh", called_by_manager_task);
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

std::string Manager::SimulationSave(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "SimulationSave", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    //std::cout << "Number of stored requests: " << Handle.Sim()->NumStoredRequests() << "\n\n";
    //std::cout << Handle.Sim()->StoredRequestsToString() << '\n';

    std::string SavedSimName = Handle.Sim()->StoredRequestsSave();
    if (SavedSimName.empty()) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusGeneralFailure);
    }
    Logger_->Log("Saved Simulation with "+std::to_string(Handle.Sim()->NumStoredRequests())+" stored requets to "+SavedSimName, 3);

    // Return Saved File Name
    return Handle.ResponseWithID("SavedSimName", SavedSimName);
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
std::string Manager::SimulationLoad(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "SimulationCreate", called_by_manager_task, true, true);
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
    std::unique_ptr<ManagerTaskData> LoadTaskData = std::make_unique<ManagerTaskData>(*this);

    // Check if save file exists and load its request contents into the task data
    if (!LoadFileIntoString("SavedSimulations/"+SavedSimName+".NES", LoadTaskData->InputData)) {
        Logger_->Log("Unable to Read Simulation Save File " + SavedSimName, 8);
        return Handle.ErrResponse(API::bgStatusCode::bgStatusGeneralFailure);
    }

    // Launch loading task thread
    LoadTaskData->Task = std::make_unique<std::thread>(SimLoadingTaskThread, LoadTaskData.get());

    // Add task with fresh task status and get task ID to be returned to requestor
    int TaskID = AddManagerTask(LoadTaskData);
    if (TaskID<0) {
        Logger_->Log("Unable to launch Loading Task", 8);
        return Handle.ErrResponse(API::bgStatusCode::bgStatusGeneralFailure);
    }

    // Return Result ID
    return Handle.ResponseWithID("TaskID", TaskID);
}

std::string Manager::SphereCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SphereCreate", called_by_manager_task);
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

std::string Manager::CylinderCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "CylinderCreate", called_by_manager_task);
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

std::string Manager::BoxCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "BoxCreate", called_by_manager_task);
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
std::string Manager::BSCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "BSCreate", called_by_manager_task);
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
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->BSCompartments.size();
    Handle.Sim()->BSCompartments.push_back(C);

    // Return Result ID
    return Handle.ResponseWithID("CompartmentID", C.ID);
}

std::string Manager::StapleCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "StapleCreate", called_by_manager_task);
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
std::string Manager::ReceptorCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "ReceptorCreate", called_by_manager_task);
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
    CoreStructs::Neuron * SrcNeuronPtr = Handle.Sim()->FindNeuronByCompartment(C.SourceCompartmentID);
    CoreStructs::Neuron * DstNeuronPtr = Handle.Sim()->FindNeuronByCompartment(C.DestinationCompartmentID);
    if ((SrcNeuronPtr==nullptr) || (DstNeuronPtr==nullptr)) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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

std::string Manager::BSNeuronCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "BSNeuronCreate", called_by_manager_task);
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
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }

    C.ID = Handle.Sim()->Neurons.size();
    
    Handle.Sim()->Neurons.push_back(std::make_shared<BallAndStick::BSNeuron>(C));
    Handle.Sim()->NeuronByCompartment.emplace(C.SomaCompartmentID, C.ID);
    Handle.Sim()->NeuronByCompartment.emplace(C.AxonCompartmentID, C.ID);

    // Return Result ID
    return Handle.ResponseWithID("NeuronID", C.ID);
}

std::string Manager::PatchClampDACCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "PatchClampDACCreate", called_by_manager_task);
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
std::string Manager::PatchClampDACSetOutputList(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "PatchClampDACSetOutputList", called_by_manager_task);
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

std::string Manager::PatchClampADCCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "PatchClampADCCreate", called_by_manager_task);
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

std::string Manager::PatchClampADCSetSampleRate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "PatchClampADCSetSampleRate", called_by_manager_task);
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

std::string Manager::PatchClampADCGetRecordedData(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "PatchClampADCGetRecordedData", called_by_manager_task);
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
std::string Manager::SetSpecificAPTimes(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "SetSpecificAPTimes", called_by_manager_task);
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
 *   "TaskID": <Manager-Task-ID>
 * }
 * 
 * Responds:
 * {
 *   "StatusCode": <status-code>,
 *   "TaskStatus": <task-status-code>
 * }
 */
std::string Manager::ManTaskStatus(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
    HandlerData Handle(this, _JSONRequest, "ManTaskStatus", called_by_manager_task, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Set Params
    int ManTaskID = -1;
    if (!Handle.GetParInt("TaskID", ManTaskID)) {
        return Handle.ErrResponse();
    }

    // Get Manager Task Data
    auto it = ManagerTasks.find(ManTaskID);
    if (it == ManagerTasks.end()) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }
    ManagerTaskData * taskdata_ptr = it->second.get();
    if (!taskdata_ptr) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
    }

    taskdata_ptr->OutputData["StatusCode"] = int(Handle.GetStatus());
    taskdata_ptr->IncludeStatusInOutputData();

    // Return Result ID
    return Handle.ResponseAndStoreRequest(taskdata_ptr->OutputData);
}

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
std::string Manager::NESRequest(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) { // Generic JSON-based NES requests.

    // Parse Request
    //Logger_->Log(_JSONRequest, 3);
    HandlerData Handle(this, _JSONRequest, "NESRequest", called_by_manager_task, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    if (!Handle.ReqJSON().is_array()) {
        Logger_->Log("Bad format. Must be array of requests.", 8);
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);       
    }

    // Build Response
    nlohmann::json ResponseJSON = nlohmann::json::array(); // Create empty array for the list of responses.

    // For each request in the JSON list:
    for (const auto & req : Handle.ReqJSON()) {

        int ReqID = -1;
        //int SimulationID = -1;
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
                if (it->second.Handler == nullptr) {
                    ReqResponseJSON["ReqID"] = ReqID;
                    ReqResponseJSON["StatusCode"] = 1; // not a valid NES request *** TODO: use the right code
                } else {
                    std::string Response = it->second.Handler(*this, ReqParams, called_by_manager_task); // Calls the handler.
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

        }
        ResponseJSON.push_back(ReqResponseJSON);

    }

    // if (!IsLoadingSim()) {
    //     std::cout << "DEBUG ---> Responding: " << ResponseJSON.dump() << '\n'; std::cout.flush();
    // }

    return Handle.ResponseAndStoreRequest(ResponseJSON, false); // See comments at ResponseAndStoreRequest().
}

std::string Manager::Debug(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {
 
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

std::string Manager::BulkSphereCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "BulkSphereCreate", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
     
    // Get Parameters
    std::vector<float> RadiusList_um;
    std::vector<float> CenterXList_um;
    std::vector<float> CenterYList_um;
    std::vector<float> CenterZList_um;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&RadiusList_um, &(Handle.ReqJSON()), "RadiusList_um");
    Util::GetFloatVector(&CenterXList_um, &(Handle.ReqJSON()), "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &(Handle.ReqJSON()), "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &(Handle.ReqJSON()), "CenterZList_um");
    Util::GetStringVector(&NameList, &(Handle.ReqJSON()), "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = RadiusList_um.size();
    LengthsEqual &= (CenterXList_um.size() == Length);
    LengthsEqual &= (CenterYList_um.size() == Length);
    LengthsEqual &= (CenterZList_um.size() == Length);
    LengthsEqual &= (NameList.size() == Length);
    if (!LengthsEqual) {
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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
        int SphereID = Handle.Sim()->Collection.Geometries.size();
        S.ID = SphereID;
        Handle.Sim()->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::BulkCylinderCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "BulkCylinderCreate", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
     
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
    Util::GetFloatVector(&Point1RadiusList_um, &(Handle.ReqJSON()), "Point1RadiusList_um");
    Util::GetFloatVector(&Point2RadiusList_um, &(Handle.ReqJSON()), "Point2RadiusList_um");
    Util::GetFloatVector(&Point1XList_um, &(Handle.ReqJSON()), "Point1XList_um");
    Util::GetFloatVector(&Point1YList_um, &(Handle.ReqJSON()), "Point1YList_um");
    Util::GetFloatVector(&Point1ZList_um, &(Handle.ReqJSON()), "Point1ZList_um");
    Util::GetFloatVector(&Point2XList_um, &(Handle.ReqJSON()), "Point2XList_um");
    Util::GetFloatVector(&Point2YList_um, &(Handle.ReqJSON()), "Point2YList_um");
    Util::GetFloatVector(&Point2ZList_um, &(Handle.ReqJSON()), "Point2ZList_um");
    Util::GetStringVector(&NameList, &(Handle.ReqJSON()), "NameList");

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
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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
        int CylinderID = Handle.Sim()->Collection.Geometries.size();
        S.ID = CylinderID;
        Handle.Sim()->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::BulkBoxCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "BulkBoxCreate", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
 
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
    Util::GetFloatVector(&CenterXList_um, &(Handle.ReqJSON()), "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &(Handle.ReqJSON()), "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &(Handle.ReqJSON()), "CenterZList_um");
    Util::GetFloatVector(&DimensionsXList_um, &(Handle.ReqJSON()), "DimensionsXList_um");
    Util::GetFloatVector(&DimensionsYList_um, &(Handle.ReqJSON()), "DimensionsYList_um");
    Util::GetFloatVector(&DimensionsZList_um, &(Handle.ReqJSON()), "DimensionsZList_um");
    Util::GetFloatVector(&RotationXList_rad, &(Handle.ReqJSON()), "RotationXList_rad");
    Util::GetFloatVector(&RotationYList_rad, &(Handle.ReqJSON()), "RotationYList_rad");
    Util::GetFloatVector(&RotationZList_rad, &(Handle.ReqJSON()), "RotationZList_rad");
    Util::GetStringVector(&NameList, &(Handle.ReqJSON()), "NameList");

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
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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

        int BoxID = Handle.Sim()->Collection.Geometries.size();
        S.ID = BoxID;
        Handle.Sim()->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

std::string Manager::BulkBSCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task) {

    HandlerData Handle(this, _JSONRequest, "BulkBSCreate", called_by_manager_task);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Get Parameters
    std::vector<float> MembranePotentialList_mV;
    std::vector<float> SpikeThresholdList_mV;
    std::vector<float> DecayTimeList_ms;
    std::vector<float> RestingPotentialList_mV;
    std::vector<float> AfterHyperpolarizationAmplitudeList_mV;
    std::vector<std::string> NameList;
    std::vector<int> ShapeIDList;
    Util::GetFloatVector(&MembranePotentialList_mV, &(Handle.ReqJSON()), "MembranePotentialList_mV");
    Util::GetFloatVector(&SpikeThresholdList_mV, &(Handle.ReqJSON()), "SpikeThresholdList_mV");
    Util::GetFloatVector(&DecayTimeList_ms, &(Handle.ReqJSON()), "DecayTimeList_ms");
    Util::GetFloatVector(&RestingPotentialList_mV, &(Handle.ReqJSON()), "RestingPotentialList_mV");
    Util::GetFloatVector(&AfterHyperpolarizationAmplitudeList_mV, &(Handle.ReqJSON()), "AfterHyperpolarizationAmplitudeList_mV");
    Util::GetStringVector(&NameList, &(Handle.ReqJSON()), "NameList");
    Util::GetIntVector(&ShapeIDList, &(Handle.ReqJSON()), "ShapeIDList");

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
        return Handle.ErrResponse(API::bgStatusCode::bgStatusInvalidParametersPassed);
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
        int CompartmentID = Handle.Sim()->BSCompartments.size();
        S.ID = CompartmentID;
        Handle.Sim()->BSCompartments.push_back(S);

        CompartmentIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["CompartmentIDs"] = CompartmentIDs;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
}

// -----------------------------------------------------------------

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
