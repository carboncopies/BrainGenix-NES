
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
    std::unique_ptr<BG::NES::Simulator::Receptors::AMPAReceptor>
        testAMPAReceptor = nullptr;

    float tol = 1e-3;

    void SetUp() {
        testAMPAReceptor =
            std::make_unique<BG::NES::Simulator::Receptors::AMPAReceptor>();
    }

    void TearDown() { return; }
};

TEST_F(AMPAReceptorTest, test_PostSynCurrent_pA_default) {
    float expectedPostSynCurrent_pA =
        testAMPAReceptor->GSyn_pS *
        (testAMPAReceptor->Vm_mV - testAMPAReceptor->ESyn_mV);
    ASSERT_NEAR(testAMPAReceptor->PostSynCurrent_pA(),
                expectedPostSynCurrent_pA, tol);
}

TEST_F(AMPAReceptorTest, test_Conductance_pS_default) {
    float expected = testAMPAReceptor->ISyn_pA /
                     (testAMPAReceptor->Vm_mV - testAMPAReceptor->ESyn_mV);
    float got = 0.0;

    ASSERT_NEAR(testAMPAReceptor->Conductance_pS(), expected, tol);
    ASSERT_NEAR(testAMPAReceptor->GSyn_pS, expected, tol);

    // Check for zero division
    testAMPAReceptor->Vm_mV = testAMPAReceptor->ESyn_mV;
    try {
        got = testAMPAReceptor->Conductance_pS();
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.", ex.what());
    }
}
