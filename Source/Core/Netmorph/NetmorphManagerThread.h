//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines paremters for the Netmorph.
    Additional Notes: None
    Date Created: 2024-07-05
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

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphParameters.h>

#include <BG/Common/Logger/Logger.h>



namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief This function is intended to be run as a separate thread by the RPC call to start netmorph.
 * It will use the given params to run a netmorph sim, and then put the result in the _Params struct.
 * After once done, it will exit, and thus terminate the thread.
 * 
 * @param _Logger
 * @param _Params 
 * @return int 
 */
int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params);


}; // namespace Simulator
}; // namespace NES
}; // namespace BG
