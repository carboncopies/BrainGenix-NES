//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the SynTrQuantalRelease
   struct. Additional Notes: None Date Created: 2023-10-29
*/

#include <gtest/gtest.h>

#include <Simulator/Structs/SynTrQuantalRelease.h>

/**
 * @brief Test class for unit tests for struct SynTrQuantalRelease.
 *
 */

struct SynTrQuantalReleaseTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Tools::SynTrQuantalRelease>
        testSynTrQuantalRelease = nullptr;
    float tol = 1e-3;

    void SetUp() {

        testSynTrQuantalRelease =
            std::make_unique<BG::NES::Simulator::Tools::SynTrQuantalRelease>();
    }

    void TearDown() { return; }
};

TEST_F(SynTrQuantalReleaseTest, test_QuantalContent_m_default) {
    float expected = testSynTrQuantalRelease->NumQuantaRelease_T *
                     testSynTrQuantalRelease->ReleaseProb;
    ASSERT_NEAR(expected, testSynTrQuantalRelease->QuantalContent_m(), tol);
}
