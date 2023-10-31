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
    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSAlignedNCRandomUniform>
        testNC = nullptr;

    float tol = 1e-3;

    void SetUp() {
        testNC = std::make_unique<
            BG::NES::Simulator::BallAndStick::BSAlignedNCRandomUniform>(100);
    }

    // Initialize cells in the neural circuit.
    void InitCells() {
        std::shared_ptr<BG::NES::Simulator::Geometries::Box> domain =
            std::make_shared<BG::NES::Simulator::Geometries::Box>();
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
