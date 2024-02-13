//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2024-02-13
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <Simulator/Manager.h>
#include <RPC/APIStatusCode.h>


namespace BG {
namespace NES {
namespace API {


enum ManagerTaskStatus {
    Success = 0, // Completed successfully.
    Active = 1,  // Freshly created and/or actively operating.
    TimeOut = 2, // Failed due to time-out.
    GeneralFailure = 3,
    NUMManagerTaskStatus
};


// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.

typedef std::string NESRequest_func_t(Simulator::Manager&, const nlohmann::json&, ManagerTaskData*);

struct RouteAndHandler {
    std::string Route;
    NESRequest_func_t* Handler;

    RouteAndHandler(const std::string& _Route, NESRequest_func_t* _Handler): Route(_Route), Handler(_Handler) {}
};



typedef std::map<std::string, RouteAndHandler> NESRequest_map_t;


/**
 * Process:
 * 1. Prepare the data that the Task will need.
 * 2. Launch the specific Task function within a std::thread, giving it a pointer to this struct.
 * 3. Call AddManagerTask with this struct.
 * 4. Use the Task ID to refer to the status and results of the Task, as stored in this struct.
 */
struct ManagerTaskData {
    // Must be set before launching Task:
    Simulator::Manager& Man;
    std::string InputData;

    // Must be set before calling AddManagerTask:
    std::unique_ptr<std::thread> Task;

    // Is set within AddManagerTask:
    int ID = -1; 

    // Results set by the Task thread:
    ManagerTaskStatus Status = ManagerTaskStatus::Active;
    nlohmann::json OutputData;
    int ReplaceSimulationID = -1;

    ManagerTaskData(Simulator::Manager& _Man): Man(_Man) {}

    void SetStatus(ManagerTaskStatus _status) { Status = _status; }

    void IncludeStatusInOutputData() { OutputData["TaskStatus"] = int(Status); }

    bool HasReplacementSimID() { return ReplaceSimulationID >= 0; }
};



// Handy class for standard handler data.
class HandlerData {
protected:
    Simulator::Manager& Man;
    std::string Source;
    ManagerTaskData* ManTaskData;
    std::vector<std::unique_ptr<Simulator::Simulation>>* SimVec = nullptr;

    std::string JSONRequestStr;
    nlohmann::json RequestJSON;
    BGStatusCode Status = BGStatusSuccess;

    int SimulationID = -1;
    Simulator::Simulation* ThisSimulation = nullptr;

public:
    HandlerData(Simulator::Manager* _Man, const std::string& _JSONRequest, const std::string& _Source, ManagerTaskData* called_by_manager_task, bool PermitBusy = false, bool NoSimulation = false);

    // See how this is used in Manager::SimulationCreate().
    Simulator::Simulation* NewSimulation();

    bool HasError() const;

    BGStatusCode GetStatus() const;

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
    std::string ResponseAndStoreRequest(nlohmann::json& ResponseJSON, NESRequest_map_t _RH, bool store = true);
    std::string ErrResponse(int _Status, NESRequest_map_t _RH);
    std::string ErrResponse(BGStatusCode _Status);
    std::string ErrResponse();

    std::string ResponseWithID(const std::string& IDName, int IDValue, NESRequest_map_t _RH);
    std::string ResponseWithID(const std::string& IDName, const std::string& IDValue, NESRequest_map_t _RH);
    std::string StringResponse(std::string _Key, std::string _Value);

    int SimID() const;
    std::string SimIDStr() const;

    Simulator::Simulation* Sim() const;

    const nlohmann::json& ReqJSON() const;

    bool FindPar(const std::string& ParName, nlohmann::json::iterator& Iterator, nlohmann::json& _JSON);
    bool FindPar(const std::string& ParName, nlohmann::json::iterator& Iterator);

    bool GetParBool(const std::string& ParName, bool& Value, nlohmann::json& _JSON);
    bool GetParBool(const std::string& ParName, bool& Value);

    bool GetParInt(const std::string& ParName, int& Value, nlohmann::json& _JSON);
    bool GetParInt(const std::string& ParName, int& Value);

    bool GetParFloat(const std::string& ParName, float& Value, nlohmann::json& _JSON);
    bool GetParFloat(const std::string& ParName, float& Value);

    bool GetParString(const std::string& ParName, std::string& Value, nlohmann::json& _JSON);
    bool GetParString(const std::string& ParName, std::string& Value);

    bool GetParVec3FromJSON(const std::string& ParName, Simulator::Geometries::Vec3D& Value, nlohmann::json& _JSON, const std::string& Units = "um");

    bool GetParVec3(const std::string& ParName, Simulator::Geometries::Vec3D& Value, const std::string& Units = "um");

    bool GetParVecInt(const std::string& ParName, std::vector<int>& Value, nlohmann::json& _JSON);
    bool GetParVecInt(const std::string& ParName, std::vector<int>& Value);


    bool GetParVecFloat(const std::string& ParName, std::vector<float>& Value, nlohmann::json& _JSON);
    bool GetParVecFloat(const std::string& ParName, std::vector<float>& Value);

};


}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG