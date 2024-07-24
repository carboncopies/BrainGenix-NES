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
#include <sstream>
#include <cstring>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Connections {

#define NeurotransmitterLEN 12

struct ReceptorBase {
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

    char Neurotransmitter[NeurotransmitterLEN] = { 0 };   /**Neurotransmitter type, e.g. AMPA, GABA*/

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nShapeID: " << ShapeID;
        ss << "\nSourceCompartmentID: " << SourceCompartmentID;
        ss << "\nDestinationCompartmentID: " << DestinationCompartmentID;
        ss << "\nConductance_nS: " << Conductance_nS;
        ss << "\nTimeConstantRise_ms: " << TimeConstantRise_ms;
        ss << "\nTimeConstantDecay_ms: " << TimeConstantDecay_ms;
        ss << "\nNeurotransmitter: " << Neurotransmitter;
        return ss.str();
    }

    void safeset_Neurotransmitter(const char* s) {
        strncpy(Neurotransmitter, s, NeurotransmitterLEN-1);
        Neurotransmitter[NeurotransmitterLEN-1] = '\0';
    }
};

/**
 * @brief This struct provides the data storage for the staple connection
 *
 */
struct Receptor: public ReceptorBase {
    std::string Name;                  /**Name of the Staple connection*/

    Receptor() {}
    Receptor(const ReceptorBase& _Base): ReceptorBase(_Base) {}
};

}; // namespace Connections
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
