//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the functions in the
                 BSMorphology.h header.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSMorphology.h>

/**
 * @brief Test class for unit tests for functions used to create morphologies
 * for ball and stick neural circuits.
 *
 */

struct BSMorphologyTest : testing::Test {
    float tol = 1e-3;

    void SetUp() {}

    void TearDown() { return; }
};

TEST_F(BSMorphologyTest, test_CreateBSSoma_default) {
    BG::NES::Simulator::BallAndStick::Align align =
        BG::NES::Simulator::BallAndStick::Align::ALIGN_CENTER;
    std::vector<std::vector<float>> domainBounds = {{0.0, 0.0, 0.0},
                                                    {1.0, 1.0, 1.0}};
    std::shared_ptr<BG::NES::Simulator::Geometries::Sphere> soma =
        CreateBSSoma(domainBounds, align);

    ASSERT_EQ(soma->Radius_um, 0.5f);
    ASSERT_NEAR(soma->Center_um.x_um, 0.5f, tol);
    ASSERT_NEAR(soma->Center_um.y_um, 0.5f, tol);
    ASSERT_NEAR(soma->Center_um.z_um, 0.5f, tol);
}

TEST_F(BSMorphologyTest, test_CreateBSAxon_default) {
    BG::NES::Simulator::BallAndStick::Align align =
        BG::NES::Simulator::BallAndStick::Align::ALIGN_LEFT;
    std::vector<std::vector<float>> domainBounds = {{0.0, 0.0, 0.0},
                                                    {1.0, 1.0, 1.0}};
    std::shared_ptr<BG::NES::Simulator::Geometries::Cylinder> axon =
        CreateBSAxon(domainBounds, align, 0.4);

    ASSERT_EQ(axon->End0Radius_um, 0.1f);
    ASSERT_EQ(axon->End1Radius_um, 0.1f);
    ASSERT_NEAR(axon->End0Pos_um.x_um, 0.5f, tol);
    ASSERT_NEAR(axon->End0Pos_um.y_um, 0.0f, tol);
    ASSERT_NEAR(axon->End0Pos_um.z_um, 0.5f, tol);
    ASSERT_NEAR(axon->End1Pos_um.x_um, 0.5f, tol);
    ASSERT_NEAR(axon->End1Pos_um.y_um, 0.2f, tol);
    ASSERT_NEAR(axon->End1Pos_um.z_um, 0.5f, tol);
}

TEST_F(BSMorphologyTest, test_CreateBSReceptor_default) {
    BG::NES::Simulator::Geometries::Vec3D receptorLocation{0.1f, 0.2f, 0.3f};
    std::shared_ptr<BG::NES::Simulator::Geometries::Box> receptor =
        BG::NES::Simulator::BallAndStick::CreateBSReceptor(receptorLocation);

    ASSERT_EQ(receptor->Dims_um.x_um, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor->Dims_um.y_um, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor->Dims_um.z_um, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor->Center_um.x_um, 0.1f);
    ASSERT_EQ(receptor->Center_um.y_um, 0.2f);
    ASSERT_EQ(receptor->Center_um.z_um, 0.3f);
}
