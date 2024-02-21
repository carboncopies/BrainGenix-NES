//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSNeuron struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <deque>

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Distributions/TruncNorm.h>
#include <Simulator/Structs/SignalFunctions.h>
#include <Simulator/Geometries/GeometryCollection.h>

/**
 * @brief Test class for unit tests for struct BSNeuron.
 *
 */

struct BSNeuronTest : testing::Test {
    BG::NES::Simulator::Geometries::GeometryCollection Collection;

    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSNeuron> testBSNeuron = nullptr;

    float axonRadius_um = 0.5;
    float somaRadius_um = 0.5;
    std::vector<float> times_ms = {0.1, 1.5, 0.2, 0.3, 1.6}; //! Time values used for simulation

    BG::NES::Simulator::Geometries::Vec3D spherecenter_um{0.0, 0.0, 0.0};
    BG::NES::Simulator::Geometries::Vec3D axonEnd0_um{0.0, 0.0, 0.0};
    BG::NES::Simulator::Geometries::Vec3D axonEnd1_um{0.0, 10.0, 0.0};

    BG::NES::Simulator::Geometries::Sphere * testSoma = &Collection.AddSphere(spherecenter_um, somaRadius_um);
    BG::NES::Simulator::Geometries::Cylinder * testAxon = &Collection.AddCylinder(axonRadius_um, axonEnd0_um, axonRadius_um, axonEnd1_um);

    float tol = 1e-3;

    void SetUp() {
        testBSNeuron = std::make_unique<BG::NES::Simulator::BallAndStick::BSNeuron>(200, testSoma, testAxon);
    }

    void Simulate() {
        std::vector<float> reversed_kernel(10, 0.0);
        testBSNeuron->SetFIFO(1.1, 0.1, reversed_kernel.size());
        testBSNeuron->SetSpontaneousActivity(0.5, 5.0, 0);

        for (float val : times_ms)
            testBSNeuron->Update(val, true);
    }

    void TearDown() { return; }
};

TEST_F(BSNeuronTest, test_GetCellCenter_default) {
    BG::NES::Simulator::Geometries::Vec3D cellCenter =
        testBSNeuron->GetCellCenter();
    BG::NES::Simulator::Geometries::Vec3D expectedCenter = testSoma->Center_um;
    ASSERT_EQ(cellCenter == expectedCenter, true);
}

TEST_F(BSNeuronTest, test_AttachDirectStim_default) {
    testBSNeuron->AttachDirectStim(0.1f);

    // Time recorded successfully
    ASSERT_EQ(testBSNeuron->TDirectStim_ms[0], 0.1f);

    for (size_t i = 0; i < 5; ++i)
        testBSNeuron->AttachDirectStim(i * 0.15f);

    // Number of direct stimulus activities recorded.
    ASSERT_EQ(testBSNeuron->TDirectStim_ms.size(), 6);
}

TEST_F(BSNeuronTest, test_SetSpontaneousActivity_default) {
    float testMu = 0.1;
    float testStd = 0.5;
    float mean_ms;
    float stdev_ms;
    std::tuple<float, float> expectedDistStats, gotDistStats;

    std::unique_ptr<BG::NES::Simulator::Distributions::Distribution>
        expectedDtSpontDist =
            std::make_unique<BG::NES::Simulator::Distributions::TruncNorm>(
                -testMu / testStd, testMu / testStd, testMu, testStd);
    expectedDistStats = expectedDtSpontDist->Stats();

    testBSNeuron->SetSpontaneousActivity(testMu, testStd, 0);

    mean_ms = testBSNeuron->TauSpont_ms.mean;
    stdev_ms = testBSNeuron->TauSpont_ms.stdev;
    gotDistStats = testBSNeuron->DtSpontDist->Stats();

    // TauSpontMeanStdev_ms must be equal to the mu and sigma values supplied
    ASSERT_NEAR(mean_ms, testMu, tol);
    ASSERT_NEAR(stdev_ms, testStd, tol);

    // Statistics for the spontaneous Dt distribution must match the
    // statistics of the expected distribution.
    ASSERT_EQ(expectedDistStats, gotDistStats);
}

