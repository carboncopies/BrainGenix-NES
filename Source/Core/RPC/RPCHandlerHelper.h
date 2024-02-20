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

#include <Simulator/Structs/Simulation.h>

// #include <RPC/ManagerTaskData.h>
#include <RPC/APIStatusCode.h>


namespace BG {
namespace NES {
namespace API {


typedef std::vector<std::unique_ptr<Simulator::Simulation>>* Simulations;




// Handy class for standard handler data.
class HandlerData {

protected:
    // ManagerTaskData* ManTaskData;
    Simulations SimVec = nullptr;

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/

    std::string JSONRequestStr;
    std::string RoutePath_; /**Path that is this route*/
    nlohmann::json RequestJSON;
    BGStatusCode Status = BGStatusSuccess;

    int SimulationID = -1;
    Simulator::Simulation* ThisSimulation = nullptr;

public:
    HandlerData(const std::string& _JSONRequest, BG::Common::Logger::LoggingSystem* _Logger, std::string _RoutePath, Simulations _Simulations, bool PermitBusy = false, bool NoSimulation = false);

    // See how this is used in Manager::SimulationCreate().
    // Simulator::Simulation* NewSimulation();

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
    std::string ResponseAndStoreRequest(nlohmann::json& ResponseJSON, bool store = true);
    std::string ErrResponse(int _Status);
    std::string ErrResponse(BGStatusCode _Status);
    std::string ErrResponse();

    std::string ResponseWithID(const std::string& IDName, int IDValue);
    std::string ResponseWithID(const std::string& IDName, const std::string& IDValue);
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