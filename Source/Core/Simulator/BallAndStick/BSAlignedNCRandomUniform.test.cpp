//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSAlignedNCRandomUniform
   struct.

    Additional Notes: None

    Date Created: 2023-10-30
*/

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSAlignedNCRandomUniform.h>

/**
 * @brief Test class for unit tests for struct BSAlignedNCRandomUniform.
 *
 */

struct BSAlignedNCRandomUniformTest : testing::Test {
    BG::NES::Simulator::Geometries::GeometryCollection Collection;

    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSAlignedNCRandomUniform> testNC = nullptr;

    float tol = 1e-3;

    void SetUp() {
        testNC = std::make_unique<BG::NES::Simulator::BallAndStick::BSAlignedNCRandomUniform>(100, &Collection);
    }

    // Initialize cells in the neural circuit.
    void InitCells() {
        BG::NES::Simulator::Geometries::Box * domain = &Collection.AddBox();
        testNC->InitCells(domain);
    }

    void TearDown() { return; }
};

TEST_F(BSAlignedNCRandomUniformTest, test_InitCells_default) {
    // No stored cells for uninitialized cells.
    ASSERT_TRUE(testNC->Cells.empty());

    InitCells();

    ASSERT_EQ(testNC->Cells.size(), testNC->NumCells);
}
