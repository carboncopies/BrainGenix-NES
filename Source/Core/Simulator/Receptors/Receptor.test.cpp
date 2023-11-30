
//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Receptor struct.
    Additional Notes: None
    Date Created: 2023-10-17
*/

#include <cmath>

#include <gtest/gtest.h>

#include <Simulator/Receptors/Receptor.h>

/**
 * @brief Test class for unit tests for struct Receptor.
 *
 */

struct ReceptorTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Receptors::Receptor> testReceptor =
        nullptr;

    float tol = 1e-3;

    void SetUp() {
        testReceptor =
            std::make_unique<BG::NES::Simulator::Receptors::Receptor>();
        testReceptor->TauR_ms = 1.0;
        testReceptor->TauD_ms = 1.0;
        testReceptor->TauD2_ms = 1.0;
        testReceptor->TauD3_ms = 1.0;
        testReceptor->ANorm = 1.0;
    }

    void TearDown() { return; }
};

TEST_F(ReceptorTest, test_PostSynCurrent_pA_default) {
    testReceptor->Vm_mV = 50.0;
    testReceptor->ESyn_mV = 40.0;
    testReceptor->GSyn_pS = 100.0;

    float expectedPostSynCurrent_pA =
        testReceptor->GSyn_pS * (testReceptor->Vm_mV - testReceptor->ESyn_mV);
    ASSERT_NEAR(testReceptor->PostSynCurrent_pA(), expectedPostSynCurrent_pA,
                tol);
    ASSERT_NEAR(testReceptor->ISyn_pA, expectedPostSynCurrent_pA, tol);
}

TEST_F(ReceptorTest, test_Conductance_pS_default) {
    testReceptor->Vm_mV = 50.0;
    testReceptor->ESyn_mV = 40.0;
    testReceptor->ISyn_pA = 100.0;

    float expected =
        testReceptor->ISyn_pA / (testReceptor->Vm_mV - testReceptor->ESyn_mV);
    float got = 0.0;

    ASSERT_NEAR(testReceptor->Conductance_pS(), expected, tol);
    ASSERT_NEAR(testReceptor->GSyn_pS, expected, tol);

    // Check for zero division
    testReceptor->Vm_mV = testReceptor->ESyn_mV;
    try {
        got = testReceptor->Conductance_pS();
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.", ex.what());
    }
}

TEST_F(ReceptorTest, test_GSynT_pS_default) {
    // Single value of t_ms
    float t_ms = 0.1;
    float expected = 0.0;

    // Double-exponential
    expected = testReceptor->GSynT_pS_DoubleExponential(t_ms);
    ASSERT_NEAR(
        testReceptor->GSynT_pS(
            t_ms,
            BG::NES::Simulator::Receptors::Receptor::PSPType::DOUBLE_EXPONENT),
        expected, tol);

    // Multiexponential
    expected = testReceptor->GSynT_pS_MXH(t_ms);
    ASSERT_NEAR(
        testReceptor->GSynT_pS(
            t_ms, BG::NES::Simulator::Receptors::Receptor::PSPType::MXH),
        expected, tol);

    // Simple exponential
    expected = testReceptor->GSynT_pS_DecayZeroRiseTime(t_ms);
    ASSERT_NEAR(
        testReceptor->GSynT_pS(
            t_ms,
            BG::NES::Simulator::Receptors::Receptor::PSPType::SIMPLE_EXPONENT),
        expected, tol);

    // Alpha function
    expected = testReceptor->GSynT_pS_RiseDecay(t_ms);
    ASSERT_NEAR(
        testReceptor->GSynT_pS(
            t_ms,
            BG::NES::Simulator::Receptors::Receptor::PSPType::ALPHA_FUNCTION),
        expected, tol);

    // Array of t_ms
    std::vector<float> t_msVec = {0.1, 2.5, 3.2, 0.4, 1.2};
    std::vector<float> expectedVals;

    // Double-exponential
    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GSynT_pS_DoubleExponential(t_ms));
    ASSERT_EQ(
        testReceptor->GSynT_pS(
            t_msVec,
            BG::NES::Simulator::Receptors::Receptor::PSPType::DOUBLE_EXPONENT),
        expectedVals);

    // Multiexponential
    expectedVals.clear();
    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GSynT_pS_MXH(t_ms));
    ASSERT_EQ(
        testReceptor->GSynT_pS(
            t_msVec, BG::NES::Simulator::Receptors::Receptor::PSPType::MXH),
        expectedVals);

    // Simple exponential
    expectedVals.clear();
    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GSynT_pS_DecayZeroRiseTime(t_ms));
    ASSERT_EQ(
        testReceptor->GSynT_pS(
            t_msVec,
            BG::NES::Simulator::Receptors::Receptor::PSPType::SIMPLE_EXPONENT),
        expectedVals);

    // Alpha function
    expectedVals.clear();
    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GSynT_pS_RiseDecay(t_ms));
    ASSERT_EQ(
        testReceptor->GSynT_pS(
            t_msVec,
            BG::NES::Simulator::Receptors::Receptor::PSPType::ALPHA_FUNCTION),
        expectedVals);
}

TEST_F(ReceptorTest, test_GSynT_pS_DecayZeroRiseTime_default) {
    // Single value of t_ms
    float t_ms = 0.1;
    float expected =
        testReceptor->GPeak_pS * exp(-t_ms / testReceptor->TauD_ms);
    ASSERT_NEAR(testReceptor->GSynT_pS_DecayZeroRiseTime(t_ms), expected, tol);

    // Array of t_ms
    std::vector<float> t_msVec = {0.1, 2.5, 3.2, 0.4, 1.2};
    std::vector<float> expectedVals;
    std::vector<float> gotVals =
        testReceptor->GSynT_pS_DecayZeroRiseTime(t_msVec);

    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GPeak_pS *
                               exp(-t_ms / testReceptor->TauD_ms));
    ASSERT_EQ(expectedVals, gotVals);

    // TauD_ms is zero.
    testReceptor->TauD_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_DecayZeroRiseTime(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauD_ms).", ex.what());
    }
}

