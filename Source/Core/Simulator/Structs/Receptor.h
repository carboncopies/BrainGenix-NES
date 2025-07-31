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

enum NeurotransmitterType: int {
    AMPA = 0,
    GABA = 1,
    NMDA = 2,
    NUMNeurotransmitterType = 3
};

struct ReceptorBase {
    int ID = -1;                       /**ID of the connection*/
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

    char Neurotransmitter[NeurotransmitterLEN] = { 0 };   /**Neurotransmitter type, e.g. AMPA, GABA, NMDA*/

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
 * @brief This struct provides the data storage for the connection
 *
 */
struct Receptor: public ReceptorBase {
    std::string Name;                  /**Name of the connection*/

    Receptor() {}
    Receptor(const ReceptorBase& _Base): ReceptorBase(_Base) {}
};

extern float compute_normalization(float tau_rise, float tau_decay);

enum LIFCSTDPMethodEnum: int {
    STDPNONE = 0,
    STDPHEBBIAN = 1,
    STDPANTIHEBBIAN = 2,
    NUMLIFCSTDPMethodEnum = 3
};

struct LIFCReceptorBase {
    int ID = -1;                       /**ID of the connection*/
    int ShapeID = -1;                  /**ID of the shape that represents this receptor*/
    
    int SourceCompartmentID = -1;      /**ID of the compartment whos data will be copied
                                          to the destination.*/
    int DestinationCompartmentID = -1; /**ID of the compartment whos data will be
                                          overwritten with the source.*/

    float ReversalPotential_mV = 0.0;
    float PSPRise_ms = 1.0;
    float PSPDecay_ms = 1.0;
    float PeakConductance_nS = 0.0;
    float Weight = 0.0;
    float OnsetDelay_ms = 0.0;
    bool voltage_gated = false;

    LIFCSTDPMethodEnum STDP_Method = STDPNONE;
    float STDP_A_pos = 0.0;
    float STDP_A_neg = 0.0;
    float STDP_Tau_pos = 0.0;
    float STDP_Tau_neg = 0.0;

    NeurotransmitterType Neurotransmitter = AMPA;

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nShapeID: " << ShapeID;
        ss << "\nSourceCompartmentID: " << SourceCompartmentID;
        ss << "\nDestinationCompartmentID: " << DestinationCompartmentID;
        ss << "\nReversalPotential_mV: " << ReversalPotential_mV; 
        ss << "\nPSPRise_ms: " << PSPRise_ms;
        ss << "\nPSPDecay_ms: " << PSPDecay_ms;
        ss << "\nPeakConductance_nS: " << PeakConductance_nS;
        ss << "\nWeight: " << Weight;
        ss << "\nOnsetDelay_ms: " << OnsetDelay_ms;
        ss << "\nvoltage_gated: " << voltage_gated;
        ss << "\nSTDP_Method: " << STDP_Method;
        ss << "\nSTDP_A_pos: " << STDP_A_pos;
        ss << "\nSTDP_A_neg: " << STDP_A_neg;
        ss << "\nSTDP_Tau_pos: " << STDP_Tau_pos;
        ss << "\nSTDP_Tau_neg: " << STDP_Tau_neg;
        ss << "\nNeurotransmitter: " << Neurotransmitter;
        return ss.str();
    }

    // void safeset_Neurotransmitter(const char* s) {
    //     strncpy(Neurotransmitter, s, NeurotransmitterLEN-1);
    //     Neurotransmitter[NeurotransmitterLEN-1] = '\0';
    // }

};

struct LIFCReceptor {
    std::string Name;                  /**Name of the connection*/

    LIFCReceptor() {}
    LIFCReceptor(const LIFCReceptorBase& _Base): LIFCReceptorBase(_Base) {}
};

}; // namespace Connections
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
