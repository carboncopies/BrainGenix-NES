//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for Connections::g_norm, in particular its per-kernel lookback cap.
*/

#include <algorithm>
#include <cmath>
#include <vector>

#include <gtest/gtest.h>

#include <Simulator/Structs/Receptor.h>

using BG::NES::Simulator::Connections::compute_normalization;
using BG::NES::Simulator::Connections::g_norm;

namespace {

// Brute force reference: the full sum over every past spike, in double, no cutoff.
double ReferenceGNorm(double t, const std::vector<float>& spikes, double rise, double decay, double norm, double onset) {
    t -= onset;
    double s = 0;
    for (float ts : spikes) {
        double dt = t - ts;
        if (dt >= 0) s += std::exp(-dt / decay) - std::exp(-dt / rise);
    }
    return s / norm;
}

// The behaviour before the lookback cap: stop only when dt > spike_dt_delta and the contribution is small.
float OriginalGNorm(float t, const std::vector<float>& spikes, float rise, float decay, float norm, float onset,
                    float spike_dt_delta = 1000, float history_delta = 0.001) {
    t -= onset;
    history_delta *= norm;
    float g = 0;
    for (auto it = spikes.rbegin(); it != spikes.rend(); ++it) {
        float dt = t - *it;
        if (dt >= 0) {
            float c = std::exp(-dt / decay) - std::exp(-dt / rise);
            if (dt > spike_dt_delta && c < history_delta) return g / norm;
            g += c;
        }
    }
    return g / norm;
}

std::vector<float> Train(float from, float to, float step) {
    std::vector<float> v;
    for (float t = from; t <= to; t += step) v.push_back(t);
    return v;
}

}  // namespace

TEST(GNormTest, EmptyAndFutureSpikesGiveZero) {
    float norm = compute_normalization(0.5f, 3.0f);
    EXPECT_EQ(g_norm(10.0f, {}, 0.5f, 3.0f, norm, 1.0f), 0.0f);
    EXPECT_EQ(g_norm(10.0f, {20.0f, 30.0f}, 0.5f, 3.0f, norm, 1.0f), 0.0f);
}

TEST(GNormTest, FastKernelDenseTrainMatchesFullSum) {
    // AMPA-like kernel, one spike per ms for 3 s (the worst case for the dropped tail).
    float rise = 0.5f, decay = 3.0f;
    float norm = compute_normalization(rise, decay);
    auto spikes = Train(0.0f, 2999.0f, 1.0f);
    for (float t : {2999.0f, 3000.0f, 3000.5f, 3010.0f}) {
        double ref = ReferenceGNorm(t, spikes, rise, decay, norm, 1.0);
        EXPECT_NEAR(g_norm(t, spikes, rise, decay, norm, 1.0f), ref, 5e-5) << "t=" << t;
    }
}

TEST(GNormTest, FastKernelSparseTrainMatchesFullSum) {
    float rise = 0.5f, decay = 3.0f;
    float norm = compute_normalization(rise, decay);
    auto spikes = Train(0.0f, 2999.0f, 7.0f);
    for (float t : {100.0f, 1500.0f, 3005.0f}) {
        EXPECT_NEAR(g_norm(t, spikes, rise, decay, norm, 1.0f), ReferenceGNorm(t, spikes, rise, decay, norm, 1.0), 5e-5);
    }
}

TEST(GNormTest, MediumKernelFAHPDenseTrainMatchesFullSum) {
    float rise = 2.5f, decay = 30.0f;  // fAHP of the dp network
    float norm = compute_normalization(rise, decay);
    auto spikes = Train(0.0f, 2999.0f, 2.0f);
    double ref = ReferenceGNorm(3000.0, spikes, rise, decay, norm, 0.0);
    EXPECT_NEAR(g_norm(3000.0f, spikes, rise, decay, norm, 0.0f), ref, 5e-4 * std::max(1.0, ref));
}

TEST(GNormTest, SlowKernelsAreUnchangedByTheCap) {
    // sAHP (30/300 ms) and ADP (20/200 ms): 12*tau > 1000 ms, so the original cutoff applies as before.
    struct K { float rise, decay; } kernels[] = {{30.0f, 300.0f}, {20.0f, 200.0f}};
    auto spikes = Train(0.0f, 4999.0f, 3.0f);
    for (auto k : kernels) {
        float norm = compute_normalization(k.rise, k.decay);
        for (float t : {500.0f, 2500.0f, 5000.0f}) {
            // Same terms in the same order; only float/double libm differences (OriginalGNorm uses expf).
            float got = g_norm(t, spikes, k.rise, k.decay, norm, 0.0f);
            float want = OriginalGNorm(t, spikes, k.rise, k.decay, norm, 0.0f);
            EXPECT_NEAR(got, want, 1e-5f * want) << "decay=" << k.decay << " t=" << t;
        }
    }
}

TEST(GNormTest, ExplicitSmallerSpikeDtDeltaStillHonoured) {
    float rise = 30.0f, decay = 300.0f;
    float norm = compute_normalization(rise, decay);
    auto spikes = Train(0.0f, 4999.0f, 3.0f);
    float want = OriginalGNorm(5000.0f, spikes, rise, decay, norm, 0.0f, 100.0f);
    EXPECT_NEAR(g_norm(5000.0f, spikes, rise, decay, norm, 0.0f, 100.0f), want, 1e-5f * want);
}
