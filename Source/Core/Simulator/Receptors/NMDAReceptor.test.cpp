
//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the NMDAReceptor struct.
    Additional Notes: None
    Date Created: 2023-10-16
*/

#include <cmath>

#include <gtest/gtest.h>

#include <Simulator/Receptors/NMDAReceptor.h>

/**
 * @brief Test class for unit tests for struct NMDAReceptor.
 *
 */

struct NMDAReceptorTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Receptors::NMDAReceptor> testReceptor =
        nullptr;

    float tol = 1e-3;

    void SetUp() {
        testReceptor =
            std::make_unique<BG::NES::Simulator::Receptors::NMDAReceptor>();
    }

    void TearDown() { return; }
};

// float PhiV(PhiVType phiVType, float Mg2plus_0, float delta, float voltage_V,
//            float temperature_K);

TEST_F(NMDAReceptorTest, test_PhiVBolzmann_default) {
    float expected =
        1.0 /
        (1.0 + exp(-(testReceptor->Vm_mV - testReceptor->VHalfBlocked_mV) /
                   testReceptor->K));
    ASSERT_NEAR(testReceptor->PhiVBolzmann(), expected, tol);

    // Check for zero division
    testReceptor->K = 0.0;
    try {
        float phiV = testReceptor->PhiVBolzmann();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.(K)", ex.what());
    }
}

TEST_F(NMDAReceptorTest, test_Phi_default) {
    float z = 2.0;
    float T_K = 343.0;
    float expected = z * _F / (_R * T_K);
    ASSERT_NEAR(testReceptor->Phi(T_K, z), expected, tol);

    // Check for zero division
    T_K = 0.0;
    try {
        expected = testReceptor->Phi(T_K, z);
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.(temperature_K)", ex.what());
    }
}

TEST_F(NMDAReceptorTest, test_KBindingRate_default) {
    float Mg2plus_0 = 0.1;
    float KBinding = 0.5;
    float delta = 0.1;
    float voltage_V = 0.5;
    float temperature_K = 343.0;
    float expected = 0.0;

    testReceptor->KBinding = KBinding;

    expected = Mg2plus_0 * KBinding *
               exp(-delta * testReceptor->Phi(temperature_K) * voltage_V / 2.0);

    ASSERT_NEAR(
        testReceptor->KBindingRate(Mg2plus_0, delta, voltage_V, temperature_K),
        expected, tol);
}

TEST_F(NMDAReceptorTest, test_KUnbindingRate_default) {
    float KUnbinding = 0.5;
    float delta = 0.1;
    float voltage_V = 0.5;
    float temperature_K = 343.0;
    float expected = 0.0;

    testReceptor->KUnbinding = KUnbinding;
    expected = KUnbinding *
               exp(delta * testReceptor->Phi(temperature_K) * voltage_V / 2.0);
    ASSERT_NEAR(testReceptor->KUnbindingRate(delta, voltage_V, temperature_K),
                expected, tol);
}

TEST_F(NMDAReceptorTest, test_PhiVWoodHull1_default) {
    float Mg2plus_0 = 0.1;
    float KBinding = 0.5;
    float KUnbinding = 0.4;
    float delta = 0.1;
    float voltage_V = 1.0;
    float temperature_K = 343.0;

    float KBindingRate =
        Mg2plus_0 * KBinding *
        exp(-delta * testReceptor->Phi(temperature_K) * voltage_V / 2.0);
    float KUnbindingRate =
        KUnbinding *
        exp(delta * testReceptor->Phi(temperature_K) * voltage_V / 2.0);
    float expected = 1.0 / (1.0 + KBindingRate / KUnbindingRate);

    testReceptor->KBinding = KBinding;
    testReceptor->KUnbinding = KUnbinding;

    ASSERT_NEAR(
        testReceptor->PhiVWoodhull1(Mg2plus_0, delta, voltage_V, temperature_K),
        expected, tol);

    // Test for zero division
    testReceptor->KUnbinding = 0.0;

    try {
        expected = testReceptor->PhiVWoodhull1(Mg2plus_0, delta, voltage_V,
                                               temperature_K);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.(KUnbinding)", ex.what());
    }
}

TEST_F(NMDAReceptorTest, test_PhiVWoodHull2_default) {
    float Mg2plus_0 = 0.1;
    float KDissociation_0mV = 0.5;
    float delta = 0.1;
    float voltage_V = 1.0;
    float temperature_K = 343.0;

    float KD = KDissociation_0mV *
               exp(delta * testReceptor->Phi(temperature_K) * voltage_V);
    float expected = 1.0 / (1.0 + Mg2plus_0 / KD);

    testReceptor->KDissociation_0mV = KDissociation_0mV;

    ASSERT_NEAR(
        testReceptor->PhiVWoodhull2(Mg2plus_0, delta, voltage_V, temperature_K),
        expected, tol);

    // Test for zero division
    testReceptor->KDissociation_0mV = 0.0;

    try {
        expected = testReceptor->PhiVWoodhull2(Mg2plus_0, delta, voltage_V,
                                               temperature_K);
        FAIL();
    } catch (std::overflow_error &ex) {
        ASSERT_STREQ("Cannot divide by zero.(KDissociation_0mV)", ex.what());
    }
}

TEST_F(NMDAReceptorTest, test_PostsynCurrentStrongVoltageDep_pA_default) {
    float KBinding = 0.5;
    float KUnbinding = 0.4;
    float Mg2plus_0 = 0.1;
    float delta = 0.1;
    float voltage_V = 1.0;
    float temperature_K = 343.0;

    testReceptor->KBinding = KBinding;
    testReceptor->KUnbinding = KUnbinding;

    float expected =
        testReceptor->PhiV(
            BG::NES::Simulator::Receptors::NMDAReceptor::WOODHULL_1, Mg2plus_0,
            delta, voltage_V, temperature_K) *
        testReceptor->PostSynCurrent_pA();
    ASSERT_NEAR(testReceptor->PostSynCurrentStrongVoltageDep_pA(
                    BG::NES::Simulator::Receptors::NMDAReceptor::WOODHULL_1,
                    Mg2plus_0, delta, voltage_V, temperature_K),
                expected, tol);
}
