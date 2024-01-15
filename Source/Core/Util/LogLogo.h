//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2024-01-15
    Author(s): Thomas Liao, Jacob Morrison


    Copyright (C) 2024  Thomas Liao

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
#include <string>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Util {




/**
 * @brief Prints the BrainGenix ASCII art logo to the terminal.
 *
 * The LogLogo function is responsible for displaying the BrainGenix ASCII art logo in the terminal.
 * It takes a pointer to the logging system instance to log the logo.
 *
 * @param _Logger Pointer to the logging system instance.
 */
void LogLogo(BG::Common::Logger::LoggingSystem* _Logger);



}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG
