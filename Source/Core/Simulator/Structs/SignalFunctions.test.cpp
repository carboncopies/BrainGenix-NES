//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the functions in the
    SignalFunction namespace.
    Additional Notes: None
    Date Created: 2023-10-13
*/
#include <cmath>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <Simulator/Structs/SignalFunctions.h>

/**
 * @brief Test class for unit tests for functions in the namespace
 * SignalFunctions.
 *
 */

struct SignalFunctionsTest : testing::Test {
    float tol = 1e-3;

    void SetUp() {}

    void TearDown() { return; }
};

TEST_F(SignalFunctionsTest, test_DoubleExponentExpr_default) {
    float amp = 1.1;
    float tauRise = 0.5;
    float tauDecay = 0.1;
    float tDiff = 0.0, expected = 0.0;

    // Test for negative tdiff
    tDiff = -0.5;
    ASSERT_NEAR(BG::NES::Simulator::SignalFunctions::DoubleExponentExpr(
                    amp, tauRise, tauDecay, tDiff),
                0.0, tol);

    // Test for non-negative tdiff
    tDiff = 0.5;
    expected = amp * (exp(-tDiff / tauRise) + exp(-tDiff / tauDecay));
    ASSERT_NEAR(BG::NES::Simulator::SignalFunctions::DoubleExponentExpr(
                    amp, tauRise, tauDecay, tDiff),
                expected, tol);
}

TEST_F(SignalFunctionsTest, test_Convolve1D_default) {
    std::deque<float> signal = {0.1, 2.0, 1.2, 2.3, 0.5,
                                 0.7, 3.0, 0.2, 0.9, 2.1};
    std::vector<float> reversed_kernel = {-1.5, -1.0, 0.5, 1.0};
    std::vector<float> expected = {0.1,   2.05,  2.1,  0.75,  -2.55,
                                   -3.15, -0.6,  0.25, -3.05, -2.15,
                                   -0.15, -3.45, -3.15};

    std::vector<float> result(signal.size()+reversed_kernel.size()-1, 0.0);

    BG::NES::Simulator::SignalFunctions::Convolve1D(signal, reversed_kernel, result);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i)
        ASSERT_NEAR(result[i], expected[i], tol)
            << "i = " << i << " result[i] = " << result[i];
}
