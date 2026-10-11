//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
     Description: Minimal salvage of the unit tests for the Simulation struct,
                  rewritten against the current API (P0-4). Broad Simulation
                  coverage is the job of the NES test-coverage initiative (NT-5).
     Date Created: 2023-10-22
*/

#include <filesystem>
#include <memory>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Util/StoragePaths.h>

using BG::NES::Simulator::Simulation;

struct SimulationTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<Simulation> testSimulation{};
    std::filesystem::path TmpDir;

    void SetUp() {
        TmpDir = std::filesystem::temp_directory_path() / "nes_simulation_test";
        std::filesystem::create_directories(TmpDir);
        BG::NES::Util::Storage::SetOutputBasePath(TmpDir.string());
        testSimulation = std::make_unique<Simulation>(&Logger);
    }

    void TearDown() {
        testSimulation.reset();
        std::error_code ec;
        std::filesystem::remove_all(TmpDir, ec);
    }
};

TEST_F(SimulationTest, test_SetRecordAll_default) {
    // At T=0: record forever
    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms, _RECORD_FOREVER_TMAX_MS);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, testSimulation->T_ms);

    // Finite window
    testSimulation->SetRecordAll(100.0);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms, 100.0);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, testSimulation->T_ms);

    // Zero turns recording off
    testSimulation->SetRecordAll(0.0);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms, 0.0);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, 0.0);
}

TEST_F(SimulationTest, test_IsRecording_default) {
    testSimulation->SetRecordAll(0.0);
    ASSERT_FALSE(testSimulation->IsRecording());

    testSimulation->SetRecordAll(100.0);
    ASSERT_TRUE(testSimulation->IsRecording());

    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    ASSERT_TRUE(testSimulation->IsRecording());
}

TEST_F(SimulationTest, test_RunFor_nonpositive_is_noop) {
    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);

    testSimulation->RunFor(-10.0);
    ASSERT_EQ(testSimulation->T_ms, 0.0f);
    ASSERT_TRUE(testSimulation->TRecorded_ms.empty());

    testSimulation->RunFor(0.0);
    ASSERT_EQ(testSimulation->T_ms, 0.0f);
    ASSERT_TRUE(testSimulation->TRecorded_ms.empty());
}

TEST_F(SimulationTest, test_AddShapes_assign_sequential_ids) {
    BG::NES::Simulator::Geometries::Sphere sphere({0.0, 0.0, 0.0}, 1.0);
    BG::NES::Simulator::Geometries::Cylinder cylinder(0.5, {0.0, 0.0, 0.0}, 0.5, {0.0, 10.0, 0.0});
    BG::NES::Simulator::Geometries::Box box({0.0, 0.0, 0.0}, {2.0, 2.0, 2.0});

    ASSERT_EQ(testSimulation->AddSphere(sphere), 0);
    ASSERT_EQ(testSimulation->AddCylinder(cylinder), 1);
    ASSERT_EQ(testSimulation->AddBox(box), 2);

    // The returned ID is written back to the argument and is the index in the collection.
    ASSERT_EQ(sphere.ID, 0);
    ASSERT_EQ(cylinder.ID, 1);
    ASSERT_EQ(box.ID, 2);
    ASSERT_EQ(testSimulation->Collection.Geometries.size(), 3u);
}

TEST_F(SimulationTest, test_AddCircuit_and_AddRegion_ids) {
    BG::NES::Simulator::CoreStructs::NeuralCircuit circuit(&testSimulation->Collection);
    ASSERT_EQ(testSimulation->AddCircuit(circuit), 0);
    ASSERT_EQ(testSimulation->NeuralCircuits.size(), 1u);

    // Adding a region also adds a circuit for it.
    BG::NES::Simulator::BrainRegions::BrainRegion region;
    ASSERT_EQ(testSimulation->AddRegion(region), 0);
    ASSERT_EQ(testSimulation->Regions.size(), 1u);
    ASSERT_EQ(testSimulation->NeuralCircuits.size(), 2u);
    ASSERT_EQ(region.CircuitID, 1);
}