TEST_F(BSNeuronTest, test_Record_default) {
    size_t oldLenTimesteps = testBSNeuron->TRecorded_ms.size();
    size_t oldLenVmRecorded = testBSNeuron->VmRecorded_mV.size();

    testBSNeuron->Record(0.1);

    ASSERT_EQ(testBSNeuron->TRecorded_ms.size(), oldLenTimesteps + 1);
    ASSERT_EQ(testBSNeuron->VmRecorded_mV.size(), oldLenVmRecorded + 1);
}

TEST_F(BSNeuronTest, test_HasSpiked_default) {
    // No spike event immediately after initialization
    ASSERT_FALSE(testBSNeuron->HasSpiked());

    // Simulate
    Simulate();

    // Confirm spike event
    ASSERT_TRUE(testBSNeuron->HasSpiked());
}

TEST_F(BSNeuronTest, test_DtAct_ms_default) {
    float dtAct_ms = testBSNeuron->DtAct_ms(0.1);

    // Immediately after startup, no spike event has been recorded.
    ASSERT_EQ(dtAct_ms, _NO_SPIKE_DT_ms);

    // Simulate
    Simulate();

    // Get dt_act_ms after spike event
    ASSERT_TRUE(testBSNeuron->DtAct_ms(times_ms.back()) >= 0.0);
}

TEST_F(BSNeuronTest, test_VSpikeT_mV_default) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VSpikeT_mV(0.1), 0.0f, tol);

    // Simulate
    Simulate();

    // After there is a spike event, spike potential is nonzero.
    ASSERT_TRUE(testBSNeuron->VSpikeT_mV(times_ms.back()) >= 0.0);
}

TEST_F(BSNeuronTest, test_VAHPT_mV_default) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VAHPT_mV(0.1), 0.0f, tol);

    // Simulate
    Simulate();

    // After there is a spike event, AHP potential is nonzero.
    ASSERT_TRUE(testBSNeuron->VAHPT_mV(times_ms.back()) >= 0.0);
}

TEST_F(BSNeuronTest, test_VPSPT_mV_default) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VPSPT_mV(0.1), 0.0f, tol);

    // Simulate
    Simulate();

    // After there is a spike event, PSP potential is nonzero.
    ASSERT_TRUE(testBSNeuron->VPSPT_mV(times_ms.back()) >= 0.0);
}

TEST_F(BSNeuronTest, test_UpdateVm_default) {
    // No spike event immediately after set up.
    size_t oldLenTimesteps = testBSNeuron->TRecorded_ms.size();
    size_t oldLenVmRecorded = testBSNeuron->VmRecorded_mV.size();

    testBSNeuron->UpdateVm(0.1, true);

    ASSERT_EQ(testBSNeuron->TRecorded_ms.size(), oldLenTimesteps + 1);
    ASSERT_EQ(testBSNeuron->VmRecorded_mV.size(), oldLenVmRecorded + 1);
}

TEST_F(BSNeuronTest, test_DetectThreshold_default) {
    // Action potential threshold will not be crossed immediately
    // after set up.
    size_t oldTAct_ms_Length = testBSNeuron->TAct_ms.size();

    testBSNeuron->DetectThreshold(0.1);
    ASSERT_EQ(testBSNeuron->TAct_ms.size(), oldTAct_ms_Length);

    Simulate();

    // After there is a spike event, the time of spike event is recorded.
    testBSNeuron->DetectThreshold(times_ms.back());
    ASSERT_TRUE(testBSNeuron->TAct_ms.size() > oldTAct_ms_Length);
}

TEST_F(BSNeuronTest, test_GetRecording_default) {
    auto recording = testBSNeuron->GetRecording();
    ASSERT_FALSE(recording.empty());

    // Immediately after set up no membrane potentials have
    // been recorded.
    ASSERT_EQ((recording.at("Vm_mV")).size(), 0);

    // Simulate
    Simulate();

    recording = testBSNeuron->GetRecording();

    ASSERT_EQ(recording.at("Vm_mV").size(), 3);
}

