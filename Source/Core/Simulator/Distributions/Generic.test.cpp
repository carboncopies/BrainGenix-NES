//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Generic distribution class.
    Additional Notes: None
    Date Created: 2026-10-10
*/

#include <cmath>
#include <vector>
#include <memory>

#include <gtest/gtest.h>

#include <Simulator/Distributions/Generic.h>

/**
 * @brief Test class for unit tests for Generic distribution.
 *
 */

struct GenericTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Distributions::Generic> gen = nullptr;
    float tol = 1e-3f;
    uint32_t seed = 42;

    void SetUp() {
        gen = std::make_unique<BG::NES::Simulator::Distributions::Generic>(seed);
    }

    void TearDown() {
        return;
    }
};

// Case 17: Determinism - two Generic(42) produce identical sequences exactly
TEST_F(GenericTest, test_Determinism_UniformRandomFloat) {
    // First sequence
    std::vector<float> seq1;
    for (int i = 0; i < 100; ++i) {
        seq1.push_back(gen->UniformRandomFloat());
    }

    // Second sequence with same seed
    std::unique_ptr<BG::NES::Simulator::Distributions::Generic> gen2 =
        std::make_unique<BG::NES::Simulator::Distributions::Generic>(seed);
    std::vector<float> seq2;
    for (int i = 0; i < 100; ++i) {
        seq2.push_back(gen2->UniformRandomFloat());
    }

    // Sequences should be identical
    for (size_t i = 0; i < seq1.size(); ++i) {
        ASSERT_EQ(seq1[i], seq2[i]) << "Sequences differ at index " << i;
    }
}

TEST_F(GenericTest, test_Determinism_NormalRandomFloat) {
    // First sequence
    std::vector<float> seq1;
    for (int i = 0; i < 100; ++i) {
        seq1.push_back(gen->NormalRandomFloat());
    }

    // Second sequence with same seed
    std::unique_ptr<BG::NES::Simulator::Distributions::Generic> gen2 =
        std::make_unique<BG::NES::Simulator::Distributions::Generic>(seed);
    std::vector<float> seq2;
    for (int i = 0; i < 100; ++i) {
        seq2.push_back(gen2->NormalRandomFloat());
    }

    // Sequences should be identical
    for (size_t i = 0; i < seq1.size(); ++i) {
        ASSERT_EQ(seq1[i], seq2[i]) << "Sequences differ at index " << i;
    }
}

TEST_F(GenericTest, test_Determinism_UniformRandomInt) {
    // First sequence
    std::vector<int> seq1;
    for (int i = 0; i < 100; ++i) {
        seq1.push_back(gen->UniformRandomInt());
    }

    // Second sequence with same seed
    std::unique_ptr<BG::NES::Simulator::Distributions::Generic> gen2 =
        std::make_unique<BG::NES::Simulator::Distributions::Generic>(seed);
    std::vector<int> seq2;
    for (int i = 0; i < 100; ++i) {
        seq2.push_back(gen2->UniformRandomInt());
    }

    // Sequences should be identical
    for (size_t i = 0; i < seq1.size(); ++i) {
        ASSERT_EQ(seq1[i], seq2[i]) << "Sequences differ at index " << i;
    }
}

TEST_F(GenericTest, test_UniformRandomFloat_range) {
    // UniformRandomFloat should be in [0,1)
    for (int i = 0; i < 1000; ++i) {
        float val = gen->UniformRandomFloat();
        ASSERT_TRUE(val >= 0.0f && val < 1.0f) << "Value " << val << " out of range [0,1)";
    }
}

TEST_F(GenericTest, test_UniformRandomFloat_moments) {
    // Case 17: N=100,000: UniformRandomFloat is in [0,1) with mean 0.5 ±0.005
    std::vector<float> samples;
    for (int i = 0; i < 100000; ++i) {
        samples.push_back(gen->UniformRandomFloat());
    }

    // Calculate mean
    double sum = 0.0;
    for (float s : samples) sum += s;
    double sample_mean = sum / samples.size();

    // Calculate standard deviation
    double sum_sq_diff = 0.0;
    for (float s : samples) {
        sum_sq_diff += (s - sample_mean) * (s - sample_mean);
    }
    double sample_sd = std::sqrt(sum_sq_diff / samples.size());

    // For uniform [0,1): mean = 0.5, sd = 1/sqrt(12) ≈ 0.2887
    ASSERT_NEAR(sample_mean, 0.5f, 0.005f) << "Sample mean " << sample_mean;
    ASSERT_NEAR(sample_sd, 1.0f / sqrt(12.0f), 0.01f) << "Sample sd " << sample_sd;
}

TEST_F(GenericTest, test_UniformRandomInt_non_negative) {
    // Case 17: UniformRandomInt ≥ 0
    for (int i = 0; i < 10000; ++i) {
        int val = gen->UniformRandomInt();
        ASSERT_GE(val, 0) << "Value " << val << " is negative";
    }
}

TEST_F(GenericTest, test_NormalRandomFloat_moments) {
    // Case 17: N=100,000: NormalRandomFloat has mean 0 ±0.02 and sd 1 ±0.02
    std::vector<float> samples;
    for (int i = 0; i < 100000; ++i) {
        samples.push_back(gen->NormalRandomFloat());
    }

    // Calculate mean
    double sum = 0.0;
    for (float s : samples) sum += s;
    double sample_mean = sum / samples.size();

    // Calculate standard deviation
    double sum_sq_diff = 0.0;
    for (float s : samples) {
        sum_sq_diff += (s - sample_mean) * (s - sample_mean);
    }
    double sample_sd = std::sqrt(sum_sq_diff / samples.size());

    // Standard normal: mean = 0, sd = 1
    ASSERT_NEAR(sample_mean, 0.0f, 0.02f) << "Sample mean " << sample_mean;
    ASSERT_NEAR(sample_sd, 1.0f, 0.02f) << "Sample sd " << sample_sd;
}

TEST_F(GenericTest, test_DifferentSeeds_produce_different_sequences) {
    // Two different seeds should produce different sequences (with high probability)
    std::vector<float> seq1;
    for (int i = 0; i < 100; ++i) {
        seq1.push_back(gen->UniformRandomFloat());
    }

    std::unique_ptr<BG::NES::Simulator::Distributions::Generic> gen2 =
        std::make_unique<BG::NES::Simulator::Distributions::Generic>(42 + 1);
    std::vector<float> seq2;
    for (int i = 0; i < 100; ++i) {
        seq2.push_back(gen2->UniformRandomFloat());
    }

    // At least some values should differ
    int diffs = 0;
    for (size_t i = 0; i < seq1.size(); ++i) {
        if (seq1[i] != seq2[i]) diffs++;
    }
    ASSERT_GT(diffs, 0) << "Sequences from different seeds are identical";
}
