//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Minimal salvage of the unit tests for the RecordingElectrode
                 struct, rewritten against the current API (P0-4). Broad
                 coverage is the job of the NES test-coverage initiative.
    Date Created: 2023-10-13
*/

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/Simulation.h>

using BG::NES::Simulator::Geometries::Vec3D;
using BG::NES::Simulator::Tools::RecordingElectrode;

struct RecordingElectrodeTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<BG::NES::Simulator::Simulation> testSim{};
    std::unique_ptr<RecordingElectrode> testElectrode{};

    std::vector<Vec3D> sites{Vec3D(0.0, 0.0, 0.0), Vec3D(0.0, 0.5, 0.2), Vec3D(0.0, 0.0, 1.0)};

    void SetUp() {
        testSim = std::make_unique<BG::NES::Simulator::Simulation>(&Logger);
        // Empty simulation (no neurons): enough to exercise the site bookkeeping.
        testElectrode = std::make_unique<RecordingElectrode>(
            0, Vec3D(0.0, 0.0, 0.0), Vec3D(0.0, 0.0, 5.0), sites, 1.0f, 2.0f, testSim.get());
    }
};

TEST_F(RecordingElectrodeTest, test_CoordsElectrodeToSystem_default) {
    Vec3D eLocRatio(0.0, 0.5, 0.2);
    auto toAdd = (testElectrode->TipPosition_um - testElectrode->EndPosition_um) * eLocRatio.z;
    auto expectedSysLoc_um = testElectrode->TipPosition_um + toAdd;

    ASSERT_TRUE(testElectrode->CoordsElectrodeToSystem(eLocRatio) == expectedSysLoc_um);
}

TEST_F(RecordingElectrodeTest, test_site_count_invariant) {
    ASSERT_EQ(testElectrode->Sites.size(), sites.size());
    ASSERT_EQ(testElectrode->SiteLocations_um.size(), testElectrode->Sites.size());
    ASSERT_EQ(testElectrode->NeuronSomaToSiteDistances_um2.size(), testElectrode->Sites.size());
    ASSERT_EQ(testElectrode->E_mV.size(), testElectrode->Sites.size());
}

TEST_F(RecordingElectrodeTest, test_AddNoise_range) {
    float lowerLim = -0.5 * testElectrode->NoiseLevel;
    float upperLim = 0.5 * testElectrode->NoiseLevel;
    for (int i = 0; i < 200; ++i) {
        float gotNoise = testElectrode->AddNoise();
        ASSERT_TRUE(lowerLim <= gotNoise && gotNoise <= upperLim);
    }
}
