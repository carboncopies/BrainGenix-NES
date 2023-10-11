//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the BSNeuron struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#include <cmath>
#include <vector>
#include <memory>

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSNeuron.h>


/**
 * @brief Test class for unit tests for struct BSNeuron.
 *
 */

struct BSNeuronTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::BallAndStick::BSNeuron> testBSNeuron = nullptr;
    
    float axonRadius_um = 0.5;
    float somaRadius_um = 0.5;
    BG::NES::Simulator::Geometries::Vec3D axonEnd0_um{0.0, 0.0, 0.0};
    BG::NES::Simulator::Geometries::Vec3D axonEnd1_um{0.0, 10.0, 0.0};

    std::shared_ptr<BG::NES::Simulator::Geometries::Sphere> testSoma = \
         std::make_shared<BG::NES::Simulator::Geometries::Sphere>(somaRadius_um);
    std::shared_ptr<BG::NES::Simulator::Geometries::Cylinder> testAxon = \
         std::make_shared<BG::NES::Simulator::Geometries::Cylinder>(
                 axonRadius_um, axonEnd0_um, axonRadius_um, axonEnd1_um);
    float tol = 1e-3;

    void SetUp() {
        testBSNeuron = std::make_unique<BG::NES::Simulator::BallAndStick::BSNeuron>(
                200, testSoma, testAxon);

    }

    void TearDown() {
        return;
    }
};

TEST_F( BSNeuronTest, test_GetCellCenter_default ) {
    BG::NES::Simulator::Geometries::Vec3D cellCenter = testBSNeuron->GetCellCenter();
    BG::NES::Simulator::Geometries::Vec3D expectedCenter = testSoma->Center_um;
    ASSERT_EQ(cellCenter==expectedCenter, true);
}

TEST_F( BSNeuronTest, test_AttachDirectStim_default ) {
    testBSNeuron->AttachDirectStim(0.1f);

    // Time recorded successfully
    ASSERT_EQ(testBSNeuron->tDirectStim_ms[0], 0.1f);

    for (size_t i=0; i<5; ++i)
        testBSNeuron->AttachDirectStim(i * 0.15f);

    // Number of direct stimulus activities recorded.
    ASSERT_EQ(testBSNeuron->tDirectStim_ms.size(), 6);  
}

// TEST_F( BSNeuronTest, test_SetSpontaneousActivity_default ) {
//     float testMu = 0.1;
//     float testStd = 0.5;
// 
//     calc_dt_spont_dist = stats.truncnorm(
//         -test_mu / test_std, test_mu / test_std, loc=test_mu, scale=test_std
//     )
//     self.neuron.set_spontaneous_activity((test_mu, test_std))
//     assert self.neuron.tau_spont_mean_stdev_ms == (test_mu, test_std)
//     assert self.neuron.dt_spont_dist.stats() == calc_dt_spont_dist.stats()
// 
// }
