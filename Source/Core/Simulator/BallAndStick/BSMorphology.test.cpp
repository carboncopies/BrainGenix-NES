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
    BG::NES::Simulator::Geometries::GeometryCollection Collection;

    float tol = 1e-3;

    void SetUp() {}

    void TearDown() { return; }
};

TEST_F(BSMorphologyTest, test_CreateBSSoma_default) {
    BG::NES::Simulator::BallAndStick::Align align = BG::NES::Simulator::BallAndStick::Align::ALIGN_CENTER;
    std::vector<std::vector<float>> domainBounds = {{0.0, 0.0, 0.0},
                                                    {1.0, 1.0, 1.0}};
    BG::NES::Simulator::Geometries::Sphere * soma_ptr = &CreateBSSoma(Collection, domainBounds, align);

    ASSERT_EQ(soma_ptr->Radius_um, 0.5f);
    ASSERT_NEAR(soma_ptr->Center_um.x, 0.5f, tol);
    ASSERT_NEAR(soma_ptr->Center_um.y, 0.5f, tol);
    ASSERT_NEAR(soma_ptr->Center_um.z, 0.5f, tol);
}

TEST_F(BSMorphologyTest, test_CreateBSAxon_default) {
    BG::NES::Simulator::BallAndStick::Align align = BG::NES::Simulator::BallAndStick::Align::ALIGN_LEFT;
    std::vector<std::vector<float>> domainBounds = {{0.0, 0.0, 0.0},
                                                    {1.0, 1.0, 1.0}};
    BG::NES::Simulator::Geometries::Cylinder * axon_ptr = &CreateBSAxon(Collection, domainBounds, align, 0.4);

    ASSERT_EQ(axon_ptr->End0Radius_um, 0.1f);
    ASSERT_EQ(axon_ptr->End1Radius_um, 0.1f);
    ASSERT_NEAR(axon_ptr->End0Pos_um.x, 0.5f, tol);
    ASSERT_NEAR(axon_ptr->End0Pos_um.y, 0.0f, tol);
    ASSERT_NEAR(axon_ptr->End0Pos_um.z, 0.5f, tol);
    ASSERT_NEAR(axon_ptr->End1Pos_um.x, 0.5f, tol);
    ASSERT_NEAR(axon_ptr->End1Pos_um.y, 0.2f, tol);
    ASSERT_NEAR(axon_ptr->End1Pos_um.z, 0.5f, tol);
}

TEST_F(BSMorphologyTest, test_CreateBSReceptor_default) {
    BG::NES::Simulator::Geometries::Vec3D receptorLocation{0.1f, 0.2f, 0.3f};
    BG::NES::Simulator::Geometries::Box * receptor_ptr = &BG::NES::Simulator::BallAndStick::CreateBSReceptor(Collection, receptorLocation);

    ASSERT_EQ(receptor_ptr->Dims_um.x, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor_ptr->Dims_um.y, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor_ptr->Dims_um.z, _DEFAULT_RECEPTOR_DIM_um);
    ASSERT_EQ(receptor_ptr->Center_um.x, 0.1f);
    ASSERT_EQ(receptor_ptr->Center_um.y, 0.2f);
    ASSERT_EQ(receptor_ptr->Center_um.z, 0.3f);
}