TEST_F(ReceptorTest, test_GSynT_pS_RiseDecay_default) {
    // Single value of t_ms
    float t_ms = 0.1;
    float expected = testReceptor->GPeak_pS * t_ms / testReceptor->TauR_ms *
                     exp(1.0 - t_ms / testReceptor->TauR_ms);
    ASSERT_NEAR(testReceptor->GSynT_pS_RiseDecay(t_ms), expected, tol);

    // Array of t_ms
    std::vector<float> t_msVec = {0.1, 2.5, 3.2, 0.4, 1.2};
    std::vector<float> expectedVals;
    std::vector<float> gotVals = testReceptor->GSynT_pS_RiseDecay(t_msVec);

    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GPeak_pS * t_ms /
                               testReceptor->TauR_ms *
                               exp(1.0 - t_ms / testReceptor->TauR_ms));
    ASSERT_EQ(expectedVals, gotVals);

    // TauR_ms is zero.
    testReceptor->TauR_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_RiseDecay(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauR_ms).", ex.what());
    }
}

TEST_F(ReceptorTest, test_GSynT_pS_DoubleExponential_default) {
    // Single value of t_ms
    float t_ms = 0.1;
    float expected = testReceptor->GPeak_pS *
                     (-exp(-t_ms / testReceptor->TauR_ms) +
                      exp(-t_ms / testReceptor->TauD_ms)) /
                     testReceptor->ANorm;
    ASSERT_NEAR(testReceptor->GSynT_pS_DoubleExponential(t_ms), expected, tol);

    // Array of t_ms
    std::vector<float> t_msVec = {0.1, 2.5, 3.2, 0.4, 1.2};
    std::vector<float> expectedVals;
    std::vector<float> gotVals =
        testReceptor->GSynT_pS_DoubleExponential(t_msVec);

    for (float t_ms : t_msVec)
        expectedVals.push_back(testReceptor->GPeak_pS *
                               (-exp(-t_ms / testReceptor->TauR_ms) +
                                exp(-t_ms / testReceptor->TauD_ms)) /
                               testReceptor->ANorm);
    ASSERT_EQ(expectedVals, gotVals);

    // TauR_ms is zero.
    float originalTauR_ms = testReceptor->TauR_ms;
    testReceptor->TauR_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_DoubleExponential(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauR_ms).", ex.what());
    }
    testReceptor->TauR_ms = originalTauR_ms;

    // TauD_ms is zero.
    testReceptor->TauD_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_DoubleExponential(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauD_ms).", ex.what());
    }
}

TEST_F(ReceptorTest, test_GSynT_pS_MXH_default) {
    // Single value of t_ms
    float t_ms = 0.1;
    float expected =
        testReceptor->GPeak_pS / testReceptor->ANorm *
        pow(testReceptor->X, 1.0 - exp(-t_ms / testReceptor->TauR_ms)) *
        testReceptor->D1 *
        (exp(-t_ms / testReceptor->TauD_ms) +
         exp(-t_ms / testReceptor->TauD2_ms) +
         exp(-t_ms / testReceptor->TauD3_ms));
    ASSERT_NEAR(testReceptor->GSynT_pS_MXH(t_ms), expected, tol);

    // Array of t_ms
    std::vector<float> t_msVec = {0.1, 2.5, 3.2, 0.4, 1.2};
    std::vector<float> expectedVals;
    std::vector<float> gotVals = testReceptor->GSynT_pS_MXH(t_msVec);

    for (float t_ms : t_msVec)
        expectedVals.push_back(
            testReceptor->GPeak_pS / testReceptor->ANorm *
            pow(testReceptor->X, 1.0 - exp(-t_ms / testReceptor->TauR_ms)) *
            testReceptor->D1 *
            (exp(-t_ms / testReceptor->TauD_ms) +
             exp(-t_ms / testReceptor->TauD2_ms) +
             exp(-t_ms / testReceptor->TauD3_ms)));
    ASSERT_EQ(expectedVals, gotVals);

    // TauR_ms is zero.
    float originalTauR_ms = testReceptor->TauR_ms;
    testReceptor->TauR_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_MXH(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauR_ms).", ex.what());
    }
    testReceptor->TauR_ms = originalTauR_ms;

    // TauD_ms is zero.
    float originalTauD_ms = testReceptor->TauD_ms;
    testReceptor->TauD_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_MXH(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauD_ms).", ex.what());
    }
    testReceptor->TauD_ms = originalTauD_ms;

    // TauD2_ms is zero.
    float originalTauD2_ms = testReceptor->TauD2_ms;
    testReceptor->TauD2_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_MXH(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauD2_ms).", ex.what());
    }
    testReceptor->TauD2_ms = originalTauD2_ms;

    // TauD3_ms is zero.
    testReceptor->TauD3_ms = 0.0;
    try {
        float calculated = testReceptor->GSynT_pS_MXH(t_ms);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero (TauD3_ms).", ex.what());
    }
}

TEST_F(ReceptorTest, test_FindANorm_default) {
    testReceptor->FindANorm();

    ASSERT_FALSE(testReceptor->ANorms.empty());
    ASSERT_FALSE(testReceptor->GDiffs.empty());
}
