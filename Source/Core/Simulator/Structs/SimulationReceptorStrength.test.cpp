//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
     Description: Unit tests for per-receptor-type strength updates of LIFC
                  simulations (optional neurotransmitter argument of
                  Simulation::UpdatePrePostStrength, UpdateBatchPrePostStrength
                  and UpdateAllStrength).
     Additional Notes: Kept separate from Simulation.test.cpp, which is
                  currently not compiled by CMake (unit test target is
                  commented out in Source/Core/CMakeLists.txt).
*/

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>


// ---------------------------------------------------------------------------
// Per-receptor-type strength updates (optional NT argument of
// UpdatePrePostStrength / UpdateBatchPrePostStrength / UpdateAllStrength).
// Three LIFC neurons: 0 and 1 principal-like, 2 interneuron-like.
//   0 -> 1: AMPA (2 receptors x 0.98 nS, w 0.2), NMDA (0.55 nS, w 0.2), GABA (0.8 nS, w 0.2)
//   2 -> 1: GABA only (0.8 nS, w 0.2)
// ---------------------------------------------------------------------------
struct SimulationReceptorStrengthTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<BG::NES::Simulator::Simulation> Sim{};
    using NT = BG::NES::Simulator::Connections::NeurotransmitterType;

    void SetUp() {
        using namespace BG::NES::Simulator;
        Sim = std::make_unique<Simulation>(&Logger);
        for (int n = 0; n < 3; n++) {
            Geometries::Box& B = Sim->Collection.AddBox();
            Compartments::LIFC C;
            C.ShapeID = B.ID;
            C.Name = "c" + std::to_string(n);
            ASSERT_GE(Sim->AddLIFCCompartment(C), 0);
            CoreStructs::LIFCNeuronStruct N{};
            N.Name = "n" + std::to_string(n);
            N.SomaCompartmentIDs = {C.ID};
            N.RestingPotential_mV = -70; N.ResetPotential_mV = -70; N.SpikeThreshold_mV = -50;
            N.MembraneResistance_MOhm = 100; N.MembraneCapacitance_pF = 100;
            // Kinetic constants must be valid (rise != decay) for the neuron to be constructed.
            N.FastAfterHyperpolarizationRise_ms = 0.5; N.FastAfterHyperpolarizationDecay_ms = 5.0;
            N.SlowAfterHyperpolarizationRise_ms = 5.0; N.SlowAfterHyperpolarizationDecay_ms = 50.0;
            N.AfterDepolarizationRise_ms = 1.0; N.AfterDepolarizationDecay_ms = 10.0;
            N.FastAfterHyperpolarizationHalfActConstant = 1.0; N.SlowAfterHyperpolarizationHalfActConstant = 1.0;
            N.FatigueRecoveryTime_ms = 100.0; N.AfterDepolarizationRecoveryTime_ms = 100.0;
            N.AdaptiveTresholdRecoveryTime_ms = 100.0; N.AdaptiveThresholdFloorRecoveryTime_ms = 100.0;
            N.RefractoryPeriod_ms = 2.0;
            ASSERT_GE(Sim->AddLIFCNeuron(N), 0);
        }
        AddReceptor(0, 1, Connections::AMPA, 0.98, 0.2);
        AddReceptor(0, 1, Connections::AMPA, 0.98, 0.2);
        AddReceptor(0, 1, Connections::NMDA, 0.55, 0.2);
        AddReceptor(0, 1, Connections::GABA, 0.80, 0.2);
        AddReceptor(2, 1, Connections::GABA, 0.80, 0.2);
    }

    void AddReceptor(int pre, int post, NT type, float gpeak, float w) {
        using namespace BG::NES::Simulator;
        Connections::LIFCReceptor R;
        R.SourceCompartmentID = pre; R.DestinationCompartmentID = post;
        R.Neurotransmitter = type; R.PeakConductance_nS = gpeak; R.Weight = w;
        R.PSPRise_ms = 0.5; R.PSPDecay_ms = 3.0; R.voltage_gated = (type == Connections::NMDA);
        ASSERT_GE(Sim->AddLIFCReceptor(R), 0);
    }

    // Conductance (weight x g_peak_sum) of the group of this type, read through the
    // public connectome output of the presynaptic neuron (types: 1 AMPA, 2 GABA, 3 NMDA); -1 if none.
    float G(int pre, int post, NT type) {
        using namespace BG::NES::Simulator;
        const int code = (type == Connections::AMPA) ? 1 : ((type == Connections::GABA) ? 2 : 3);
        nlohmann::json tv(nlohmann::json::value_t::array), ty(nlohmann::json::value_t::array),
                       wv(nlohmann::json::value_t::array), gv(nlohmann::json::value_t::array), nv(nlohmann::json::value_t::array);
        static_cast<LIFCNeuron*>(Sim->Neurons.at(pre).get())->GetConnectomeTargetsJSON(tv, ty, wv, gv, nv);
        for (size_t i = 0; i < tv.size(); i++)
            if (tv[i].get<int>() == post && ty[i].get<int>() == code) return wv[i].get<float>() * gv[i].get<float>();
        return -1.0f;
    }
    float tol = 1e-5;
};

TEST_F(SimulationReceptorStrengthTest, baseline_groups) {
    ASSERT_NEAR(G(0, 1, BG::NES::Simulator::Connections::AMPA), 0.2f * 1.96f, tol);
    ASSERT_NEAR(G(0, 1, BG::NES::Simulator::Connections::NMDA), 0.2f * 0.55f, tol);
    ASSERT_NEAR(G(0, 1, BG::NES::Simulator::Connections::GABA), 0.2f * 0.80f, tol);
    ASSERT_NEAR(G(2, 1, BG::NES::Simulator::Connections::GABA), 0.2f * 0.80f, tol);
}

