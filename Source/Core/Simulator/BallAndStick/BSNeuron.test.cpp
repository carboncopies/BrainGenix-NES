//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSNeuron struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

////! Records the time of direct stimulation for every occurrence
////! of a direct stimulation.
//void AttachDirectStim(float t_ms);

////! Set the distribution for delta t spontaneous (time changed
////! since last spontaneous activity).
//void SetSpontaneousActivity(float mean, float stdev);
//
////! Keeps track of the membrane potential and the time of update.
//void Record(float t_ms);

////! Returns the recorded membrane potentials.
//std::unordered_map<std::string, std::vector<float>> GetRecording();
//
////! Tells if the action potential threshold has been crossed.
//bool HasSpiked();

////! Returns the time since the action potential threshold was
////! crossed last.
//float DtAct_ms(float t_ms);

////! Updates V_spike_t.
//float VSpikeT_mV(float t_ms);

////! Updates V_AHP_t.
//float VAHPT_mV(float t_ms);

////! Updates V_PSP_t.
//float VPSPT_mV(float t_ms);

////! Vm = Vrest + VSpike(t) + VAHP(t) + VPSP(t)
////! Compares Vm with Vact.
//void UpdateVm(float t_ms, bool recording);

////! Compares Vm with Vact.
//void DetectThreshold(float t_ms);

////! Checks for possible spontaneous activity.
//void SpontaneousActivity(float t_ms);
//
////! Updates all potential components, the membrane potential
////! and the time of update.
//void Update(float t_ms, bool recording);

////! We have to flip the signal FIFO, because the most recent is in [0].
////! We need this, because the kernel has a specific time order.
////! Alternatively, when we prepare the kernel we can flip it and
////! remember to view [0] as most recent in the convolution result.
//void UpdateConvolvedFIFO(std::vector<float> kernel);
//
////! Sets the initial value of the FIFO.
//void SetFIFO(float FIFO_ms, float dt_ms);

#include <cmath>
#include <vector>
#include <memory>
#include <cstddef>

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Distributions/TruncNorm.h>

/**
 * @brief Test class for unit tests for struct BSNeuron.
 *
 */

struct BSNeuronTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSNeuron> testBSNeuron = nullptr;
    
    float axonRadius_um = 0.5;
    float somaRadius_um = 0.5;
    BG::NES::Simulator::Geometries::Vec3D axonEnd0_um{0.0, 0.0, 0.0};
    BG::NES::Simulator::Geometries::Vec3D axonEnd1_um{0.0, 10.0, 0.0};

    std::shared_ptr<BG::NES::Simulator::Geometries::Sphere> testSoma = \
         std::make_shared<BG::NES::Simulator::Geometries::Sphere>(somaRadius_um);
    std::shared_ptr<BG::NES::Simulator::Geometries::Cylinder> testAxon = \
         std::make_shared<BG::NES::Simulator::Geometries::Cylinder>(
                 axonRadius_um, axonEnd0_um, axonRadius_um, axonEnd1_um);
    float tol = 1e-3;

    void SetUp() {
        testBSNeuron = std::make_unique<BG::NES::Simulator::BallAndStick::BSNeuron>(
                200, testSoma, testAxon);

    }

    void TearDown() {
        return;
    }
};

TEST_F( BSNeuronTest, test_GetCellCenter_default ) {
    BG::NES::Simulator::Geometries::Vec3D cellCenter = testBSNeuron->GetCellCenter();
    BG::NES::Simulator::Geometries::Vec3D expectedCenter = testSoma->Center_um;
    ASSERT_EQ(cellCenter==expectedCenter, true);
}

TEST_F( BSNeuronTest, test_AttachDirectStim_default ) {
    testBSNeuron->AttachDirectStim(0.1f);

    // Time recorded successfully
    ASSERT_EQ(testBSNeuron->tDirectStim_ms[0], 0.1f);

    for (size_t i=0; i<5; ++i)
        testBSNeuron->AttachDirectStim(i * 0.15f);

    // Number of direct stimulus activities recorded.
    ASSERT_EQ(testBSNeuron->tDirectStim_ms.size(), 6);  
}

