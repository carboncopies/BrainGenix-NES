//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines paremters for the Netmorph.
    Additional Notes: None
    Date Created: 2024-02-07
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <cstdint>
#include <vector>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/maths/vec3.h>

#include <Include/Netmorph.h>

// Internal Libraries (BG convention: use <> instead of "")
//#include <Simulator/Structs/Simulation.h>

namespace BG {
namespace NES {
namespace Simulator {

// forward declaration:
struct Simulation;

enum NetmorphState { Netmorph_NONE, Netmorph_REQUESTED, Netmorph_WORKING, Netmorph_DONE };

/**
 * @brief Selectively buffers output generated by Netmorph so that, in addition to
 * pushing it to a log-level of NES Logging, it can also be returned to the
 * front-end through the API.
 * 
 * Note that this object is created as part of Nm2NESLogging in NetmorphManagerThread.cpp.
 */
class BufferNetmorphOutput {
public:
    bool log_error = false;
    bool log_warning = false;
    bool log_report = false;
    bool log_progress = false;
    std::string error_log;
    std::string warning_log;
    std::string report_log;
    std::string progress_log;
public:
    void SetLogBuffers(const std::string& _logbuffers) {
        log_error = _logbuffers.find("error") != std::string::npos;
        log_warning = _logbuffers.find("warning") != std::string::npos;
        log_report = _logbuffers.find("report") != std::string::npos;
        log_progress = _logbuffers.find("progress") != std::string::npos;
    }
    void LogError(const std::string& msg) {
        error_log += msg;
    }
    void LogWarning(const std::string& msg) {
        warning_log += msg;
    }
    void LogReport(const std::string& msg) {
        report_log += msg;
    }
    void LogProgress(const std::string& msg) {
        std::cout << "DEBUG LogProgress: " << msg << '\n';
        progress_log += msg;
    }
};

/**
 * @brief Struct containing various operation parameters used for netmorph.
 * A worker thread monitors the state attribute of this struct.
 * 
 */
struct NetmorphParameters {

    // *** We don't really need this, because we can give the model content directly.
    //std::string ModelFile; /**String containing the magic modelfile that netmorph needs to do stuff. See the netmorph manual for how to do that, cause IDK*/

    // BAD BAD BAD BAD
    Simulation* Sim; // This is terrible and we should NEVER EVER DO THIS AGAIN!!!!!!
    // This struct is included in simulation, and we include simulation in here - BAD!!!

    std::string ModelContent; /**String containing the Netmorph model.*/

    std::string LogBuffers; // A string such as "error,warning,report,progress".
    BufferNetmorphOutput* BufferedOutput = nullptr; /**Use this to retrieve buffered Netmorph logging output.*/

    int Progress_percent; /**Netmorph status percent*/

    Netmorph Result; /**Output provided by netmorph written to this struct*/

    NetmorphState State = Netmorph_NONE; /**Sets the state of this Netmorph task*/

};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
