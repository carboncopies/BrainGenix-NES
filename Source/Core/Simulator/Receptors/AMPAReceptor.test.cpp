
//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the AMPAReceptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#include <gtest/gtest.h>

#include <Simulator/Receptors/AMPAReceptor.h>

/**
 * @brief Test class for unit tests for struct AMPAReceptor.
 *
 */

struct AMPAReceptorTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Receptors::AMPAReceptor> testReceptor =
        nullptr;

    float tol = 1e-3;

    void SetUp() {
        testReceptor =
            std::make_unique<BG::NES::Simulator::Receptors::AMPAReceptor>();
    }

    void TearDown() { return; }
};
