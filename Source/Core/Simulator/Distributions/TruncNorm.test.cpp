//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the TruncNorm struct.
    Additional Notes: None
    Date Created: 2023-10-10
*/

#include <cstdint>
#include <vector>
#include <tuple>

#include <gtest/gtest.h>

#include <Simulator/Distributions/TruncNorm.h>


/**
 * @brief Test class for unit tests for struct TruncNorm.
 *
 */

struct TruncNormTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::Distributions::TruncNorm> testTruncNorm = nullptr;
    
    uint32_t seed = 43;
    float tol = 1e-3;

    void SetUp() {
        testTruncNorm = std::make_unique<BG::NES::Simulator::Distributions::TruncNorm>(0.1f, 2.0f);
        testTruncNorm->SetSeed(seed);
    }

    void TearDown() {
        return;
    }
};

TEST_F( TruncNormTest, test_RandomSample_default ) {
    std::vector<float> sample = testTruncNorm->RandomSample(10);
    
    ASSERT_EQ(sample.size(), 10);
    for (float val: sample)
        ASSERT_TRUE((0.1f <= val) && (val <= 2.0f));
}

TEST_F( TruncNormTest, test_PDF_default ) {
    std::vector<float> x = {0.1f, 1.0f, 2.0f};
    std::vector<float> pdf = testTruncNorm->PDF(x);

    ASSERT_NEAR(pdf[0], 0.9075, tol);
    ASSERT_NEAR(pdf[1], 0.5532, tol);
    ASSERT_NEAR(pdf[2], 0.1234, tol);
}

TEST_F( TruncNormTest, test_CDF_default ) {
    std::vector<float> x = {0.1011f, 0.7016f, 1.9920f};
    std::vector<float> cdf = testTruncNorm->CDF(x);

    ASSERT_NEAR(cdf[0], 0.001, tol);
    ASSERT_NEAR(cdf[1], 0.5, tol);
    ASSERT_NEAR(cdf[2], 0.999, tol);
}

TEST_F( TruncNormTest, test_Stats_default ) {
    std::tuple<float, float> stats = testTruncNorm->Stats();

    ASSERT_NEAR(std::get<0>(stats), 0.7840, tol);
    ASSERT_NEAR(std::get<1>(stats), 0.4787, tol);
}

TEST_F( TruncNormTest, test_Mean_default ) {
    float mean = testTruncNorm->Mean();
    
    ASSERT_NEAR(mean, 0.7840, tol);
}

TEST_F( TruncNormTest, test_Std_default ) {
    float std = testTruncNorm->Std();
    
    ASSERT_NEAR(std, 0.4787, tol);
}
