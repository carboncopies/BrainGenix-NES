//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the RecordingElectrode
   struct. Additional Notes: None Date Created: 2023-10-13
*/

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>
#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/Simulation.h>

/**
 * @brief Test class for unit tests for the RecordingElectrode struct.
 *
 */

struct RecordingElectrodeTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Tools::RecordingElectrode>
        testElectrode{};
    std::shared_ptr<BG::NES::Simulator::CoreStructs::NeuralCircuit> testNC{};
    std::shared_ptr<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion>
        testRegion{};
    std::shared_ptr<BG::NES::Simulator::Simulation> testSim{};

    float tol = 1e-3;

    void SetUp() {
        testSim = std::make_shared<BG::NES::Simulator::Simulation>();
        testNC =
            std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(
                4000, 2);
        auto testBSAlignedNC = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSAlignedNC>(testNC);
        testRegion = std::make_shared<
            BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion>(
            3000,
            std::make_shared<BG::NES::Simulator::Geometries::Box>(
                BG::NES::Simulator::Geometries::Vec3D(0.0, 0.0, 0.0),
                BG::NES::Simulator::Geometries::Vec3D(20.0, 20.0, 20.0)),
            testBSAlignedNC);

        testSim->AddCircuit(testNC);
        testSim->AddRegion(testRegion);

        testElectrode =
            std::make_unique<BG::NES::Simulator::Tools::RecordingElectrode>(
                testSim);
    }

    void TearDown() { return; }
};

//  void InitRecords();
//  float AddNoise();
//  float ElectricFieldPotential(int siteIdx);
//  void Record(float t_ms);
//  std::unordered_map<std::string, std::vector<float>> GetRecording();

TEST_F(RecordingElectrodeTest, test_CoordsElectrodeToSystem_default) {
    auto eLocRatio = std::make_tuple(0.0, 0.5, 0.2);
    auto toAdd =
        (testElectrode->TipPosition_um - testElectrode->EndPosition_um) *
        std::get<2>(eLocRatio);
    auto expectedSysLoc_um = testElectrode->TipPosition_um + toAdd;

    ASSERT_TRUE(testElectrode->CoordsElectrodeToSystem(eLocRatio) ==
                expectedSysLoc_um);
}

TEST_F(RecordingElectrodeTest, test_InitSystemCoordSiteLocations_default) {
    ASSERT_FALSE(testElectrode->SiteLocations_um.empty());
    ASSERT_EQ(testElectrode->SiteLocations_um.size(),
              testElectrode->Sites.size());
}

// TEST_F(RecordingElectrodeTest, test_InitNeuronReferencesAndDistances_default)
// {
//     size_t expectedSize =
//     (testElectrode->Sim->GetAllNeurons()).size();
//     ASSERT_FALSE(testElectrode->Neurons.empty());
//     ASSERT_EQ(testElectrode->Neurons.size(), expectedSize);
//
//     expectedSize = testElectrode->SiteLocations_um.size();
//     ASSERT_FALSE(testElectrode->NeuronSomaToSiteDistances_um2.empty());
//     ASSERT_EQ(testElectrode->NeuronSomaToSiteDistances_um2.size(),
//               expectedSize);
// }

TEST_F(RecordingElectrodeTest, test_InitRecords_default) {}

TEST_F(RecordingElectrodeTest, test_AddNoise_default) {}
TEST_F(RecordingElectrodeTest, test_ElectricFieldPotential_default) {}
TEST_F(RecordingElectrodeTest, test_Record_default) {}
TEST_F(RecordingElectrodeTest, test_GetRecording_default) {}
