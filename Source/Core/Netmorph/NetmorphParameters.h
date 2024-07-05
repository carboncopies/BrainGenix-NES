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

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/maths/vec3.h>

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace Simulator {


enum NetmorphState { Netmorph_NONE, Netmorph_REQUESTED, Netmorph_WORKING, Netmorph_DONE };


/**
 * @brief Struct containing various operation parameters used for netmorph.
 * A worker thread monitors the state attribute of this struct.
 * 
 */
struct NetmorphParameters {

    std::string ModelFile; /**String containing the magic modelfile that netmorph needs to do stuff. See the netmorph manual for how to do that, cause IDK*/

    int Progress_percent; /**Netmorph status percent*/

    NetmorphState State = Netmorph_NONE; /**Sets the state of this Netmorph task*/

};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