TEST_F(BSNeuronTest, test_InAbsRef_default) {
    float dtAct_ms = testBSNeuron->DtAct_ms(0.1);
    bool inAbsRef = testBSNeuron->in_absref;

    // No spike has occurred immediately after set up
    ASSERT_EQ(dtAct_ms, _NO_SPIKE_DT_ms);
    ASSERT_TRUE(!inAbsRef);

    // Simulate
    Simulate();

    dtAct_ms = testBSNeuron->DtAct_ms(times_ms.back());
    inAbsRef = testBSNeuron->in_absref;

    ASSERT_TRUE(dtAct_ms >= 0);
    ASSERT_TRUE(inAbsRef);
}

TEST_F(BSNeuronTest, test_SpontaneousActivity_default) {
    // No spontaneous activity immediately after setup
    ASSERT_NEAR(testBSNeuron->TSpontNext_ms, _T_SPONT_NEXT_mS_INIT, tol);

    testBSNeuron->SetSpontaneousActivity(0.5, 5.0, 0);
    testBSNeuron->SpontaneousActivity(0.1);
    testBSNeuron->SpontaneousActivity(1.5);

    ASSERT_TRUE(testBSNeuron->TSpontNext_ms >= 0.0);
    ASSERT_EQ(testBSNeuron->TAct_ms.size(), 1);
}

TEST_F(BSNeuronTest, test_Update_default) {

    size_t oldLenTimesteps = testBSNeuron->TRecorded_ms.size();
    size_t oldLenVmRecorded = testBSNeuron->VmRecorded_mV.size();

    testBSNeuron->SetSpontaneousActivity(0.5, 5.0, 0);
    testBSNeuron->Update(0.1, true);
    testBSNeuron->Update(1.5, true);

    ASSERT_EQ(testBSNeuron->TRecorded_ms.size(), oldLenTimesteps + 2);
    ASSERT_EQ(testBSNeuron->VmRecorded_mV.size(), oldLenVmRecorded + 2);
    ASSERT_EQ(testBSNeuron->TAct_ms.size(), 1);
    ASSERT_TRUE(testBSNeuron->T_ms >= 0.0);
    ASSERT_TRUE(testBSNeuron->TSpontNext_ms >= 0.0);
}

TEST_F(BSNeuronTest, test_SetFIFO_default) {
    // Immediately after set up, size of FIFO has not been set
    ASSERT_EQ(testBSNeuron->FIFO.size(), 0);

    float FIFO_ms = 1.1, dt_ms = 0.1;
    size_t expectedFIFOSize = FIFO_ms / dt_ms + 1;

    std::vector<float> reversed_kernel(10, 0.0);
    testBSNeuron->SetFIFO(1.1, 0.1, reversed_kernel.size());

    ASSERT_EQ(testBSNeuron->FIFO.size(), expectedFIFOSize);
}

TEST_F(BSNeuronTest, test_UpdateConvolvedFIFO_default) {
    std::vector<float> reversed_kernel = {1.0, 0.0, -1.0};
    std::vector<float> expectedConvolvedFIFO{};
    std::deque<float> FIFO{};

    // Simulate
    Simulate();

    testBSNeuron->UpdateConvolvedFIFO(reversed_kernel);

    std::reverse_copy(testBSNeuron->FIFO.begin(), testBSNeuron->FIFO.end(), FIFO.begin());

    for (size_t i = 0; i < FIFO.size(); ++i) {
        FIFO[i] *= -1.0;
        if (FIFO[i] < 0.0)
            FIFO[i] = 0.0;
    }

    expectedConvolvedFIFO.resize(FIFO.size(), 0.0);

    BG::NES::Simulator::SignalFunctions::Convolve1D(FIFO, reversed_kernel, expectedConvolvedFIFO);

    ASSERT_EQ(testBSNeuron->ConvolvedFIFO.size(), expectedConvolvedFIFO.size());
    for (size_t i = 0; i < testBSNeuron->ConvolvedFIFO.size(); ++i)
        ASSERT_NEAR(testBSNeuron->ConvolvedFIFO[i], expectedConvolvedFIFO[i],
                    tol);

    ASSERT_EQ(testBSNeuron->CaSamples.back(),
              testBSNeuron->ConvolvedFIFO.back() + 1.0f);
    ASSERT_EQ(testBSNeuron->TCaSamples_ms.back(), 1.6f);
}
