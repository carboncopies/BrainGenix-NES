//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Tests of the Simulation run and query API: recording window, RunFor, and the
                 JSON getters (spike times, recording, soma positions, connectome, abstract
                 connectome), connection-strength updates and geometric centre, all on tiny
                 networks (dev_frame NT-5). Tests only, no production code changes.
                 Suites ending in KnownBug assert the CORRECT behaviour of confirmed bugs
                 and carry the ctest label known_bug (dev_frame D8).
*/

#include <cmath>
#include <limits>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/SimulationTestHelpers.h>
#include <TestUtil/TestUtil.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::Simulator::SimTest;
using nlohmann::json;

namespace {

struct SimulationRun : BG::NES::TestUtil::TempOutputRoot {
    std::unique_ptr<Simulation> Sim;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Sim = std::make_unique<Simulation>(Logger_.get());
    }
    void TearDown() override {
        Sim.reset();
        TempOutputRoot::TearDown();
    }
};

struct SimulationRunKnownBug : SimulationRun {};

std::vector<float> Floats(const json& J) { return J.get<std::vector<float>>(); }

// Two identical BS networks must behave identically; this builds the standard one:
// 3 neurons, neuron 0 stimulated at 3 ms.
void BuildStimulatedBSNet(Simulation& Sim) {
    BuildBSNet(Sim, 3);
    Sim.Neurons[0]->AddSpecificAPTime(3.0f);
}

} // namespace

// ---------------------------------------------------------------- recording window

TEST_F(SimulationRun, NothingIsRecordedUntilRecordingIsRequested) {
    EXPECT_FALSE(Sim->IsRecording());
    EXPECT_FALSE(Sim->InstrumentsAreRecording());
    Sim->RunFor(10);
    EXPECT_TRUE(Sim->TRecorded_ms.empty());
    EXPECT_TRUE(Sim->TInstruments_ms.empty());
}

TEST_F(SimulationRun, RecordingWindowCoversBeforeInsideAndAfterTheWindow) {
    Sim->SetRecordAll(10.0f);
    EXPECT_TRUE(Sim->IsRecording()) << "before the window ends (T=0)";
    Sim->RunFor(5);
    EXPECT_TRUE(Sim->IsRecording()) << "inside the window (T=5)";
    Sim->RunFor(5);
    EXPECT_FALSE(Sim->IsRecording()) << "window [0,10) is closed at T=10";
    Sim->RunFor(10);
    EXPECT_FALSE(Sim->IsRecording()) << "after the window";

    ASSERT_EQ(Sim->TRecorded_ms.size(), 10u);
    for (size_t i = 0; i < 10; i++) EXPECT_NEAR(Sim->TRecorded_ms[i], float(i), 1e-4);
}

TEST_F(SimulationRun, RecordForeverNeverStops) {
    Sim->SetRecordAll(); // default argument is _RECORD_FOREVER_TMAX_MS
    Sim->RunFor(100);
    EXPECT_TRUE(Sim->IsRecording());
    EXPECT_EQ(Sim->TRecorded_ms.size(), 100u);
}

TEST_F(SimulationRun, RecordingCanBeSwitchedOffMidRun) {
    Sim->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    Sim->RunFor(5);
    Sim->SetRecordAll(0.0f);
    EXPECT_FALSE(Sim->IsRecording());
    Sim->RunFor(5);
    EXPECT_EQ(Sim->TRecorded_ms.size(), 5u);
}

TEST_F(SimulationRun, SettingTheWindowWhileRecordingRestartsItAtTheCurrentTime) {
    Sim->SetRecordAll(10.0f);
    Sim->RunFor(5);
    Sim->SetRecordAll(10.0f); // window is now [5, 15)
    EXPECT_NEAR(Sim->StartRecordTime_ms, 5.0f, 1e-4);
    Sim->RunFor(20);
    EXPECT_EQ(Sim->TRecorded_ms.size(), 15u);
}

