#//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Receptor abstract struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#pragma once

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

/**
 * @brief This struct defines the Receptor abstract struct, the base struct for
 * all receptor classes which are basic models of synaptic transmission.
 * Compare with: Rothman, J.S. and Silver, R.A. (2016), "Data-Driven Modeling
 * of Synaptic Transmission and Integration", Prog.Mol.Biol.Transl.Sci.
 * https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4748401/
 */

struct Receptor {
    //! Type of post-synaptic potential used in the receptor.
    enum PSPType { DOUBLE_EXPONENT = 0, MXH, SIMPLE_EXPONENT, ALPHA_FUNCTION };
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