TEST_F( BSNeuronTest, test_SetSpontaneousActivity_default ) {
    float testMu = 0.1;
    float testStd = 0.5;
    std::tuple<float, float> gotTauSpontMeanStdev_ms;
    std::tuple<float, float> expectedDistStats, gotDistStats;


    std::unique_ptr<BG::NES::Simulator::Distributions::Distribution> expectedDtSpontDist =\
         std::make_unique<BG::NES::Simulator::Distributions::TruncNorm>(
        -testMu / testStd, testMu / testStd, testMu, testStd
    );
    expectedDistStats = expectedDtSpontDist->Stats();
    
    testBSNeuron->SetSpontaneousActivity(testMu, testStd);

    gotTauSpontMeanStdev_ms = testBSNeuron->tauSpontMeanStdev_ms;
    gotDistStats = testBSNeuron->dtSpontDist->Stats();
    
    // TauSpontMeanStdev_ms must be equal to the mu and sigma values supplied
    ASSERT_NEAR(std::get<0>(gotTauSpontMeanStdev_ms), testMu, tol);
    ASSERT_NEAR(std::get<1>(gotTauSpontMeanStdev_ms), testStd, tol);
    
    // Statistics for the spontaneous Dt distribution must match the
    // statistics of the expected distribution.
    ASSERT_EQ(expectedDistStats, gotDistStats);
}

TEST_F( BSNeuronTest, test_Record_default ) {
    size_t oldLenTimesteps = testBSNeuron->tRecorded_ms.size();
    size_t oldLenVmRecorded = testBSNeuron->VmRecorded_mV.size();

    testBSNeuron->Record(0.1);
    
    ASSERT_EQ( testBSNeuron->tRecorded_ms.size(), oldLenTimesteps + 1 );
    ASSERT_EQ( testBSNeuron->VmRecorded_mV.size(), oldLenVmRecorded + 1 );
}

TEST_F( BSNeuronTest, test_HasSpiked_default ) {
    // No spike event immediately after initialization
    ASSERT_FALSE(testBSNeuron->HasSpiked());

    // TODO: Add test for confirming spike event
}

TEST_F( BSNeuronTest, test_DtAct_ms_default ) {
    float dtAct_ms = testBSNeuron->DtAct_ms(0.1);

    // Immediately after startup, no spike event has been recorded.
    ASSERT_NEAR(dtAct_ms, _NO_SPIKE_DT_mS, tol) << "dtAct_ms = " << dtAct_ms; 
    // TODO: Add test for getting dt_act_ms after spike event
}

TEST_F( BSNeuronTest, test_VSpiket_mV_default ) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VSpikeT_mV(0.1), 0.0f, tol);
    // TODO: What happens after there is a spike event?
}

TEST_F( BSNeuronTest, test_VAHPT_mV_default ) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VAHPT_mV(0.1), 0.0f, tol);
    // TODO: What happens after there is a spike event?
}

TEST_F( BSNeuronTest, test_VPSPT_mV_default ) {
    // No spike event immediately after set up.
    ASSERT_NEAR(testBSNeuron->VPSPT_mV(0.1), 0.0f, tol); 
    // TODO: What happens after there is a spike event?
}

TEST_F( BSNeuronTest, test_UpdateVm_default ) {
    // No spike event immediately after set up.
    size_t oldLenTimesteps = testBSNeuron->tRecorded_ms.size();
    size_t oldLenVmRecorded = testBSNeuron->VmRecorded_mV.size();
    
    testBSNeuron->UpdateVm(0.1, true);

    ASSERT_EQ(testBSNeuron->tRecorded_ms.size(), oldLenTimesteps + 1);
    ASSERT_EQ(testBSNeuron->VmRecorded_mV.size(), oldLenVmRecorded + 1);
}

TEST_F( BSNeuronTest, test_DetectThreshold_default ) {
    // Action potential threshold will not be crossed immediately
    // after set up.
    size_t oldTAct_ms_Length = testBSNeuron->tAct_ms.size();

    testBSNeuron->DetectThreshold(0.1);
    ASSERT_EQ(testBSNeuron->tAct_ms.size(), oldTAct_ms_Length);
}

TEST_F( BSNeuronTest, test_xx_default ) {

}