TEST_F(SimulationReceptorStrengthTest, set_GABA_only_leaves_other_types) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_TRUE(Sim->UpdatePrePostStrength(0, 1, 0.4f, GABA));
    ASSERT_NEAR(G(0, 1, GABA), 0.4f, tol);
    ASSERT_NEAR(G(0, 1, AMPA), 0.2f * 1.96f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.2f * 0.55f, tol);
    ASSERT_NEAR(G(2, 1, GABA), 0.2f * 0.80f, tol);
}

TEST_F(SimulationReceptorStrengthTest, set_NMDA_only_leaves_other_types) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_TRUE(Sim->UpdatePrePostStrength(0, 1, 0.3f, NMDA));
    ASSERT_NEAR(G(0, 1, NMDA), 0.3f, tol);
    ASSERT_NEAR(G(0, 1, AMPA), 0.2f * 1.96f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.2f * 0.80f, tol);
}

TEST_F(SimulationReceptorStrengthTest, set_AMPA_only_leaves_other_types) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_TRUE(Sim->UpdatePrePostStrength(0, 1, 1.5f, AMPA));
    ASSERT_NEAR(G(0, 1, AMPA), 1.5f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.2f * 0.55f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.2f * 0.80f, tol);
}

TEST_F(SimulationReceptorStrengthTest, legacy_call_unchanged) {
    using namespace BG::NES::Simulator::Connections;
    // No NT: first AMPA group set, NMDA and GABA of that pair zeroed (legacy behavior).
    ASSERT_TRUE(Sim->UpdatePrePostStrength(0, 1, 1.5f));
    ASSERT_NEAR(G(0, 1, AMPA), 1.5f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.0f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.0f, tol);
    ASSERT_NEAR(G(2, 1, GABA), 0.2f * 0.80f, tol); // other presynaptic cell untouched
    // Legacy on a GABA-only pair: returns false and zeroes the GABA (documented legacy behavior).
    ASSERT_FALSE(Sim->UpdatePrePostStrength(2, 1, 1.0f));
    ASSERT_NEAR(G(2, 1, GABA), 0.0f, tol);
}

TEST_F(SimulationReceptorStrengthTest, set_GABA_on_GABA_only_pair) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_TRUE(Sim->UpdatePrePostStrength(2, 1, 0.64f, GABA));
    ASSERT_NEAR(G(2, 1, GABA), 0.64f, tol);
}

TEST_F(SimulationReceptorStrengthTest, missing_type_fails_without_side_effects) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_FALSE(Sim->UpdatePrePostStrength(2, 1, 1.0f, AMPA)); // 2 -> 1 has no AMPA
    ASSERT_NEAR(G(2, 1, GABA), 0.2f * 0.80f, tol);              // legacy would have zeroed it
    ASSERT_FALSE(Sim->UpdatePrePostStrength(0, 1, -1.0f, GABA)); // negative rejected
    ASSERT_NEAR(G(0, 1, GABA), 0.2f * 0.80f, tol);
    ASSERT_FALSE(Sim->UpdatePrePostStrength(0, 1, 1.0f, 7));     // invalid type
    ASSERT_FALSE(Sim->UpdatePrePostStrength(0, 9, 1.0f, GABA));  // invalid post
}

TEST_F(SimulationReceptorStrengthTest, batch_single_and_per_pair_types) {
    using namespace BG::NES::Simulator::Connections;
    ASSERT_TRUE(Sim->UpdateBatchPrePostStrength({0, 2}, {1, 1}, {0.4f, 0.5f}, {GABA, GABA}));
    ASSERT_NEAR(G(0, 1, GABA), 0.4f, tol);
    ASSERT_NEAR(G(2, 1, GABA), 0.5f, tol);
    ASSERT_NEAR(G(0, 1, AMPA), 0.2f * 1.96f, tol);
    ASSERT_TRUE(Sim->UpdateBatchPrePostStrength({0, 0}, {1, 1}, {1.0f, 0.2f}, {AMPA, NMDA}));
    ASSERT_NEAR(G(0, 1, AMPA), 1.0f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.2f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.4f, tol);
    // length mismatch of the NT vector
    ASSERT_FALSE(Sim->UpdateBatchPrePostStrength({0, 0}, {1, 1}, {1.0f, 0.2f}, {AMPA}));
    // no NT vector = legacy
    ASSERT_TRUE(Sim->UpdateBatchPrePostStrength({0}, {1}, {0.7f}));
    ASSERT_NEAR(G(0, 1, AMPA), 0.7f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.0f, tol);
}

TEST_F(SimulationReceptorStrengthTest, set_all_per_type_and_legacy) {
    using namespace BG::NES::Simulator::Connections;
    Sim->UpdateAllStrength(0.3f, GABA);
    ASSERT_NEAR(G(0, 1, GABA), 0.3f, tol);
    ASSERT_NEAR(G(2, 1, GABA), 0.3f, tol);
    ASSERT_NEAR(G(0, 1, AMPA), 0.2f * 1.96f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.2f * 0.55f, tol);
    Sim->UpdateAllStrength(0.0f); // legacy: all AMPA set, every NMDA and GABA zeroed
    ASSERT_NEAR(G(0, 1, AMPA), 0.0f, tol);
    ASSERT_NEAR(G(0, 1, NMDA), 0.0f, tol);
    ASSERT_NEAR(G(0, 1, GABA), 0.0f, tol);
    ASSERT_NEAR(G(2, 1, GABA), 0.0f, tol);
}
