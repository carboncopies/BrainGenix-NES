
//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the NMDAReceptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/
#include <gtest/gtest.h>

#include <Simulator/Receptors/NMDAReceptor.h>

/**
 * @brief Test class for unit tests for struct NMDAReceptor.
 *
 */

struct NMDAReceptorTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Receptors::NMDAReceptor>
        testNMDAReceptor = nullptr;

    float tol = 1e-3;

    void SetUp() {
        testNMDAReceptor =
            std::make_unique<BG::NES::Simulator::Receptors::NMDAReceptor>();
    }

    void TearDown() { return; }
};

TEST_F(NMDAReceptorTest, test__default) {}
