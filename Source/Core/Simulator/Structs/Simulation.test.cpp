//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
     Description: This file provides unit tests for the Simulation struct.
     Additional Notes: None
     Date Created: 2023-10-22
*/

#include <algorithm>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>
#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Distributions/TruncNorm.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>


/**
 * @brief Test class for unit tests for the Simulation struct.
 *
 */

struct SimulationTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    //Logger.SetKeepVectorLogs(false);

    std::unique_ptr<BG::NES::Simulator::Simulation> testSimulation{};
    std::shared_ptr<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion> testRegion{};

    float tol = 1e-3;

    void SetUp() {
        testSimulation = std::make_unique<BG::NES::Simulator::Simulation>(&Logger);
    }

    void InitNCAndBrainRegion() {
        std::shared_ptr<BG::NES::Simulator::CoreStructs::NeuralCircuit> testNC = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(4000, &testSimulation->Collection);

        auto testBSAlignedNC = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(testNC);
        BG::NES::Simulator::Geometries::Box * testBox = &testSimulation->Collection.AddBox();
        std::shared_ptr<BG::NES::Simulator::BrainRegions::BrainRegion> testRegion = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion>(3000, testBox, testBSAlignedNC);

        testBSAlignedNC->InitCells(testBox);

        testSimulation->AddCircuit(testNC);
        testSimulation->AddRegion(testRegion);
    }

    void TearDown() { return; }
};

TEST_F(SimulationTest, test_AddCircuit_default) {
    auto testNC = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(4000, 2, &testSimulation->Collection);
    testSimulation->AddCircuit(testNC);

    ASSERT_EQ(testSimulation->NeuralCircuits.at(std::to_string(testNC->ID)), testNC);
}

TEST_F(SimulationTest, test_AddRegion_default) {
    auto testNC = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(4000, 2, &testSimulation->Collection);
    auto testBox = &testSimulation->Collection.AddBox(
        BG::NES::Simulator::Geometries::Vec3D(1.0, -1.0, 1.0),
        BG::NES::Simulator::Geometries::Vec3D(20.0, 20.0, 20.0));
    std::shared_ptr<BG::NES::Simulator::BrainRegions::BrainRegion> testRegion = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedBrainRegion>(3000, testBox, testNC);

    testNC->InitCells(testBox);
    testSimulation->AddCircuit(testNC);
    testSimulation->AddRegion(testRegion);

    ASSERT_EQ(testSimulation->Regions.at(std::to_string(3000)), testRegion);
}

TEST_F(SimulationTest, test_GetAllNeurons_default) {
    InitNCAndBrainRegion();

    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>> expectedNeurons{};
    auto allNeurons = testSimulation->GetAllNeurons();

    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto neurons = circuit->GetNeurons();
        for (auto neuron : neurons)
            expectedNeurons.emplace_back(neuron);
    }

    ASSERT_EQ(allNeurons.size(), expectedNeurons.size());
    for (auto neuron : allNeurons) {
        auto it =
            std::find(expectedNeurons.begin(), expectedNeurons.end(), neuron);
        ASSERT_NE(it, expectedNeurons.end());
    }
}

TEST_F(SimulationTest, test_GetAllNeuronIDs_default) {
    InitNCAndBrainRegion();

    std::vector<size_t> allNeuronIDs = testSimulation->GetAllNeuronIDs();
    std::vector<size_t> expectedNeuronIDs{};

    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto neurons = circuit->GetNeurons();
        for (auto neuron : neurons)
            expectedNeuronIDs.emplace_back(neuron->ID);
    }

    ASSERT_FALSE(allNeuronIDs.empty());

    ASSERT_EQ(allNeuronIDs.size(), expectedNeuronIDs.size());
    for (size_t neuronID : expectedNeuronIDs) {
        auto it = std::find(allNeuronIDs.begin(), allNeuronIDs.end(), neuronID);
        ASSERT_NE(it, allNeuronIDs.end());
    }
}

TEST_F(SimulationTest, test_GetNeuronsByIDs_default) {
    InitNCAndBrainRegion();

    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>> listedNeurons{}, expectedNeurons{};
    std::vector<size_t> IDs{};

    // Case 1: Empty list of IDs
    listedNeurons = testSimulation->GetNeuronsByIDs(IDs);
    ASSERT_TRUE(listedNeurons.empty());

    // Case 2 : List of IDs contains neuron IDs not in system
    IDs.emplace_back(98u);
    IDs.emplace_back(99u);
    try {
        listedNeurons = testSimulation->GetNeuronsByIDs(IDs);
        FAIL();
    } catch (std::out_of_range &ex) {
        ASSERT_STREQ(ex.what(), "unordered_map::at");
    }
    ASSERT_TRUE(listedNeurons.empty()) << listedNeurons.size();

    // 3: List of IDs is correct
    IDs.clear();
    IDs.emplace_back(0u);
    IDs.emplace_back(1u);
    IDs = testSimulation->GetAllNeuronIDs();

    listedNeurons = testSimulation->GetNeuronsByIDs(IDs);
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto neurons = circuit->GetNeuronsByIDs(IDs);
        for (auto neuron : neurons)
            expectedNeurons.emplace_back(neuron);
    }

    ASSERT_EQ(listedNeurons.size(), expectedNeurons.size());

    for (auto neuron : listedNeurons) {
        auto it =
            std::find(expectedNeurons.begin(), expectedNeurons.end(), neuron);
        ASSERT_NE(it, expectedNeurons.end());
    }
}

TEST_F(SimulationTest, test_GetGeoCenter_default) {
    InitNCAndBrainRegion();

    std::shared_ptr<BG::NES::Simulator::CoreStructs::NeuralCircuit> testNC = std::make_shared<BG::NES::Simulator::BallAndStick::BSAlignedNC>(5000, &testSimulation->Collection);
    auto testBSAlignedNC = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(testNC);
    BG::NES::Simulator::Geometries::Box * testBox = &testSimulation->Collection.AddBox(
            BG::NES::Simulator::Geometries::Vec3D(1.0, -1.0, 1.0),
            BG::NES::Simulator::Geometries::Vec3D(5.0, 10.0, 10.0));

    testBSAlignedNC->InitCells(testBox);
    testSimulation->AddCircuit(testNC);

    BG::NES::Simulator::Geometries::Vec3D expectedGeoCenter{0.5, -2.5, 0.5};
    auto gotGeoCenter = testSimulation->GetGeoCenter();
    ASSERT_EQ(gotGeoCenter, expectedGeoCenter);
}

TEST_F(SimulationTest, test_AttachDirectStim_default) {
    InitNCAndBrainRegion();

    std::vector<std::tuple<float, size_t>> listOfStims{};

    // Case 1: An empty list of stimuli
    testSimulation->AttachDirectStim(listOfStims);
    for (auto &[circuitId, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        for (auto &[cellID, cell] : circuit->Cells) {
            auto cellPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
            ASSERT_TRUE(cellPtr->TDirectStim_ms.empty());
        }
    }

    // Case 2: A wrong cell_id is provided
    listOfStims.emplace_back(std::make_tuple(0.1f, 99u));
    try {
        testSimulation->AttachDirectStim(listOfStims);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ(ex.what(), "Cell not found.");
    }

    // Case 3: All arguments in list are correct
    listOfStims.clear();
    listOfStims.emplace_back(std::make_tuple(0.1f, 0u));
    listOfStims.emplace_back(std::make_tuple(0.1f, 1u));
    testSimulation->AttachDirectStim(listOfStims);

    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {

        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        for (auto &[cellID, cell] : circuitPtr->Cells) {

            auto cellPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
            ASSERT_EQ(cellPtr->TDirectStim_ms.back(), 0.1f);
        }
    }
}

TEST_F(SimulationTest, test_SetSpontaneousActivity_default) {
    InitNCAndBrainRegion();

    std::vector<std::tuple<float, float, size_t>> spontSpikeSettings{};

    // Case 1: An empty list is provided
    testSimulation->SetSpontaneousActivity(spontSpikeSettings);
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        for (auto &[cellID, cell] : circuitPtr->Cells) {
            auto cellPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
            ASSERT_EQ(cellPtr->DtSpontDist, nullptr);
        }
    }

    // Case 2: A wrong cell ID is provided
    spontSpikeSettings.clear();
    spontSpikeSettings.emplace_back(std::make_tuple(290.0f, 140.0f, 99u));
    try {
        testSimulation->SetSpontaneousActivity(spontSpikeSettings);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ(ex.what(), "Cell not found.");
    }

    // Case 3: All parameters are valid.
    spontSpikeSettings.clear();
    spontSpikeSettings.emplace_back(std::make_tuple(280.0f, 140.0f, 0u));
    testSimulation->SetSpontaneousActivity(spontSpikeSettings);
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        float mu = 280.0, sigma = 140.0;
        auto testDist = BG::NES::Simulator::Distributions::TruncNorm(-mu / sigma, mu / sigma, mu, sigma);
        auto cell = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(circuitPtr->Cells.at("0"));
        auto gotStats = cell->DtSpontDist->Stats();
        auto expectedStats = testDist.Stats();

        ASSERT_EQ(expectedStats, gotStats);
    }
}

TEST_F(SimulationTest, test_SetRecordAll_default) {

    InitNCAndBrainRegion();

    // Case 1: Record forever
    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms,_RECORD_FOREVER_TMAX_MS);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, testSimulation->T_ms);

    // Case 2: Record for a finite time
    testSimulation->SetRecordAll(100.0);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms, 100.0);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, testSimulation->T_ms);

    // Case 3: Record for zero time
    testSimulation->SetRecordAll(0.0);
    ASSERT_EQ(testSimulation->MaxRecordTime_ms, 0.0);
    ASSERT_EQ(testSimulation->StartRecordTime_ms, 0.0);
}

TEST_F(SimulationTest, test_IsRecording_default) {
    InitNCAndBrainRegion();

    // 1: System is not set to record anything
    testSimulation->SetRecordAll(0.0);
    ASSERT_FALSE(testSimulation->IsRecording());

    // Case 2: System is set to record for a finite time
    testSimulation->SetRecordAll(100.0);
    ASSERT_TRUE(testSimulation->IsRecording());

    // Case 3: System is set to record everything
    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    ASSERT_TRUE(testSimulation->IsRecording());
}

TEST_F(SimulationTest, test_GetRecording_default) {
    InitNCAndBrainRegion();

    // Nothing has been recorded immediately after setup.
    auto data = testSimulation->GetRecording();

    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto it = data.find(circuitID);
        ASSERT_NE(it, data.end());
        auto circuitData = data.at(circuitID);

        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        for (auto &[cellID, cell] : circuitPtr->Cells) {
            auto it2 = circuitData.find(cellID);
            ASSERT_NE(it2, circuitData.end());
            ASSERT_TRUE((circuitData.at(cellID)).at("Vm_mV").empty());
        }
    }

    // After a simulation has been run, the data must be collected
    // and correctly returned.

    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    testSimulation->RunFor(10.0);
    data = testSimulation->GetRecording();

    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto it = data.find(circuitID);
        ASSERT_NE(it, data.end());
        auto circuitData = data.at(circuitID);

        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        for (auto &[cellID, cell] : circuitPtr->Cells) {
            auto it2 = circuitData.find(cellID);
            ASSERT_NE(it2, circuitData.end());
            ASSERT_FALSE((circuitData.at(cellID)).at("Vm_mV").empty());
        }
    }
}

TEST_F(SimulationTest, test_RunFor_default) {
    InitNCAndBrainRegion();

    testSimulation->SetRecordAll(_RECORD_FOREVER_TMAX_MS);

    // Case 1: Time period to run simulation for is invalid (e.g. negative).
    testSimulation->RunFor(-10.0);
    auto data = testSimulation->GetRecording();
    ASSERT_TRUE(testSimulation->TRecorded_ms.empty());
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        auto circuitData = data.at(circuitID);
        for (auto &[cellID, cell] : circuitPtr->Cells)
            ASSERT_TRUE(((circuitData.at(cellID)).at("Vm_mV")).empty());
    }

    // Case 2: Time period to run simulation for is zero.
    testSimulation->RunFor(0.0);
    data = testSimulation->GetRecording();
    ASSERT_TRUE(testSimulation->TRecorded_ms.empty());
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        auto circuitData = data.at(circuitID);
        for (auto &[cellID, cell] : circuitPtr->Cells)
            ASSERT_TRUE(((circuitData.at(cellID)).at("Vm_mV")).empty());
    }

    // Case 3: Time period to run simulation for is positive.
    testSimulation->RunFor(10.0);
    data = testSimulation->GetRecording();
    ASSERT_FALSE(testSimulation->TRecorded_ms.empty());
    for (auto &[circuitID, circuit] : testSimulation->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        auto circuitData = data.at(circuitID);
        for (auto &[cellID, cell] : circuitPtr->Cells)
            ASSERT_FALSE(((circuitData.at(cellID)).at("Vm_mV")).empty());
    }
}
