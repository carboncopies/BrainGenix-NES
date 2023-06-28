//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Shapes.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/Staple.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/PatchClampDAC.h>
#include <Simulator/Structs/PatchClampADC.h>



namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief Name of the simulation
 * 
 */
struct Simulation {

    std::string Name; /**Name of the simulation*/
    int ID; /**ID of the simulation*/

    Shapes::Shapes Shapes;
    std::vector<Compartments::BS> BSCompartments; /**This will need to be updated later to a std::variant type, but for now it stores the only type of supported compartments, BallStick type*/

};


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG