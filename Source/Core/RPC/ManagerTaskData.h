//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides bgStatusCode.
    Additional Notes: None
    Date Created: 2024-02-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <memory>
#include <map>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")


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



/**
 * Process:
 * 1. Prepare the data that the Task will need.
 * 2. Launch the specific Task function within a std::thread, giving it a pointer to this struct.
 * 3. Call AddManagerTask with this struct.
 * 4. Use the Task ID to refer to the status and results of the Task, as stored in this struct.
 */
struct ManagerTaskData {
    // Must be set before launching Task:
    // Simulator::Manager& Man;
    std::string InputData;

    // Must be set before calling AddManagerTask:
    std::unique_ptr<std::thread> Task;

    // Is set within AddManagerTask:
    int ID = -1; 

    // Results set by the Task thread:
    ManagerTaskStatus Status = ManagerTaskStatus::Active;
    nlohmann::json OutputData;
    int ReplaceSimulationID = -1;

    ManagerTaskData() {}

    void SetStatus(ManagerTaskStatus _status) { Status = _status; }

    void IncludeStatusInOutputData() { OutputData["TaskStatus"] = int(Status); }

    bool HasReplacementSimID() { return ReplaceSimulationID >= 0; }
};



}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG
