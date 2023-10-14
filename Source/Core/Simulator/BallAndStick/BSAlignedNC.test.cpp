//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSAlignedNC struct.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Geometries/Box.h>

/**
 * @brief Test class for unit tests for struct BSAlignedNC.
 *
 */

struct BSAlignedNCTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSAlignedNC>
        testBSAlignedNC = nullptr;

    float axonRadius_um = 0.5;
    float somaRadius_um = 0.5;
    std::vector<float> times_ms = {0.1, 1.5, 0.2, 0.3,
                                   1.6}; //! Time values used for simulation

    BG::NES::Simulator::Geometries::Vec3D axonEnd0_um{0.0, 0.0, 0.0};
    BG::NES::Simulator::Geometries::Vec3D axonEnd1_um{0.0, 10.0, 0.0};

    std::shared_ptr<BG::NES::Simulator::Geometries::Sphere> testSoma =
        std::make_shared<BG::NES::Simulator::Geometries::Sphere>(somaRadius_um);
    std::shared_ptr<BG::NES::Simulator::Geometries::Cylinder> testAxon =
        std::make_shared<BG::NES::Simulator::Geometries::Cylinder>(
            axonRadius_um, axonEnd0_um, axonRadius_um, axonEnd1_um);
    float tol = 1e-3;

    void SetUp() {
        testBSAlignedNC =
            std::make_unique<BG::NES::Simulator::BallAndStick::BSAlignedNC>(
                100);
    }

    // Initialize cells in the neural circuit.
    void InitCells() {
        std::shared_ptr<BG::NES::Simulator::Geometries::Box> domain =
            std::make_shared<BG::NES::Simulator::Geometries::Box>();
        testBSAlignedNC->InitCells(domain);
    }

    void TearDown() { return; }
};

TEST_F(BSAlignedNCTest, test_InitCells_default) {
    // No cells for uninitialized cells.
    ASSERT_TRUE(testBSAlignedNC->Cells.empty());

    InitCells();

    ASSERT_EQ(testBSAlignedNC->Cells.size(), testBSAlignedNC->NumCells);
}
