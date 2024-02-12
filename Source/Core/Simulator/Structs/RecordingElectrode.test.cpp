//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the RecordingElectrode
   struct. Additional Notes: None Date Created: 2023-10-13
*/

#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>
#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>
#include <Simulator/Structs/Simulation.h>
#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

/**
 * @brief Test class for unit tests for the RecordingElectrode struct.
 *
 */

struct RecordingElectrodeTest : testing::Test {
    BG::NES::Simulator::Geometries::GeometryCollection Collection;

    std::unique_ptr<BG::NES::Simulator::Tools::RecordingElectrode> testElectrode{};
    std::shared_ptr<BG::NES::Simulator::CoreStructs::NeuralCircuit> testNC{};
    std::shared_ptr<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion> testRegion{};
    std::shared_ptr<BG::NES::Simulator::Simulation> testSim{};

    float tol = 1e-3;

    void SetUp() {
        testSim = std::make_shared<BG::NES::Simulator::Simulation>(nullptr);  // this will fail an assert when we try and use the logger fyi
        testNC = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(4000, 2, &Collection);
        auto testBSAlignedNC = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(testNC);
        testRegion = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion>(3000, &Collection.AddBox(BG::NES::Simulator::Geometries::Vec3D(0.0, 0.0, 0.0), BG::NES::Simulator::Geometries::Vec3D(20.0, 20.0, 20.0)), testBSAlignedNC);

        testSim->AddCircuit(testNC);
        testSim->AddRegion(testRegion);

        testElectrode = std::make_unique<BG::NES::Simulator::Tools::RecordingElectrode>(testSim.get());
    }

    void TearDown() { return; }
};

//  std::unordered_map<std::string, std::vector<float>> GetRecording();

TEST_F(RecordingElectrodeTest, test_CoordsElectrodeToSystem_default) {
    BG::NES::Simulator::Geometries::Vec3D eLocRatio(0.0, 0.5, 0.2);
    auto toAdd = (testElectrode->TipPosition_um - testElectrode->EndPosition_um) * eLocRatio.z;
    auto expectedSysLoc_um = testElectrode->TipPosition_um + toAdd;

    ASSERT_TRUE(testElectrode->CoordsElectrodeToSystem(eLocRatio) == expectedSysLoc_um);
}

TEST_F(RecordingElectrodeTest, test_InitSystemCoordSiteLocations_default) {
    ASSERT_FALSE(testElectrode->SiteLocations_um.empty());
    ASSERT_EQ(testElectrode->SiteLocations_um.size(), testElectrode->Sites.size());
}

TEST_F(RecordingElectrodeTest, test_InitNeuronReferencesAndDistances_default) {
    size_t expectedSize = (testElectrode->Sim->GetAllNeurons()).size();
    ASSERT_FALSE(testElectrode->Neurons.empty());
    ASSERT_EQ(testElectrode->Neurons.size(), expectedSize);

    expectedSize = testElectrode->SiteLocations_um.size();
    ASSERT_FALSE(testElectrode->NeuronSomaToSiteDistances_um2.empty());
    ASSERT_EQ(testElectrode->NeuronSomaToSiteDistances_um2.size(), expectedSize);
}

TEST_F(RecordingElectrodeTest, test_InitRecords_default) {
    testElectrode->InitRecords();
    ASSERT_EQ(testElectrode->E_mV.size(), testElectrode->Sites.size());
}

TEST_F(RecordingElectrodeTest, test_AddNoise_default) {
    float lowerLim = -0.5 * testElectrode->NoiseLevel;
    float upperLim = 0.5 * testElectrode->NoiseLevel;
    float gotNoise = testElectrode->AddNoise();
    ASSERT_TRUE(lowerLim <= gotNoise && gotNoise <= upperLim);
}

TEST_F(RecordingElectrodeTest, test_ElectricFieldPotential_default) {
    float gotE_mV{};

    // Site index is out of bounds
    try {
        gotE_mV = testElectrode->ElectricFieldPotential(testElectrode->NeuronSomaToSiteDistances_um2.size());
        FAIL();
    } catch (std::out_of_range &ex) {
        ASSERT_STREQ("Out of bounds. (siteIdx)", ex.what());
    }

    // Sensitivity Dampening is zero
    testElectrode->SensitivityDampening = 0.0;
    try {
        gotE_mV = testElectrode->ElectricFieldPotential(0);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero. (SensitivityDampening)", ex.what());
    }

    // Correct Site Index and nonzero Sensitivity Dampening
    size_t siteIdx = 0;
    float expectedE_mV = 0.0;
    auto siteDistances_um = testElectrode->NeuronSomaToSiteDistances_um2[siteIdx];

    testElectrode->SensitivityDampening = 2.0;

    for (size_t i = 0; i < testElectrode->Neurons.size(); ++i) {
        float Vm_mV = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(testElectrode->Neurons[i])->Vm_mV;

        if (siteDistances_um[i] <= 1.0)
            expectedE_mV = Vm_mV;
        else {
            expectedE_mV = Vm_mV / siteDistances_um[i];
            expectedE_mV += (expectedE_mV / testElectrode->SensitivityDampening);
        }
    }
    float lowerLim = expectedE_mV - 0.5 * testElectrode->NoiseLevel;
    float upperLim = expectedE_mV + 0.5 * testElectrode->NoiseLevel;

    gotE_mV = testElectrode->ElectricFieldPotential(0);
    ASSERT_TRUE(lowerLim <= gotE_mV && gotE_mV <= upperLim);
}

TEST_F(RecordingElectrodeTest, test_Record_default) {
    // Nothing has been recorded immediately after setup
    ASSERT_TRUE(testElectrode->TRecorded_ms.empty());
    for (const auto &E_mVVec : testElectrode->E_mV) ASSERT_TRUE(E_mVVec.empty());

    float recordingTime_ms = 1.0;
    testElectrode->Record(recordingTime_ms);

    ASSERT_FALSE(testElectrode->TRecorded_ms.empty());
    ASSERT_EQ(testElectrode->TRecorded_ms.back(), recordingTime_ms);
    for (const auto &E_mVVec : testElectrode->E_mV) ASSERT_FALSE(E_mVVec.empty());
}

TEST_F(RecordingElectrodeTest, test_GetRecording_default) {
    auto data = testElectrode->GetRecording();

    // No data immediately after setup
    auto it = data.find("E_mV");
    ASSERT_TRUE(it != data.end());
    for (const auto &E_mVVec : data.at("E_mV")) ASSERT_TRUE(E_mVVec.empty());

    // Data available after recording for some time.
    testElectrode->Record(0.1);
    testElectrode->Record(0.4);
    testElectrode->Record(0.2);
    data = testElectrode->GetRecording();
    for (const auto &E_mVVec : data.at("E_mV")) {
        ASSERT_FALSE(E_mVVec.empty());
        ASSERT_EQ(E_mVVec.size(), 3);
    }
}
