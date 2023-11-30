//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the SynTrQuantalRelease struct.
    Additional Notes: None
    Date Created: 2023-10-29
*/

#pragma once

#include <cassert>
#include <vector>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

/**
 * @brief Struct representing synaptic transmission quantal release.
 *        In CNS, the number of vesicles ready to fuse with membrane in the
 *        presence of Ca2+ and cause quantal release is typically between
 *        10-100. For cerebellar MF-GC connections, the number is 5-10, 1
 *        vesicle per synaptic contact.  The release probability is
 *        characteristically 0.5.
 *
 */
struct SynTrQuantalRelease {
    int NumQuantaRelease_T =
        10;                  //! Total number of quanta available for release.
    float ReleaseProb = 0.5; //! Release probability.

    //! Constructors
    SynTrQuantalRelease();
    SynTrQuantalRelease(int _NumQuantaRelease_T, float _ReleaseProb);

    float QuantalContent_m();
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
