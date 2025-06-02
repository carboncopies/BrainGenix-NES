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
 * 
 * This struct models the quantal release of neurotransmitters at synapses.
 * In the central nervous system (CNS), the number of vesicles ready to fuse
 * with the membrane in the presence of Ca2+ and cause quantal release is
 * typically between 10-100. For cerebellar MF-GC connections, the number is
 * 5-10, with 1 vesicle per synaptic contact. The release probability is
 * characteristically 0.5.
 */
struct SynTrQuantalRelease {
    int NumQuantaRelease_T = 10; /**< Total number of quanta available for release. */
    float ReleaseProb = 0.5; /**< Probability of release for each quantum. */

    /**
     * @brief Default constructor initializing default values.
     */
    SynTrQuantalRelease();

    /**
     * @brief Parameterized constructor to initialize the number of quanta and release probability.
     * 
     * @param _NumQuantaRelease_T Total number of quanta available for release.
     * @param _ReleaseProb Probability of release for each quantum.
     */
    SynTrQuantalRelease(int _NumQuantaRelease_T, float _ReleaseProb);

    /**
     * @brief Calculates the quantal content (mean number of quanta released).
     * 
     * @return float Mean quantal content.
     */
    float QuantalContent_m();
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
