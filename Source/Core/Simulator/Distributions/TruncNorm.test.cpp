//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the TruncNorm struct.
    Additional Notes: None
    Date Created: 2023-10-10
*/

#include <cstdint>
#include <random>
#include <vector>
#include <tuple>

#include <gtest/gtest.h>

#include <Simulator/Distributions/TruncNorm.h>


/**
 * @brief Test class for unit tests for struct TruncNorm.
 *
 */

//  std::vector<float> RandomSample(float loc, float scale, size_t numSamples);
//  
//  std::vector<float> PDF(std::vector<float> x, float loc, float scale);
//  float PDF(float x, float loc, float scale);
//  
//  std::vector<float> CDF(std::vector<float> x, float loc, float scale);
//  float CDF(float x, float loc, float scale);

//  std::vector<float> PPF(std::vector<float> x, float loc, float scale);
//  float PPF(float x, float loc, float scale);
//  
//  std::tuple<float, float, float, float> Stats(float loc, float scale);

//  float Mean(float loc, float scale);

//  float Std(float loc, float scale);


struct TruncNormTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::Distributions::TruncNorm> testTruncNorm = nullptr;
    
    uint32_t seed = 43;
    float tol = 1e-3;
    std::mt19937 gen;

    void SetUp() {
        testTruncNorm = std::make_unique<BG::NES::Simulator::Distributions::TruncNorm>(0.1f, 2.0f);
        testTruncNorm->SetSeed(seed);
        gen.seed(seed);
    }

    void TearDown() {
        return;
    }
};

TEST_F( TruncNormTest, test_RandomSample_default ) {

}

TEST_F( TruncNormTest, test_PDF_default ) {
}

TEST_F( TruncNormTest, test_CDF_default ) {
}

TEST_F( TruncNormTest, test_PPF_default ) {
    std::vector<float> x = {0.001f, 0.5f, 0.999f}; 
    std::vector<float> ppf = testTruncNorm->PPF(x, 0.0f, 1.0f);

    ASSERT_EQ(1.1f, 1.1f);
}

TEST_F( TruncNormTest, test_Stats_default ) {
    std::tuple<float, float, float, float> stats = testTruncNorm->Stats(0.0, 1.0);
    ASSERT_NEAR(std::get<0>(stats), 0.0, tol);
    ASSERT_NEAR(std::get<1>(stats), 0.0, tol);
    ASSERT_NEAR(std::get<2>(stats), 0.0, tol);
    ASSERT_NEAR(std::get<3>(stats), 0.0, tol);
}

TEST_F( TruncNormTest, test_Mean_default ) {
}

TEST_F( TruncNormTest, test_Std_default ) {
}