// Bug: dev_frame #113
TEST_F(SimulationRunKnownBug, RecordingCanBeEnabledAfterTheSimulationAlreadyRan) {
    Sim->RunFor(20);
    Sim->SetRecordAll(10.0f);
    EXPECT_TRUE(Sim->IsRecording());
    Sim->RunFor(10);
    EXPECT_EQ(Sim->TRecorded_ms.size(), 10u);
}

TEST_F(SimulationRun, InstrumentRecordingWindowAtTimeZero) {
    Sim->SetRecordInstruments(10.0f);
    EXPECT_TRUE(Sim->InstrumentsAreRecording());
    Sim->RunFor(20);
    EXPECT_FALSE(Sim->InstrumentsAreRecording());
    EXPECT_EQ(Sim->TInstruments_ms.size(), 10u);
    EXPECT_TRUE(Sim->TRecorded_ms.empty()) << "instrument recording is independent of SetRecordAll";
}

TEST_F(SimulationRun, InstrumentRecordingForeverAndOff) {
    Sim->SetRecordInstruments();
    Sim->RunFor(7);
    EXPECT_EQ(Sim->TInstruments_ms.size(), 7u);
    Sim->SetRecordInstruments(0.0f);
    EXPECT_FALSE(Sim->InstrumentsAreRecording());
    Sim->RunFor(7);
    EXPECT_EQ(Sim->TInstruments_ms.size(), 7u);
}

// Bug: dev_frame #113
TEST_F(SimulationRunKnownBug, InstrumentRecordingCanBeEnabledAfterTheSimulationAlreadyRan) {
    Sim->RunFor(20);
    Sim->SetRecordInstruments(10.0f);
    EXPECT_TRUE(Sim->InstrumentsAreRecording());
    Sim->RunFor(10);
    EXPECT_EQ(Sim->TInstruments_ms.size(), 10u);
}

// ---------------------------------------------------------------- RunFor

TEST_F(SimulationRun, RunForNonPositiveOrNanIsANoOp) {
    BuildStimulatedBSNet(*Sim);
    Sim->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    Sim->SetRecordInstruments(_RECORD_FOREVER_TMAX_MS);
    for (float t : {0.0f, -0.5f, -100.0f, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()}) {
        Sim->RunFor(t);
        EXPECT_EQ(Sim->T_ms, 0.0f) << t;
        EXPECT_TRUE(Sim->TRecorded_ms.empty()) << t;
        EXPECT_TRUE(Sim->TInstruments_ms.empty()) << t;
        EXPECT_EQ(Sim->TotalSpikes(), 0ul) << t;
    }
}

TEST_F(SimulationRun, RunForAdvancesTimeByTheRequestedDuration) {
    for (float Dt : {1.0f, 0.5f, 0.25f}) {
        Simulation S(Logger_.get());
        S.Dt_ms = Dt;
        S.RunFor(10.0f);
        EXPECT_NEAR(S.T_ms, 10.0f, 1e-3) << "Dt=" << Dt;
    }
}

TEST_F(SimulationRun, RunForOfANonMultipleOfDtOvershootsByLessThanOneStep) {
    Sim->Dt_ms = 1.0f;
    Sim->RunFor(2.5f);
    EXPECT_GE(Sim->T_ms, 2.5f);
    EXPECT_LT(Sim->T_ms, 2.5f + Sim->Dt_ms);
}

