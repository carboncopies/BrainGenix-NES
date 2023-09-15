//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering system interface code.
    Additional Notes: None
    Date Created: 2023-09-15
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
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief Rendering system interface for NES. This system is used to generate virtual scan data from models.
 */
class Interface {

private: 

    BG::Common::Logger::LoggingSystem *Logger_; /**Logging Class Pointer*/

    bool EnableDebugWindow_ = false; /**Window For Debugging Purposes - should be disabled in most cases.*/

public:

    /**
     * @brief Constructor for the rendering system.
     * Use the EnableWindow param for debugging by shwoing an SDL window containing the to-be-rendered content.
     * Note that use of this parameter is for debugging only and requires having either an X-Server or Wayland installed.
    */
    Interface(BG::Common::Logger::LoggingSystem* _Logger, bool _EnableDebugWindow = true);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
    */
    ~Interface();


};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG