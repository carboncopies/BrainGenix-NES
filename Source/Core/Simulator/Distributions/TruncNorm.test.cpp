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

// New comprehensive tests for NT-3

TEST_F( TruncNormTest, test_RandomSample_moments_large_N ) {
    // Case 15: N=100,000 samples with seed 43 in [0.1,2.0]
    // Sample mean/sd should be within tolerance of analytic values
    testTruncNorm->SetSeed(43);
    std::vector<float> samples = testTruncNorm->RandomSample(100000);

    // All samples should be in bounds
    for (float s : samples) {
        ASSERT_TRUE(s >= 0.1f && s <= 2.0f) << "Sample " << s << " outside bounds";
    }

    // Calculate sample statistics
    double sum = 0.0;
    for (float s : samples) sum += s;
    double sample_mean = sum / samples.size();

    double sum_sq_diff = 0.0;
    for (float s : samples) sum_sq_diff += (s - sample_mean) * (s - sample_mean);
    double sample_sd = std::sqrt(sum_sq_diff / samples.size());

    float analytic_mean = testTruncNorm->Mean();
    float analytic_sd = testTruncNorm->Std();

    // Tolerances for large-N sample statistics
    ASSERT_NEAR(sample_mean, analytic_mean, 0.01f) << "Sample mean " << sample_mean << " vs analytic " << analytic_mean;
    ASSERT_NEAR(sample_sd, analytic_sd, 0.01f) << "Sample sd " << sample_sd << " vs analytic " << analytic_sd;
}

// Bug: dev_frame #101
// TruncNorm.cpp:65,91-93 (also 115-117, 131-153): z_a/z_b/Z/mean/sigma are
// function statics computed once per process, so a second instance gets the
// first one's PDF, CDF, Mean and Std. This test asserts the CORRECT behaviour
// (independent instances) and fails today.
TEST( TruncNormKnownBug, test_InstanceIndependence ) {
    // First instance: (0.1, 2.0)
    BG::NES::Simulator::Distributions::TruncNorm tn1(0.1f, 2.0f);
    tn1.SetSeed(43);
    float mean1 = tn1.Mean();
    float pdf1 = tn1.PDF(1.0f);

    // Second instance: (0, 20, 10, 2) - different parameters
    BG::NES::Simulator::Distributions::TruncNorm tn2(0.0f, 20.0f, 10.0f, 2.0f);
    tn2.SetSeed(43);
    float mean2 = tn2.Mean();
    float pdf2 = tn2.PDF(10.0f);

    // Each instance should have its own mean and PDF, not share statics from first instance
    // The analytic mean for (0, 20, 10, 2) should be around 10 ±1e-3
    EXPECT_NEAR(mean2, 10.0f, 1e-3f);

    // PDF(10) of N(10,2) truncated to [0,20] is 1/(2*sqrt(2*pi)) = 0.1995
    EXPECT_NEAR(pdf2, 0.1995f, 1e-3f);

    // These should be different (unless by coincidence)
    EXPECT_NE(mean1, mean2);
}