TEST_F(SimulationRun, TwoRunsOfFiveEqualOneRunOfTen) {
    for (float Dt : {1.0f, 0.5f}) {
        Simulation A(Logger_.get()), B(Logger_.get());
        A.Dt_ms = B.Dt_ms = Dt;
        BuildStimulatedBSNet(A);
        BuildStimulatedBSNet(B);
        A.SetRecordAll(_RECORD_FOREVER_TMAX_MS);
        B.SetRecordAll(_RECORD_FOREVER_TMAX_MS);

        A.RunFor(5);
        A.RunFor(5);
        B.RunFor(10);

        EXPECT_NEAR(A.T_ms, B.T_ms, 1e-4) << "Dt=" << Dt;
        EXPECT_EQ(A.TRecorded_ms.size(), B.TRecorded_ms.size()) << "Dt=" << Dt;
        EXPECT_EQ(A.GetSpikeTimesJSON(), B.GetSpikeTimesJSON()) << "Dt=" << Dt;
        EXPECT_EQ(A.GetRecordingJSON(), B.GetRecordingJSON()) << "Dt=" << Dt;
        EXPECT_EQ(A.TotalSpikes(), B.TotalSpikes());
    }
}

TEST_F(SimulationRun, RecordedSampleCountIsOnePerStepForSimulationAndForEveryNeuron) {
    BuildStimulatedBSNet(*Sim);
    Sim->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    Sim->RunFor(10);
    EXPECT_EQ(Sim->TRecorded_ms.size(), 10u);
    for (auto& N : Sim->Neurons) {
        EXPECT_EQ(static_cast<BallAndStick::BSNeuron*>(N.get())->VmRecorded_mV.size(), 10u);
    }
}

TEST_F(SimulationRun, RecordingFlagIsPassedToNeuronsOnlyInsideTheWindow) {
    BuildStimulatedBSNet(*Sim);
    Sim->SetRecordAll(4.0f);
    Sim->RunFor(10);
    EXPECT_EQ(Sim->TRecorded_ms.size(), 4u);
    EXPECT_EQ(static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[1].get())->VmRecorded_mV.size(), 4u);
}

TEST_F(SimulationRun, DirectStimulationMakesTheStimulatedNeuronSpikeExactlyAtTheStimulusTime) {
    BuildBSNet(*Sim, 3);
    Sim->Neurons[0]->AddSpecificAPTime(7.0f); // out of order on purpose: run-time sorts them
    Sim->Neurons[0]->AddSpecificAPTime(3.0f);
    Sim->RunFor(12);
    EXPECT_EQ(Sim->Neurons[0]->TAct_ms, (std::vector<float>{3.0f, 7.0f}));
    EXPECT_TRUE(Sim->Neurons[1]->TAct_ms.empty());
    EXPECT_TRUE(Sim->Neurons[2]->TAct_ms.empty());
    EXPECT_EQ(Sim->TotalSpikes(), 2ul);
}

TEST_F(SimulationRun, RunForOnAnEmptySimulationJustAdvancesTime) {
    Sim->RunFor(5);
    EXPECT_NEAR(Sim->T_ms, 5.0f, 1e-4);
    EXPECT_EQ(Sim->TotalSpikes(), 0ul);
}

TEST_F(SimulationRun, SCSimulationRunsAndSpikesOnStimulus) {
    for (int i = 0; i < 2; i++) AddSphereAt(*Sim, 10.0f * i, 0, 0);
    for (int i = 0; i < 2; i++) {
        Compartments::BS C = MakeBSCompartment(i);
        ASSERT_EQ(Sim->AddSCCompartment(C, SCNEURONS), i);
    }
    CoreStructs::SCNeuronStruct N0 = MakeSCNeuronStruct({0}), N1 = MakeSCNeuronStruct({1});
    ASSERT_EQ(Sim->AddSCNeuron(N0), 0);
    ASSERT_EQ(Sim->AddSCNeuron(N1), 1);
    Sim->Neurons[1]->AddSpecificAPTime(2.0f);
    Sim->RunFor(6);
    EXPECT_TRUE(Sim->Neurons[0]->TAct_ms.empty());
    EXPECT_EQ(Sim->Neurons[1]->TAct_ms, (std::vector<float>{2.0f}));
}

TEST_F(SimulationRun, LIFCSimulationRunsAndSpikesOnStimulus) {
    BuildLIFCNet(*Sim, 2);
    Sim->SetRandomSeed(42);
    Sim->Neurons[0]->AddSpecificAPTime(3.0f);
    Sim->SetRecordAll(_RECORD_FOREVER_TMAX_MS);
    Sim->RunFor(10);
    EXPECT_EQ(Sim->Neurons[0]->TAct_ms, (std::vector<float>{3.0f}));
    EXPECT_TRUE(Sim->Neurons[1]->TAct_ms.empty());
    EXPECT_EQ(Sim->TRecorded_ms.size(), 10u);
}

TEST_F(SimulationRun, SetRandomSeedStoresTheSeedAndCreatesTheGenerator) {
    EXPECT_EQ(Sim->MasterRandom_, nullptr);
    Sim->SetRandomSeed(42);
    EXPECT_EQ(Sim->RandomSeed, 42);
    ASSERT_NE(Sim->MasterRandom_, nullptr);
    Sim->SetRandomSeed(7);
    EXPECT_EQ(Sim->RandomSeed, 7);
    EXPECT_NE(Sim->MasterRandom_, nullptr);
}

TEST_F(SimulationRun, ShowDoesNotThrowForEmptyAndPopulatedSimulations) {
    EXPECT_NO_THROW(Sim->Show());
    BuildBSNet(*Sim, 2);
    EXPECT_NO_THROW(Sim->Show());
}

// ---------------------------------------------------------------- JSON getters

TEST_F(SimulationRun, SpikeTimesJsonHasOneEntryPerNeuronWithItsSpikeTimes) {
    BuildBSNet(*Sim, 3);
    Sim->Neurons[1]->AddSpecificAPTime(2.0f);
    Sim->Neurons[1]->AddSpecificAPTime(8.0f);
    Sim->RunFor(10);
    json J = Sim->GetSpikeTimesJSON();
    ASSERT_TRUE(J.is_object());
    EXPECT_EQ(J.size(), 3u);
    EXPECT_EQ(Floats(J.at("0").at("tSpike_ms")), std::vector<float>{});
    EXPECT_EQ(Floats(J.at("1").at("tSpike_ms")), (std::vector<float>{2.0f, 8.0f}));
    EXPECT_EQ(Floats(J.at("2").at("tSpike_ms")), std::vector<float>{});
}

TEST_F(SimulationRun, RecordingJsonHasTimeAxisAndPerNeuronMembranePotential) {
    BuildStimulatedBSNet(*Sim);
    Sim->SetRecordAll(6.0f);
    Sim->RunFor(10);
    json J = Sim->GetRecordingJSON();
    ASSERT_TRUE(J.contains("t_ms"));
    ASSERT_TRUE(J.contains("neurons"));
    EXPECT_EQ(Floats(J["t_ms"]), (std::vector<float>{0, 1, 2, 3, 4, 5}));
    EXPECT_EQ(J["neurons"].size(), 3u);
    for (const char* Key : {"0", "1", "2"}) {
        ASSERT_TRUE(J["neurons"].contains(Key));
        EXPECT_EQ(J["neurons"][Key]["Vm_mV"].size(), 6u) << Key;
    }
    auto Vm0 = Floats(J["neurons"]["0"]["Vm_mV"]);
    auto Vm1 = Floats(J["neurons"]["1"]["Vm_mV"]);
    EXPECT_NEAR(Vm0[0], -60.0f, 1e-4);
    EXPECT_GT(Vm0[3], -50.0f) << "stimulated neuron is depolarised at the spike";
    for (float v : Vm1) EXPECT_NEAR(v, -60.0f, 1e-4) << "unstimulated neuron stays at rest";
}

TEST_F(SimulationRun, RecordingJsonIsEmptyBeforeAnythingWasRecorded) {
    BuildBSNet(*Sim, 2);
    Sim->RunFor(5);
    json J = Sim->GetRecordingJSON();
    EXPECT_TRUE(J["t_ms"].empty());
    EXPECT_TRUE(J["neurons"]["0"]["Vm_mV"].empty());
}

TEST_F(SimulationRun, SomaPositionsJsonListsCentreTypeAndRadiusPerNeuron) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "GABA");
    ConnectBS(*Sim, 1, 2, "AMPA");
    json J = Sim->GetSomaPositionsJSON();
    ASSERT_EQ(J["SomaCenters"].size(), 3u);
    for (int i = 0; i < 3; i++) {
        auto C = Floats(J["SomaCenters"][i]);
        ASSERT_EQ(C.size(), 3u);
        EXPECT_NEAR(C[0], 100.0f * i, 1e-4);
        EXPECT_NEAR(C[1], 0.0f, 1e-4);
        EXPECT_NEAR(C[2], 0.0f, 1e-4);
        EXPECT_NEAR(J["SomaRadius"][i].get<float>(), 5.0f, 1e-5);
    }
    EXPECT_EQ(J["SomaTypes"], (json{2, 1, 0})) << "source of GABA = interneuron, source of AMPA = principal, no outputs = unknown";
}

TEST_F(SimulationRun, SomaPositionsJsonOfAnEmptySimulationHasEmptyLists) {
    json J = Sim->GetSomaPositionsJSON();
    EXPECT_TRUE(J["SomaCenters"].empty());
    EXPECT_TRUE(J["SomaTypes"].empty());
    EXPECT_TRUE(J["SomaRadius"].empty());
}

TEST_F(SimulationRun, BSConnectomeJsonListsTargetsTypesAndWeightsPerSourceNeuron) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "AMPA", 2.0f);
    ConnectBS(*Sim, 0, 2, "GABA", 3.0f);
    ConnectBS(*Sim, 1, 2, "NMDA", 4.0f);
    json J = Sim->GetConnectomeJSON();
    EXPECT_EQ(J["ConnectionTargets"], (json{json{1, 2}, json{2}, json::array()}));
    EXPECT_EQ(J["ConnectionTypes"], (json{json{1, 2}, json{3}, json::array()}));
    EXPECT_EQ(J["ConnectionWeights"], (json{json{2.0, 3.0}, json{4.0}, json::array()}));
    EXPECT_FALSE(J.contains("ConnectionGPeakSum"));
}

TEST_F(SimulationRun, LIFCConnectomeJsonMergesAbstractedReceptorsAndAddsGPeakAndCount) {
    BuildLIFCNet(*Sim, 2);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.5f);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.25f);
    json J = Sim->GetConnectomeJSON();
    EXPECT_EQ(J["ConnectionTargets"], (json{json{1}, json::array()}));
    EXPECT_EQ(J["ConnectionTypes"], (json{json{1}, json::array()}));
    EXPECT_EQ(J["NumReceptors"], (json{json{2}, json::array()}));
    ASSERT_EQ(J["ConnectionGPeakSum"][0].size(), 1u);
    EXPECT_NEAR(J["ConnectionGPeakSum"][0][0].get<float>(), 0.75f, 1e-6);
    EXPECT_NEAR(J["ConnectionWeights"][0][0].get<float>(), 1.0f, 1e-6);
}

TEST_F(SimulationRun, ConnectomeJsonWithoutReceptorsHasOneEmptyListPerNeuron) {
    BuildBSNet(*Sim, 2);
    json J = Sim->GetConnectomeJSON();
    EXPECT_EQ(J["ConnectionTargets"], (json{json::array(), json::array()}));
}

// ---------------------------------------------------------------- abstract connectome

namespace {
// 0->1 twice, 1->2 once, 2->0 three times: asymmetric so a transposed index shows up.
void BuildAsymmetricBSNet(Simulation& Sim) {
    BuildBSNet(Sim, 3);
    ConnectBS(Sim, 0, 1);
    ConnectBS(Sim, 0, 1);
    ConnectBS(Sim, 1, 2);
    ConnectBS(Sim, 2, 0);
    ConnectBS(Sim, 2, 0);
    ConnectBS(Sim, 2, 0);
}

// Convert sparse [pre,post,count] triples to the dense [pre][post] matrix.
std::vector<std::vector<size_t>> SparseToDense(const json& Sparse, size_t N) {
    std::vector<std::vector<size_t>> M(N, std::vector<size_t>(N, 0));
    for (const auto& Triple : Sparse) M.at(Triple[0].get<size_t>()).at(Triple[1].get<size_t>()) = Triple[2].get<size_t>();
    return M;
}
} // namespace

TEST_F(SimulationRun, AbstractConnectionCountsReceptorsPerPresynapticPostsynapticPair) {
    BuildAsymmetricBSNet(*Sim);
    EXPECT_EQ(Sim->GetAbstractConnection(0, 1, false), 2u);
    EXPECT_EQ(Sim->GetAbstractConnection(1, 2, false), 1u);
    EXPECT_EQ(Sim->GetAbstractConnection(2, 0, false), 3u);
    EXPECT_EQ(Sim->GetAbstractConnection(1, 0, false), 0u) << "direction matters";
    EXPECT_EQ(Sim->GetAbstractConnection(0, 99, false), 0u);
    EXPECT_EQ(Sim->GetAbstractConnection(0, -1, false), 0u);
}

TEST_F(SimulationRun, AbstractConnectomeMatrixIsIndexedPostThenPre) {
    BuildAsymmetricBSNet(*Sim);
    auto M = Sim->GetAbstractConnectome(false);
    ASSERT_EQ(M.size(), 3u);
    EXPECT_EQ(M[1][0], 2u);
    EXPECT_EQ(M[2][1], 1u);
    EXPECT_EQ(M[0][2], 3u);
    size_t Total = 0;
    for (auto& Row : M) for (size_t c : Row) Total += c;
    EXPECT_EQ(Total, 6u);
}

TEST_F(SimulationRun, DenseAndSparseAbstractConnectomeJsonDescribeTheSameConnections) {
    BuildAsymmetricBSNet(*Sim);
    json Dense = Sim->GetAbstractConnectomeJSON(false, false);
    json Sparse = Sim->GetAbstractConnectomeJSON(true, false);

    // Dense is [pre][post].
    ASSERT_EQ(Dense["PrePostNumReceptors"].size(), 3u);
    EXPECT_EQ(Dense["PrePostNumReceptors"], (json{json{0, 2, 0}, json{0, 0, 1}, json{3, 0, 0}}));
    // Sparse lists only non-empty pairs as [pre, post, count].
    EXPECT_EQ(Sparse["PrePostNumReceptors"].size(), 3u);
    EXPECT_EQ(SparseToDense(Sparse["PrePostNumReceptors"], 3), Dense["PrePostNumReceptors"].get<std::vector<std::vector<size_t>>>());
    EXPECT_EQ(Dense["Types"], Sparse["Types"]);
    EXPECT_EQ(Dense["Regions"], Sparse["Regions"]);
}

TEST_F(SimulationRun, NonZeroFlagCountsOnlyReceptorsWithConductance) {
    BuildAsymmetricBSNet(*Sim);
    ASSERT_TRUE(Sim->UpdatePrePostStrength(2, 0, 0.0f));
    EXPECT_EQ(Sim->GetAbstractConnection(2, 0, false), 3u);
    EXPECT_EQ(Sim->GetAbstractConnection(2, 0, true), 0u);
    ASSERT_TRUE(Sim->UpdatePrePostStrength(2, 0, 2.5f));
    EXPECT_EQ(Sim->GetAbstractConnection(2, 0, true), 1u) << "one receptor carries the strength, the others are cleared";
    EXPECT_EQ(Sim->GetAbstractConnection(2, 0, false), 3u);
    json Sparse = Sim->GetAbstractConnectomeJSON(true, true);
    EXPECT_EQ(Sparse["PrePostNumReceptors"].size(), 3u);
    EXPECT_EQ(Sparse["PrePostNumReceptors"][2], (json{2, 0, 1}));
}

TEST_F(SimulationRun, AbstractConnectomeJsonReportsRegionMembershipAndNeuronTypes) {
    BuildAsymmetricBSNet(*Sim);
    BrainRegions::BrainRegion R;
    R.SetName("cortex");
    ASSERT_EQ(Sim->AddRegion(R), 0);
    Sim->NeuralCircuits.at(R.CircuitID)->NeuronIDs = {0, 2};
    json J = Sim->GetAbstractConnectomeJSON(true, false);
    EXPECT_EQ(J["Regions"], (json{{"cortex", json{0, 2}}}));
    EXPECT_EQ(J["Types"].size(), 3u);
}

TEST_F(SimulationRun, LIFCAbstractConnectionCountsAmpaAndGabaButNotNmda) {
    BuildLIFCNet(*Sim, 2);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.5f);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.25f);
    ConnectLIFC(*Sim, 0, 1, Connections::NMDA, 0.1f);
    ConnectLIFC(*Sim, 1, 0, Connections::GABA, 0.2f);
    EXPECT_EQ(Sim->GetAbstractConnection(0, 1, false), 2u);
    EXPECT_EQ(Sim->GetAbstractConnection(1, 0, false), 1u);
    json Dense = Sim->GetAbstractConnectomeJSON(false, false);
    json Sparse = Sim->GetAbstractConnectomeJSON(true, false);
    EXPECT_EQ(Dense["PrePostNumReceptors"], (json{json{0, 2}, json{1, 0}}));
    EXPECT_EQ(SparseToDense(Sparse["PrePostNumReceptors"], 2), Dense["PrePostNumReceptors"].get<std::vector<std::vector<size_t>>>());
}

// ---------------------------------------------------------------- connection strength

TEST_F(SimulationRun, UpdatePrePostStrengthSetsTheConductanceOfAnExistingConnection) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    ConnectBS(*Sim, 1, 2, "AMPA", 1.0f);
    EXPECT_TRUE(Sim->UpdatePrePostStrength(0, 1, 4.5f));
    EXPECT_NEAR(Sim->Receptors[0]->Conductance_nS, 4.5f, 1e-6);
    EXPECT_NEAR(Sim->Receptors[1]->Conductance_nS, 1.0f, 1e-6) << "other connections are untouched";
}

TEST_F(SimulationRun, UpdatePrePostStrengthRejectsUnknownPairsAndNeurons) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    EXPECT_FALSE(Sim->UpdatePrePostStrength(1, 0, 9.0f)) << "no such connection";
    EXPECT_FALSE(Sim->UpdatePrePostStrength(0, 3, 9.0f));
    EXPECT_FALSE(Sim->UpdatePrePostStrength(0, -1, 9.0f));
    EXPECT_NEAR(Sim->Receptors[0]->Conductance_nS, 1.0f, 1e-6);
}

TEST_F(SimulationRun, UpdatePrePostStrengthKeepsOneActiveReceptorWhenAPairHasSeveral) {
    BuildBSNet(*Sim, 2);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    ASSERT_TRUE(Sim->UpdatePrePostStrength(0, 1, 6.0f));
    EXPECT_NEAR(Sim->Receptors[0]->Conductance_nS, 0.0f, 1e-6);
    EXPECT_NEAR(Sim->Receptors[1]->Conductance_nS, 6.0f, 1e-6);
}

TEST_F(SimulationRun, UpdateAllStrengthSetsEveryExistingConnection) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    ConnectBS(*Sim, 1, 2, "GABA", 2.0f);
    ConnectBS(*Sim, 2, 0, "AMPA", 3.0f);
    Sim->UpdateAllStrength(0.0f);
    for (auto& R : Sim->Receptors) EXPECT_NEAR(R->Conductance_nS, 0.0f, 1e-6);
    Sim->UpdateAllStrength(2.5f);
    for (auto& R : Sim->Receptors) EXPECT_NEAR(R->Conductance_nS, 2.5f, 1e-6);
    EXPECT_EQ(Sim->Receptors.size(), 3u) << "no connections are created for absent pairs";
}

TEST_F(SimulationRun, UpdateBatchPrePostStrengthAppliesAllEntriesOrRejectsMismatchedLengths) {
    BuildBSNet(*Sim, 3);
    ConnectBS(*Sim, 0, 1, "AMPA", 1.0f);
    ConnectBS(*Sim, 1, 2, "AMPA", 1.0f);

    EXPECT_FALSE(Sim->UpdateBatchPrePostStrength({0, 1}, {1}, {5.0f, 6.0f}));
    EXPECT_FALSE(Sim->UpdateBatchPrePostStrength({0, 1}, {1, 2}, {5.0f}));
    EXPECT_NEAR(Sim->Receptors[0]->Conductance_nS, 1.0f, 1e-6) << "nothing applied on a length mismatch";
    EXPECT_NEAR(Sim->Receptors[1]->Conductance_nS, 1.0f, 1e-6);

    EXPECT_TRUE(Sim->UpdateBatchPrePostStrength({0, 1}, {1, 2}, {5.0f, 6.0f}));
    EXPECT_NEAR(Sim->Receptors[0]->Conductance_nS, 5.0f, 1e-6);
    EXPECT_NEAR(Sim->Receptors[1]->Conductance_nS, 6.0f, 1e-6);

    EXPECT_TRUE(Sim->UpdateBatchPrePostStrength({}, {}, {}));
    EXPECT_FALSE(Sim->UpdateBatchPrePostStrength({0, 2}, {1, 0}, {7.0f, 7.0f})) << "second pair does not exist";
}

TEST_F(SimulationRun, LIFCUpdatePrePostStrengthSetsTheAbstractedWeight) {
    BuildLIFCNet(*Sim, 3);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.5f);
    ConnectLIFC(*Sim, 1, 2, Connections::AMPA, 0.5f);
    EXPECT_TRUE(Sim->UpdatePrePostStrength(0, 1, 0.25f));
    EXPECT_NEAR(Sim->LIFCReceptorDataVec[0]->weight, 0.5f, 1e-6) << "weight = strength / summed peak conductance";
    EXPECT_NEAR(Sim->LIFCReceptorDataVec[1]->weight, 1.0f, 1e-6);
    EXPECT_FALSE(Sim->UpdatePrePostStrength(2, 0, 1.0f));
    EXPECT_FALSE(Sim->UpdatePrePostStrength(0, 3, 1.0f));
}

// ---------------------------------------------------------------- geometric centre

TEST_F(SimulationRun, GeoCenterIsTheMeanOfTheSomaCentres) {
    BuildBSNet(*Sim, 3); // soma x = 0, 100, 200
    auto C = Sim->GetGeoCenter();
    EXPECT_NEAR(C.x, 100.0f, 1e-3);
    EXPECT_NEAR(C.y, 0.0f, 1e-3);
    EXPECT_NEAR(C.z, 0.0f, 1e-3);
}

TEST_F(SimulationRun, GeoCenterOfASingleNeuronIsItsSoma) {
    BuildBSNet(*Sim, 1);
    auto C = Sim->GetGeoCenter();
    EXPECT_NEAR(C.x, 0.0f, 1e-3);
}

// Bug: dev_frame #114
TEST_F(SimulationRunKnownBug, GeoCenterOfAnEmptySimulationIsAValidPoint) {
    auto C = Sim->GetGeoCenter();
    EXPECT_FALSE(std::isnan(C.x));
    EXPECT_FALSE(std::isnan(C.y));
    EXPECT_FALSE(std::isnan(C.z));
}
