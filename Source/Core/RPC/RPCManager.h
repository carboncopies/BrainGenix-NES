//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/StaticRoutes.h>
#include <RPC/RouteAndHandler.h>

#include <BG/Common/Logger/Logger.h>

#include <Config/Config.h>



namespace BG {
namespace NES {
namespace API {

/**
 * @brief Manages the NES remote procedure call (RPC) host.
 *
 * The RPCManager class owns the RPC server and handles its initialization and destruction at the end of the program's run.
 * It also takes a copy of the systemwide configuration struct to configure the RPC server (e.g., host and port).
 */
class RPCManager {

private:

    Config::Config* Config_; /**Pointer to configuration struct owned by rest of system*/
    std::unique_ptr<rpc::server> RPCServer_; /**Instance of RPC Server from rpclib*/
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/



    std::map<std::string, std::function<std::string(std::string _JSONRequest)>> RequestHandlers_;// = {
        // {"SimulationCreate", {"Simulation/Create", SimulationCreateHandler} },
        // {"SimulationReset", {"Simulation/Reset", SimulationResetHandler} },
        // {"SimulationRunFor", {"Simulation/RunFor", SimulationRunForHandler} },
        // {"SimulationRecordAll", {"Simulation/RecordAll", SimulationRecordAllHandler} },
        // {"SimulationGetRecording", {"Simulation/GetRecording", SimulationGetRecordingHandler} },
        // {"SimulationGetStatus", {"Simulation/GetStatus", SimulationGetStatusHandler} },
        // {"SimulationBuildMesh", {"Simulation/BuildMesh", SimulationBuildMeshHandler} },
        // {"SimulationSave", {"Simulation/Save", SimulationSaveHandler} },
        // {"SimulationLoad", {"Simulation/Load", SimulationLoadHandler} },
        // {"SimulationGetGeoCenter", {"", SimulationGetGeoCenterHandler} },

        // {"SphereCreate", {"Geometry/Shape/Sphere/Create", SphereCreateHandler} },
        // {"BulkSphereCreate", {"Geometry/Shape/Sphere/BulkCreate", nullptr} },
        // {"CylinderCreate", {"Geometry/Shape/Cylinder/Create", CylinderCreateHandler} },
        // {"BulkCylinderCreate", {"Geometry/Shape/Cylinder/BulkCreate", nullptr} },
        // {"BoxCreate", {"Geometry/Shape/Box/Create", BoxCreateHandler} },
        // {"BulkBoxCreate", {"Geometry/Shape/Box/BulkCreate", nullptr}},

        // {"BSCreate", {"Compartment/BS/Create", BSCreateHandler} },
        // {"BulkBSCreate", {"Compartment/BS/BulkCreate", nullptr}},

        // {"StapleCreate", {"Connection/Staple/Create", StapleCreateHandler} },
        // {"ReceptorCreate", {"Connection/Receptor/Create", ReceptorCreateHandler} },

        // {"BSNeuronCreate", {"Neuron/BSNeuron/Create", BSNeuronCreateHandler} },

        // {"PatchClampDACCreate", {"Tool/PatchClampDAC/Create", PatchClampDACCreateHandler} },
        // {"PatchClampDACSetOutputList", {"Tool/PatchClampDAC/SetOutputList", PatchClampDACSetOutputListHandler} },

        // {"PatchClampADCCreate", {"Tool/PatchClampADC/Create", PatchClampADCCreateHandler} },
        // {"PatchClampADCSetSampleRate", {"Tool/PatchClampADC/SetSampleRate", PatchClampADCSetSampleRateHandler} },
        // {"PatchClampADCGetRecordedData", {"Tool/PatchClampADC/GetRecordedData", PatchClampADCGetRecordedDataHandler} },

        // {"SetSpecificAPTimes", {"", SetSpecificAPTimesHandler} },
        // {"SetSpontaneousActivity", {"", SetSpontaneousActivityHandler} },

        // {"AttachRecordingElectrodes", {"", AttachRecordingElectrodesHandler} },
        // {"CalciumImagingAttach", {"", CalciumImagingAttachHandler} },
        // {"CalciumImagingShowVoxels", {"", CalciumImagingShowVoxelsHandler} },
        // {"CalciumImagingRecordAposteriori", {"", CalciumImagingRecordAposterioriHandler} },
        // {"SetRecordInstruments", {"", SetRecordInstrumentsHandler} },
        // {"GetInstrumentRecordings", {"", GetInstrumentRecordingsHandler} },

        // {"NESRequest", {"NES", nullptr}},

        // {"ManTaskStatus", {"ManTaskStatus", ManTaskStatusHandler}},

        // {"VisualizerGenerateImage", {"VisualizerGenerateImage", VisualizerGenerateImage}},
        // {"VisualizerGetImageHandles", {"VisualizerGetImageHandles",VisualizerGetImageHandles}},
        // {"VisualizerGetImage", {"VisualizerGetImage", VisualizerGetImage}},
        // {"VisualizerGetStatus", {"VisualizerGetStatus", VisualizerGetStatus}}


    //};


   /**
     * @brief Adds the given route 
     * 
     * @param _RouteName
     * @param _Handler 
     */
    void AddRequestHandler(std::string _RouteName, RouteAndHandler _Handler);



public:

    /**
     * @brief Construct a new RPCManager object
     * Give this a pointer to an initialized configuration object.
     * Also it takes a pointer to the logging system instance.
     * 
     * @param _Config 
     * @param _Logger
     */
    RPCManager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger);


    /**
     * @brief Destroy the RPCManager object
     * 
     */
    ~RPCManager();



 
    /**
     * @brief Adds a route to the NES RPC Handler.
     * 
     * @param _RouteHandle 
     * @param _Function 
     */
    void AddRoute(std::string _RouteHandle, std::function<std::string(std::string _JSONRequest)> _Function);

    


    std::string NESRequest(std::string _JSONRequest, int _SimulationIDOverride = -1); // Generic JSON-based NES requests.


    /**
     * @brief Registers a callback to the API service.
     *
     * Registers a callback function for a specific route. Assumes the callback function may be accessed from any thread.
     *
     * @param _RouteName Name of the route to register.
     * @param _Logger Pointer to the logging system instance.
     * @param _CallbackFunction Callback function to be registered.
     */
    template <typename F> void AddRoute(std::string _RouteName, BG::Common::Logger::LoggingSystem* _Logger, F _CallbackFunction) {
        std::string LogMsg = "Registering Callback For Route " + _RouteName;
        _Logger->Log(LogMsg, 0);
        RPCServer_->bind(_RouteName.c_str(), _CallbackFunction);
    }

};

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG