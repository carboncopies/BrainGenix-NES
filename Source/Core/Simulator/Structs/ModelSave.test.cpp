//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
     Description: Unit tests for LIFC receptor weight persistence in model
                  Save/Load.
     Additional Notes: None
     Date Created: 2026-11-04
*/

#include <filesystem>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>
#include <Util/StoragePaths.h>

/**
 * LIFC receptor strengths live in LIFCReceptorData::weight at runtime, while
 * the model file stores the underlying LIFCReceptorBase structs. This fixture
 * exercises a save/load round-trip after SetPrePostStrength.
 */
struct ModelSaveTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<BG::NES::Simulator::Simulation> testSimulation{};
    std::string TestRoot = "/tmp/nes-modelsave-unit-test";
    std::string PreviousOutputBase{};

    void SetUp() override {
        PreviousOutputBase = BG::NES::Util::Storage::GetOutputBasePath();
        BG::NES::Util::Storage::SetOutputBasePath(TestRoot);
        testSimulation = std::make_unique<BG::NES::Simulator::Simulation>(&Logger);
        testSimulation->OwnerUsername = "modelsave-test";
    }

    void TearDown() override {
        std::error_code Error;
        std::filesystem::remove_all(TestRoot, Error);
        BG::NES::Util::Storage::SetOutputBasePath(PreviousOutputBase);
    }

    //! Create a minimal LIFC neuron and return its soma compartment ID.
    int AddLIFCCell(BG::NES::Simulator::Simulation & Sim, size_t Index) {
        using namespace BG::NES::Simulator;
        auto & S = Sim.Collection.AddSphere(Geometries::Vec3D(Index * 20.0f, 0.0f, 0.0f), 5.0f);

        Compartments::LIFC C;
        C.ShapeID = S.ID;
        C.RestingPotential_mV = -70.0f;
        C.ResetPotential_mV = -55.0f;
        C.SpikeThreshold_mV = -50.0f;
        C.MembraneResistance_MOhm = 100.0f;
        C.MembraneCapacitance_pF = 100.0f;
        C.AfterHyperpolarizationAmplitude_mV = -90.0f;
        EXPECT_GE(Sim.AddLIFCCompartment(C), 0);

        CoreStructs::LIFCNeuronStruct N{};
        N.Name = "test-lifc-" + std::to_string(Index);
        N.RestingPotential_mV = -70.0f;
        N.ResetPotential_mV = -55.0f;
        N.SpikeThreshold_mV = -50.0f;
        N.MembraneResistance_MOhm = 100.0f;
        N.MembraneCapacitance_pF = 100.0f;
        N.RefractoryPeriod_ms = 2.0f;
        N.SpikeDepolarization_mV = 30.0f;
        N.UpdateMethod = CoreStructs::EXPEULER_CM;
        N.ResetMethod = CoreStructs::TOVM;
        N.FastAfterHyperpolarizationRise_ms = 2.5f;
        N.FastAfterHyperpolarizationDecay_ms = 30.0f;
        N.SlowAfterHyperpolarizationRise_ms = 30.0f;
        N.SlowAfterHyperpolarizationDecay_ms = 300.0f;
        N.AfterDepolarizationRise_ms = 20.0f;
        N.AfterDepolarizationDecay_ms = 200.0f;
        N.SomaCompartmentIDs.push_back(C.ID);
        EXPECT_GE(Sim.AddLIFCNeuron(N), 0);
        return C.ID;
    }

    void AddLIFCReceptor(BG::NES::Simulator::Simulation & Sim, int SrcComp, int DstComp,
                         BG::NES::Simulator::Connections::NeurotransmitterType Type,
                         float PeakConductance_nS, float Weight) {
        using namespace BG::NES::Simulator;
        Connections::LIFCReceptor R;
        R.SourceCompartmentID = SrcComp;
        R.DestinationCompartmentID = DstComp;
        R.Neurotransmitter = Type;
        R.PeakConductance_nS = PeakConductance_nS;
        R.Weight = Weight;
        R.PSPRise_ms = 1.0f;
        R.PSPDecay_ms = 5.0f;
        EXPECT_GE(Sim.AddLIFCReceptor(R), 0);
    }

    //! Runtime abstracted weight of the receptor group of the given type, from
    //! SrcNeuron to DstNeuron.
    static float FindWeight(BG::NES::Simulator::Simulation & Sim, int SrcNeuron, int DstNeuron,
                            BG::NES::Simulator::Connections::NeurotransmitterType Type) {
        for (auto & RData : Sim.LIFCReceptorDataVec) {
            if (RData->SrcNeuronID == SrcNeuron && RData->DstNeuronID == DstNeuron && RData->Type() == Type) return RData->weight;
        }
        return -1.0f;
    }

    //! Save, then load into a fresh simulation, as ModelSave/ModelLoad do over the RPC.
    std::unique_ptr<BG::NES::Simulator::Simulation> RoundTrip(const std::string & Name) {
        EXPECT_TRUE(testSimulation->SaveModel(Name));
        auto Loaded = std::make_unique<BG::NES::Simulator::Simulation>(&Logger);
        Loaded->OwnerUsername = testSimulation->OwnerUsername;
        EXPECT_TRUE(Loaded->LoadModel(Name));
        return Loaded;
    }
};

TEST_F(ModelSaveTest, test_ModelSaveLoad_PersistsEditedReceptorWeights) {
    using namespace BG::NES::Simulator;

    int Comp0 = AddLIFCCell(*testSimulation, 0);
    int Comp1 = AddLIFCCell(*testSimulation, 1);
    int Comp2 = AddLIFCCell(*testSimulation, 2);

    // Neuron 0 -> 1: an AMPA/NMDA pair (the setter edits the AMPA and zeroes the NMDA).
    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::AMPA, 2.0f, 0.25f);
    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::NMDA, 1.0f, 0.5f);
    // Neuron 2 -> 1: never edited.
    AddLIFCReceptor(*testSimulation, Comp2, Comp1, Connections::AMPA, 4.0f, 0.7f);
    ASSERT_EQ(testSimulation->LIFCReceptorDataVec.size(), 3u);

    // New conductance is divided by the abstracted peak conductance to give the weight.
    ASSERT_TRUE(testSimulation->UpdatePrePostStrength(0, 1, 1.0f));
    ASSERT_NEAR(FindWeight(*testSimulation, 0, 1, Connections::AMPA), 0.5f, 1e-6f);
    ASSERT_NEAR(FindWeight(*testSimulation, 0, 1, Connections::NMDA), 0.0f, 1e-6f);

    auto Loaded = RoundTrip("modelsave-roundtrip");
    ASSERT_EQ(Loaded->LIFCReceptorDataVec.size(), 3u);

    // Edited receptors: what the running simulation used is what was saved.
    EXPECT_NEAR(FindWeight(*Loaded, 0, 1, Connections::AMPA), 0.5f, 1e-6f);
    EXPECT_NEAR(FindWeight(*Loaded, 0, 1, Connections::NMDA), 0.0f, 1e-6f);
    // Unedited receptor keeps its weight.
    EXPECT_NEAR(FindWeight(*Loaded, 2, 1, Connections::AMPA), 0.7f, 1e-6f);

    // The saved base structs agree too (receptor order is preserved).
    ASSERT_EQ(Loaded->LIFCReceptors.size(), 3u);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(0)->Weight, 0.5f, 1e-6f);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(1)->Weight, 0.0f, 1e-6f);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(2)->Weight, 0.7f, 1e-6f);

    // Saving must not alter the in-memory model's base structs.
    EXPECT_NEAR(testSimulation->LIFCReceptors.at(0)->Weight, 0.25f, 1e-6f);
}

TEST_F(ModelSaveTest, test_ModelSaveLoad_UneditedAbstractedGroupKeepsIndividualWeights) {
    using namespace BG::NES::Simulator;

    int Comp0 = AddLIFCCell(*testSimulation, 0);
    int Comp1 = AddLIFCCell(*testSimulation, 1);

    // Two AMPA receptors between the same pair are abstracted into one group
    // whose runtime weight is their g_peak-weighted mean. Without any edit the
    // individual base weights must survive unchanged.
    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::AMPA, 1.0f, 0.2f);
    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::AMPA, 3.0f, 0.6f);
    ASSERT_EQ(testSimulation->LIFCReceptorDataVec.size(), 1u);

    auto Loaded = RoundTrip("modelsave-unedited-group");
    ASSERT_EQ(Loaded->LIFCReceptors.size(), 2u);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(0)->Weight, 0.2f, 1e-6f);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(1)->Weight, 0.6f, 1e-6f);
}

TEST_F(ModelSaveTest, test_ModelSaveLoad_EditedAbstractedGroupSavedForAllMembers) {
    using namespace BG::NES::Simulator;

    int Comp0 = AddLIFCCell(*testSimulation, 0);
    int Comp1 = AddLIFCCell(*testSimulation, 1);

    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::AMPA, 1.0f, 0.2f);
    AddLIFCReceptor(*testSimulation, Comp0, Comp1, Connections::AMPA, 3.0f, 0.6f);
    ASSERT_EQ(testSimulation->LIFCReceptorDataVec.size(), 1u);

    // Group g_peak sum is 4 nS, so 1 nS gives a runtime weight of 0.25 (the unedited aggregate is 0.5).
    ASSERT_TRUE(testSimulation->UpdatePrePostStrength(0, 1, 1.0f));
    ASSERT_NEAR(FindWeight(*testSimulation, 0, 1, Connections::AMPA), 0.25f, 1e-6f);

    auto Loaded = RoundTrip("modelsave-edited-group");
    EXPECT_NEAR(FindWeight(*Loaded, 0, 1, Connections::AMPA), 0.25f, 1e-6f);
    ASSERT_EQ(Loaded->LIFCReceptors.size(), 2u);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(0)->Weight, 0.25f, 1e-6f);
    EXPECT_NEAR(Loaded->LIFCReceptors.at(1)->Weight, 0.25f, 1e-6f);
}
