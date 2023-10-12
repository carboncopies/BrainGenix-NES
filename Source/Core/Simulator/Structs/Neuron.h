//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2023-10-09
*/

#pragma once

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

/**
 * @brief This struct provides the base struct for all neurons.
 * 
 */
struct Neuron {

    int ID; /**ID of the Neuron */

    //! Returns the time since the action potential threshold was
    //! crossed last.
    virtual float DtAct_ms(float t_ms) = 0;

    //! Tells if the action potential threshold has been crossed.
    virtual bool HasSpiked() = 0;
};

}; // Close Namespace CoreStructs
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
