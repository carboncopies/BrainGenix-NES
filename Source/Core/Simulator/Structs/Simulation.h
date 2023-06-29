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


enum SimulationActions {
    SIMULATION_RESET,
    SIMULATION_RUNFOR
};


/**
 * @brief Name of the simulation
 * 
 */
struct Simulation {

    std::string Name; /**Name of the simulation*/
    int ID; /**ID of the simulation*/
    float MaxRecordTime_ms = 0.; /**Maximum god-mode record time for everything*/


    std::vector<float> RunTimes_ms; /**List containing number of milliseconds to run the simulation for next time SIMULATION_RUNFOR is called, removes from start of list every time.*/
    std::vector<SimulationActions> ProcessingQueue; /**List of tasks that need to be processed on this simulation, could be run for, or reset, etc. See above enum for more info.*/


    Shapes::Shapes Shapes; /**Instance of shape struct containing all shapes in this simulation*/


    std::vector<Compartments::BS> BSCompartments; /**This will need to be updated later to a std::variant type, but for now it stores the only type of supported compartments, BallStick type*/

    std::vector<Connections::Staple> Staples; /**List of staple connections, index is their id (also stored in struct)*/
    std::vector<Connections::Receptor> Receptors; /**List of receptor connections, index is their id (and it's also stored in the struct itself)*/

    std::vector<Tools::PatchClampDAC> PatchClampDACs; /**List of patchclamp dacs, id is index*/
    std::vector<Tools::PatchClampADC> PatchClampADCs; /**List of patchclamp adcs, id is index*/


};


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG