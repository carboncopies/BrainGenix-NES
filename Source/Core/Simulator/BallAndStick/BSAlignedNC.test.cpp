//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSAlignedNC struct.
    Additional Notes: None
    Date Created: 2023-10-14
*/
#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Distributions/TruncNorm.h>
#include <Simulator/Geometries/Box.h>

/**
 * @brief Test class for unit tests for struct BSAlignedNC.
 *
 */

struct BSAlignedNCTest : testing::Test {
    BG::NES::Simulator::Geometries::GeometryCollection Collection;

    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSAlignedNC> testBSAlignedNC = nullptr;

    float tol = 1e-3;

    void SetUp() {
        testBSAlignedNC = std::make_unique<BG::NES::Simulator::BallAndStick::BSAlignedNC>(100, &Collection);
    }

    // Initialize cells in the neural circuit.
    void InitCells() {
        BG::NES::Simulator::Geometries::Box * domain = &Collection.AddBox();
        testBSAlignedNC->InitCells(domain);
    }

    void TearDown() { return; }
};

TEST_F(BSAlignedNCTest, test_InitCells_default) {
    // No stored cells for uninitialized cells.
    ASSERT_TRUE(testBSAlignedNC->Cells.empty());

    InitCells();

    ASSERT_EQ(testBSAlignedNC->Cells.size(), testBSAlignedNC->NumCells);
}

TEST_F(BSAlignedNCTest, test_GetCellCenters_default) {
    auto cellCenters = testBSAlignedNC->GetCellCenters();

    ASSERT_TRUE(cellCenters.empty());

    InitCells();

    cellCenters = testBSAlignedNC->GetCellCenters();
    ASSERT_EQ(cellCenters.size(), 2);
}

TEST_F(BSAlignedNCTest, test_SetWeight_default) {
    size_t from, to;
    auto method = BG::NES::Simulator::BallAndStick::BSAlignedNC::SetWeightMethod::BINARY;
    std::shared_ptr<BG::NES::Simulator::BallAndStick::BSNeuron> sourceCell{}, targetCell{};

    InitCells();

    // Case 1: Method supplied is "binary", but either "from" or
    // "to" cells are invalid.

    from = 99;
    to = 1;

    try {
        testBSAlignedNC->SetWeight(from, to, method);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ("Unknown source cell.", ex.what());
    }

    from = 0;
    to = 99;

    try {
        testBSAlignedNC->SetWeight(from, to, method);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ("Unknown target cell.", ex.what());
    }

    // Case 2: All parameters supplied are valid
    from = 0;
    to = 1;

    testBSAlignedNC->SetWeight(
        from, to,
        BG::NES::Simulator::BallAndStick::BSAlignedNC::SetWeightMethod::BINARY);
    targetCell =
        std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(
            testBSAlignedNC->Cells[std::to_string(to)]);
    sourceCell =
        std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>(
            testBSAlignedNC->Cells[std::to_string(from)]);

    auto lastReceptorData = targetCell->ReceptorDataVec.back();

    //ASSERT_EQ(std::get<0>(lastReceptorData), sourceCell);
    //ASSERT_EQ(std::get<1>(lastReceptorData), 1.0);
    ASSERT_TRUE(targetCell->Morphology["receptor"]->Center_um ==
                sourceCell->Morphology["soma"]->Center_um);
}

TEST_F(BSAlignedNCTest, test_Encode_default) {
    std::vector<std::tuple<size_t, size_t>> patternSet;
    auto encodingMethod =
        BG::NES::Simulator::BallAndStick::BSAlignedNC::EncodingMethod::INSTANT;
    auto weightMethod =
        BG::NES::Simulator::BallAndStick::BSAlignedNC::SetWeightMethod::BINARY;

    InitCells();

    // Case 1: Empty pattern set
    testBSAlignedNC->Encode(patternSet, encodingMethod, weightMethod);

    // No receptors have been added to any cell
    for (const auto &[cellID, cell] : testBSAlignedNC->Cells) {
        auto bsCellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_TRUE(bsCellPtr->ReceptorDataVec.empty());
    }

    // Case 2: All parameters valid and non-empty pattern set
    patternSet.push_back(std::make_tuple(0, 1));
    testBSAlignedNC->Encode(patternSet, encodingMethod, weightMethod);

    for (const auto fromTo : patternSet) {
        auto targetCell = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(
            testBSAlignedNC->Cells[std::to_string(std::get<1>(fromTo))]);
        auto sourceCell = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(
            testBSAlignedNC->Cells[std::to_string(std::get<0>(fromTo))]);
        auto lastReceptorData = targetCell->ReceptorDataVec.back();

        //ASSERT_EQ(std::get<0>(lastReceptorData), sourceCell);
        //ASSERT_EQ(std::get<1>(lastReceptorData), 1.0);
        ASSERT_TRUE(targetCell->Morphology["receptor"]->Center_um ==
                    sourceCell->Morphology["soma"]->Center_um);
    }
}

TEST_F(BSAlignedNCTest, test_AttachDirectStim_default) {
    std::vector<std::tuple<float, size_t>> tStim_ms;

    InitCells();

    // Case 1: An empty list is passed.
    testBSAlignedNC->AttachDirectStim(tStim_ms);
    for (const auto &[cellId, cell] : testBSAlignedNC->Cells) {
        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_TRUE(cellPtr->TDirectStim_ms.empty());
    }

    // Case 2: A non-existent cell's ID is passed.
    tStim_ms.push_back(std::make_pair(0.1, 99));
    try {
        testBSAlignedNC->AttachDirectStim(tStim_ms);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ("Cell not found.", ex.what());
    }

    // Case 3: All parameters are valid.
    tStim_ms.clear();
    tStim_ms.push_back(std::make_tuple(0.1, 0));
    tStim_ms.push_back(std::make_tuple(0.1, 1));

    testBSAlignedNC->AttachDirectStim(tStim_ms);

    for (const auto &[cellId, cell] : testBSAlignedNC->Cells) {
        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_EQ(cellPtr->TDirectStim_ms.size(), 1);
    }
}

TEST_F(BSAlignedNCTest, test_SetSpontaneousActivity_default) {
    std::vector<std::tuple<float, float, size_t>> spontSpikeSettings;

    InitCells();

    // Case 1: An empty list is supplied.
    testBSAlignedNC->SetSpontaneousActivity(spontSpikeSettings);
    for (const auto &[cellID, cell] : testBSAlignedNC->Cells) {

        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_TRUE(cellPtr->DtSpontDist == nullptr);
    }

    // Case 2: A nonexistent cell ID is supplied.
    spontSpikeSettings.push_back(std::make_tuple(0.1f, 0.05f, 99u));
    try {
        testBSAlignedNC->SetSpontaneousActivity(spontSpikeSettings);
        FAIL();
    } catch (std::invalid_argument &ex) {
        ASSERT_STREQ("Cell not found.", ex.what());
    }

    // Case 3: All parameters are valid.
    spontSpikeSettings.clear();
    spontSpikeSettings.push_back(std::make_tuple(0.1f, 0.05f, 0u));
    spontSpikeSettings.push_back(std::make_tuple(0.15f, 0.1f, 1u));

    testBSAlignedNC->SetSpontaneousActivity(spontSpikeSettings);

    for (const auto &settings : spontSpikeSettings) {
        float mu = std::get<0>(settings);
        float sigma = std::get<1>(settings);
        size_t listedCellID = std::get<2>(settings);

        auto expectedDist = BG::NES::Simulator::Distributions::TruncNorm(
            -mu / sigma, mu / sigma, mu, sigma);

        auto cell = testBSAlignedNC->Cells[std::to_string(listedCellID)];
        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        auto expectedStats = expectedDist.Stats();
        auto gotStats = cellPtr->DtSpontDist->Stats();

        ASSERT_EQ(gotStats, expectedStats);
    }
}

TEST_F(BSAlignedNCTest, test_Update_default) {
    InitCells();

    testBSAlignedNC->Update(0.1, false);
    for (const auto &[cellID, cell] : testBSAlignedNC->Cells) {
        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_EQ(cellPtr->T_ms, 0.1f);
    }
}

TEST_F(BSAlignedNCTest, test_GetRecording_default) {
    InitCells();

    auto data = testBSAlignedNC->GetRecording();

    for (const auto &[cellID, cell] : testBSAlignedNC->Cells) {
        auto cellPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSNeuron>(cell);
        ASSERT_EQ(data[cellID], cellPtr->GetRecording());
    }
}

TEST_F(BSAlignedNCTest, test_GetNeurons_default) {
    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>>
        expectedNeurons{};
    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>>
        gotNeurons;

    for (auto &[cellID, cell] : testBSAlignedNC->Cells)
        expectedNeurons.push_back(cell);

    gotNeurons = testBSAlignedNC->GetNeurons();

    for (size_t i = 0; i < gotNeurons.size(); ++i) {
        auto it = std::find(expectedNeurons.begin(), expectedNeurons.end(),
                            gotNeurons[i]);
        ASSERT_NE(it, expectedNeurons.end());
    }
}

TEST_F(BSAlignedNCTest, test_GetNeuronsByIDs_default) {
    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>>
        expectedNeurons{};
    std::vector<std::shared_ptr<BG::NES::Simulator::CoreStructs::Neuron>>
        gotNeurons;
    std::vector<size_t> idList;

    // Case 1: IDList is empty.
    gotNeurons = testBSAlignedNC->GetNeuronsByIDs(idList);
    ASSERT_TRUE(gotNeurons.empty());

    // Case 2: IDList is nonempty.
    idList.push_back(0u);
    for (const size_t id : idList)
        expectedNeurons.push_back(testBSAlignedNC->Cells[std::to_string(id)]);

    gotNeurons = testBSAlignedNC->GetNeuronsByIDs(idList);

    for (size_t i = 0; i < gotNeurons.size(); ++i) {
        auto it = std::find(expectedNeurons.begin(), expectedNeurons.end(),
                            gotNeurons[i]);
        ASSERT_NE(it, expectedNeurons.end());
    }
}
