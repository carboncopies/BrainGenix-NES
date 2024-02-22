//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the receptor struct.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Connections {

/**
 * @brief This struct provides the data storage for the staple connection
 *
 */
struct Receptor {

    std::string Name;                  /**Name of the Staple connection*/
    int ID = -1;                       /**ID of the Staple connection*/
    int ShapeID = -1;                  /**ID of the shape that represents this receptor*/
    
    int SourceCompartmentID = -1;      /**ID of the compartment whos data will be copied
                                          to the destination.*/
    int DestinationCompartmentID = -1; /**ID of the compartment whos data will be
                                          overwritten with the source.*/

    float Conductance_nS = 0.0;        /**Conductance from source to destination in
                                          nanoSiemens.*/
    float TimeConstantRise_ms = 1.0;   /**Postsynaptic potential rise time constant in milliseconds*/
    float TimeConstantDecay_ms = 1.0;  /**Postsynaptic potential decay time constant in milliseconds*/
    //Geometries::Vec3D ReceptorPos_um;  /**Position of the receptor in world
    //                                      space, in micrometers*/
};

}; // namespace Connections
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
