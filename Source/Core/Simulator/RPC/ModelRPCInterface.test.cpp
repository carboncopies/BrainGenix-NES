//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the model-building route handlers in ModelRPCInterface.cpp
                 (compartments, BS/SC/LIFC neurons and edits, staples, receptors, patch clamp
                 DAC/ADC, specific AP times, spontaneous activity, opto), called directly
                 with JSON strings against a real Simulation. Generic table-driven checks
                 (valid request, each required field missing or wrongly typed, unknown or
                 busy simulation) plus route-specific state and enum checks. Tests whose
                 name is in a *KnownBug suite assert the correct behaviour of a real bug
                 (dev_frame D8). (dev_frame NT-11)
*/

#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <RPC/APIStatusCode.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>
#include <TestUtil/RouteHarness.h>

using nlohmann::json;
using BG::NES::TestUtil::RouteHarness;
using BG::NES::Simulator::Simulation;
using BG::NES::Simulator::ModelRPCInterface;
namespace CoreStructs = BG::NES::Simulator::CoreStructs;
namespace Connections = BG::NES::Simulator::Connections;

namespace {

enum class Kind { None, BS, SC, LIFC };
enum class Prep { Nothing, Shapes, Compartments, Neurons };

// ---- Request builders ------------------------------------------------------------------

json SphereReq(int Sim, float X, float Y, float Z) {
    return {{"SimulationID", Sim}, {"Radius_um", 5.0}, {"CenterPosX_um", X}, {"CenterPosY_um", Y}, {"CenterPosZ_um", Z}, {"Name", "sph"}};
}
json CylinderReq(int Sim, float X) {
    return {{"SimulationID", Sim}, {"Point1Radius_um", 1.0}, {"Point1PosX_um", X}, {"Point1PosY_um", 0.0}, {"Point1PosZ_um", 0.0},
            {"Point2Radius_um", 1.0}, {"Point2PosX_um", X}, {"Point2PosY_um", 0.0}, {"Point2PosZ_um", 20.0}, {"Name", "cyl"}};
}

json BSCompJson(int Shape) {
    return {{"ShapeID", Shape}, {"MembranePotential_mV", -65.0}, {"SpikeThreshold_mV", -50.0}, {"DecayTime_ms", 5.0},
            {"RestingPotential_mV", -70.0}, {"AfterHyperpolarizationAmplitude_mV", -5.0}, {"Name", "bsc"}};
}
json LIFCCompJson(int Shape) {
    return {{"ShapeID", Shape}, {"RestingPotential_mV", -70.0}, {"ResetPotential_mV", -75.0}, {"SpikeThreshold_mV", -50.0},
            {"MembraneResistance_MOhm", 100.0}, {"MembraneCapacitance_pF", 100.0}, {"AfterHyperpolarizationAmplitude_mV", -5.0}, {"Name", "lifc"}};
}
json BSNeuronJson(int Soma, int Axon) {
    return {{"SomaID", Soma}, {"AxonID", Axon}, {"MembranePotential_mV", -65.0}, {"RestingPotential_mV", -70.0},
            {"SpikeThreshold_mV", -50.0}, {"DecayTime_ms", 5.0}, {"AfterHyperpolarizationAmplitude_mV", -5.0},
            {"PostsynapticPotentialRiseTime_ms", 1.0}, {"PostsynapticPotentialDecayTime_ms", 4.0},
            {"PostsynapticPotentialAmplitude_nA", 0.5}, {"Name", "bsn"}};
}
json SCNeuronJson(const std::vector<int>& Soma, const std::vector<int>& Dendrite, const std::vector<int>& Axon) {
    return {{"SomaIDs", Soma}, {"DendriteIDs", Dendrite}, {"AxonIDs", Axon}, {"MembranePotential_mV", -65.0},
            {"RestingPotential_mV", -70.0}, {"SpikeThreshold_mV", -50.0}, {"DecayTime_ms", 5.0},
            {"AfterHyperpolarizationAmplitude_mV", -5.0}, {"PostsynapticPotentialRiseTime_ms", 1.0},
            {"PostsynapticPotentialDecayTime_ms", 4.0}, {"PostsynapticPotentialAmplitude_nA", 0.5}, {"Name", "scn"}};
}
json LIFCNeuronJson(const std::vector<int>& Soma, const std::vector<int>& Dendrite, const std::vector<int>& Axon) {
    return {{"SomaIDs", Soma}, {"DendriteIDs", Dendrite}, {"AxonIDs", Axon},
            {"RestingPotential_mV", -70.0}, {"ResetPotential_mV", -75.0}, {"SpikeThreshold_mV", -50.0},
            {"MembraneResistance_MOhm", 100.0}, {"MembraneCapacitance_pF", 100.0}, {"RefractoryPeriod_ms", 2.0},
            {"SpikeDepolarization_mV", 20.0}, {"UpdateMethod", "ExpEulerCm"}, {"ResetMethod", "ToVm"},
            {"AfterHyperpolarizationReversalPotential_mV", -90.0},
            {"FastAfterHyperpolarizationRise_ms", 1.0}, {"FastAfterHyperpolarizationDecay_ms", 5.0},
            {"FastAfterHyperpolarizationPeakConductance_nS", 1.0}, {"FastAfterHyperpolarizationMaxPeakConductance_nS", 2.0},
            {"FastAfterHyperpolarizationHalfActConstant", 1.0},
            {"SlowAfterHyperpolarizationRise_ms", 10.0}, {"SlowAfterHyperpolarizationDecay_ms", 50.0},
            {"SlowAfterHyperpolarizationPeakConductance_nS", 0.5}, {"SlowAfterHyperpolarizationMaxPeakConductance_nS", 1.0},
            {"SlowAfterHyperpolarizationHalfActConstant", 1.0},
            {"AfterHyperpolarizationSaturationModel", "clip"},
            {"FatigueThreshold", 1.0}, {"FatigueRecoveryTime_ms", 100.0},
            {"AfterDepolarizationReversalPotential_mV", 0.0}, {"AfterDepolarizationRise_ms", 1.0},
            {"AfterDepolarizationDecay_ms", 5.0}, {"AfterDepolarizationPeakConductance_nS", 1.0},
            {"AfterDepolarizationSaturationMultiplier", 1.0}, {"AfterDepolarizationRecoveryTime_ms", 100.0},
            {"AfterDepolarizationDepletion", 0.1}, {"AfterDepolarizationSaturationModel", "clip"},
            {"AdaptiveThresholdDiffPerSpike", 0.0}, {"AdaptiveTresholdRecoveryTime_ms", 100.0},
            {"AdaptiveThresholdDiffPotential_mV", 0.0}, {"AdaptiveThresholdFloor_mV", -50.0},
            {"AdaptiveThresholdFloorDeltaPerSpike_mV", 0.0}, {"AdaptiveThresholdFloorRecoveryTime_ms", 100.0},
            {"Name", "lifcn"}};
}
json ReceptorJson(int Src, int Dst) {
    return {{"SourceCompartmentID", Src}, {"DestinationCompartmentID", Dst}, {"Conductance_nS", 2.0},
            {"TimeConstantRise_ms", 1.0}, {"TimeConstantDecay_ms", 3.0}, {"Neurotransmitter", "AMPA"},
            {"ReceptorMorphology", 0}, {"Name", "rec"}};
}
json LIFCReceptorJson(int Src, int Dst) {
    return {{"SourceCompartmentID", Src}, {"DestinationCompartmentID", Dst}, {"ReversalPotential_mV", 0.0},
            {"PSPRise_ms", 1.0}, {"PSPDecay_ms", 3.0}, {"PeakConductance_nS", 2.0}, {"Weight", 0.5},
            {"OnsetDelay_ms", 1.5}, {"Neurotransmitter", "AMPA"}, {"voltage_gated", false},
            {"STDP_Method", "None"}, {"STDP_A_pos", 0.1}, {"STDP_A_neg", 0.1}, {"STDP_Tau_pos", 20.0},
            {"STDP_Tau_neg", 20.0}, {"STDP_Shift", 0.0}, {"ReceptorMorphology", 0}, {"Name", "lrec"}};
}
json NetmorphReceptorJson(int Src, int Dst) {
    return {{"SourceCompartmentID", Src}, {"DestinationCompartmentID", Dst}, {"ReversalPotential_mV", 0.0},
            {"PSPRise_ms", 1.0}, {"PSPDecay_ms", 3.0}, {"ReceptorPeakConductance_nS", 0.5}, {"ReceptorQuantity", 4},
            {"HillocDistance_um", 100.0}, {"Velocity_mps", 1.0}, {"SynapticDelay_ms", 0.5},
            {"Neurotransmitter", "GABA"}, {"voltage_gated", false}, {"Weight", 0.5},
            {"STDP_Method", "Hebbian"}, {"STDP_A_pos", 0.1}, {"STDP_A_neg", 0.1}, {"STDP_Tau_pos", 20.0},
            {"STDP_Tau_neg", 20.0}, {"STDP_Shift", 0.0}, {"ReceptorMorphology", 0}, {"Name", "nrec"}};
}

// ---- Fixture ---------------------------------------------------------------------------

class ModelRouteTest : public ::testing::Test {
protected:
    RouteHarness& H() { return RouteHarness::Get(); }
    ModelRPCInterface& M() { return H().Model(); }

    static json Parse(const std::string& S) { return json::parse(S); }
    static json With(json Req, int Sim) {
        Req["SimulationID"] = Sim;
        return Req;
    }
    json Do(const std::function<std::string(const std::string&)>& Fn, json Req, int Sim) { return Parse(Fn(With(std::move(Req), Sim).dump())); }

    // Four shapes: 0 sphere at x=0, 1 cylinder, 2 sphere at x=100, 3 cylinder. All created before any
    // compartment so that no later shape insertion can invalidate cached ShapePtrs (dev_frame #112).
    void AddShapes(int Sim) {
        ASSERT_EQ(H().Geometry().SphereCreate(SphereReq(Sim, 0, 0, 0).dump()), "{\"ShapeID\":0,\"StatusCode\":0}");
        ASSERT_EQ(Parse(H().Geometry().CylinderCreate(CylinderReq(Sim, 0).dump()))["ShapeID"], 1);
        ASSERT_EQ(Parse(H().Geometry().SphereCreate(SphereReq(Sim, 100, 0, 0).dump()))["ShapeID"], 2);
        ASSERT_EQ(Parse(H().Geometry().CylinderCreate(CylinderReq(Sim, 100).dump()))["ShapeID"], 3);
    }
    std::function<std::string(const std::string&)> CompFn(Kind K) {
        auto& M_ = M();
        switch (K) {
            case Kind::BS: return [&M_](const std::string& R) { return M_.BSCreate(R); };
            case Kind::SC: return [&M_](const std::string& R) { return M_.SCCreate(R); };
            default: return [&M_](const std::string& R) { return M_.LIFCCreate(R); };
        }
    }
    void AddCompartments(int Sim, Kind K) {
        for (int Shape = 0; Shape < 4; Shape++) {
            json Req = (K == Kind::LIFC) ? LIFCCompJson(Shape) : BSCompJson(Shape);
            json Resp = Do(CompFn(K), Req, Sim);
            ASSERT_EQ(Resp, json({{"StatusCode", 0}, {"CompartmentID", Shape}}));
        }
    }
    // Two neurons: (soma 0, axon 1) and (soma 2, axon 3).
    void AddNeurons(int Sim, Kind K) {
        for (int N = 0; N < 2; N++) {
            json Resp;
            if (K == Kind::BS) Resp = Do([this](const std::string& R) { return M().BSNeuronCreate(R); }, BSNeuronJson(2 * N, 2 * N + 1), Sim);
            if (K == Kind::SC) Resp = Do([this](const std::string& R) { return M().SCNeuronCreate(R); }, SCNeuronJson({2 * N}, {}, {2 * N + 1}), Sim);
            if (K == Kind::LIFC) Resp = Do([this](const std::string& R) { return M().LIFCNeuronCreate(R); }, LIFCNeuronJson({2 * N}, {}, {2 * N + 1}), Sim);
            ASSERT_EQ(Resp, json({{"StatusCode", 0}, {"NeuronID", N}}));
        }
    }
    int SimWith(Kind K, Prep P) {
        int Sim = H().NewSim();
        if (P >= Prep::Shapes) AddShapes(Sim);
        if (P >= Prep::Compartments && K != Kind::None) AddCompartments(Sim, K);
        if (P >= Prep::Neurons && K != Kind::None) AddNeurons(Sim, K);
        return Sim;
    }
    // Compact summary of everything a route could change, to show a rejected request changed nothing.
    static std::string Fingerprint(Simulation* S) {
        size_t DacData = 0, Stim = 0;
        double AdcSum = 0;
        for (auto& D : S->PatchClampDACs) DacData += D.ControlData.size();
        for (auto& A : S->PatchClampADCs) AdcSum += A.Timestep_ms + A.RecordedData_mV.size();
        for (auto& N : S->Neurons) Stim += N->TDirectStim_ms.size() + size_t(N->opto_wavelength_nm);
        return std::to_string(S->Collection.Size()) + "/" + std::to_string(S->BSCompartments.size()) + "/" +
               std::to_string(S->LIFCCompartments.size()) + "/" + std::to_string(S->Neurons.size()) + "/" +
               std::to_string(S->Staples.size()) + "/" + std::to_string(S->Receptors.size()) + "/" +
               std::to_string(S->LIFCReceptors.size()) + "/" + std::to_string(S->PatchClampDACs.size()) + "/" +
               std::to_string(DacData) + "/" + std::to_string(S->PatchClampADCs.size()) + "/" + std::to_string(AdcSum) + "/" +
               std::to_string(Stim) + "/" + std::to_string(int(S->SimNeuronClass));
    }
};

// ---- Generic table over every route ----------------------------------------------------

struct RouteSpec {
    std::string Name;
    std::function<std::string(ModelRPCInterface&, const std::string&)> Call;
    Kind K;
    Prep P;
    std::function<void(RouteHarness&, int)> ExtraPrep;  // e.g. create a DAC first
    json Valid;                                          // without SimulationID
    std::string IDKey;                                   // "" when the response carries no ID
    std::function<size_t(Simulation*)> Count;            // number of created objects (creators only)
    std::vector<std::string> Optional;                   // keys that may be omitted
    std::vector<std::string> NoStateCheck;               // keys whose wrong type partially applies (see ModelRouteKnownBug)
};

std::vector<RouteSpec> AllRoutes() {
    using MI = ModelRPCInterface;
    auto DAC = [](RouteHarness& H, int Sim) {
        json R = {{"SimulationID", Sim}, {"DestinationCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "d"}};
        H.Model().PatchClampDACCreate(R.dump());
    };
    auto ADC = [](RouteHarness& H, int Sim) {
        json R = {{"SimulationID", Sim}, {"SourceCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "a"}};
        H.Model().PatchClampADCCreate(R.dump());
    };
    json Pos = {{"ClampPosX_um", 1.0}, {"ClampPosY_um", 2.0}, {"ClampPosZ_um", 3.0}};
    json DacCreate = Pos; DacCreate["DestinationCompartmentID"] = 0; DacCreate["Name"] = "dac";
    json AdcCreate = Pos; AdcCreate["SourceCompartmentID"] = 0; AdcCreate["Name"] = "adc";

    std::vector<RouteSpec> R;
    R.push_back({"Staple", [](MI& M, const std::string& S) { return M.StapleCreate(S); }, Kind::None, Prep::Nothing, nullptr,
                 {{"SourceCompartmentID", 0}, {"DestinationCompartmentID", 1}, {"Name", "st"}}, "StapleID",
                 [](Simulation* S) { return S->Staples.size(); }, {}, {}});
    R.push_back({"Receptor", [](MI& M, const std::string& S) { return M.ReceptorCreate(S); }, Kind::BS, Prep::Neurons, nullptr,
                 ReceptorJson(1, 2), "ReceptorID", [](Simulation* S) { return S->Receptors.size(); }, {}, {}});
    R.push_back({"LIFCReceptor", [](MI& M, const std::string& S) { return M.LIFCReceptorCreate(S); }, Kind::LIFC, Prep::Neurons, nullptr,
                 LIFCReceptorJson(1, 2), "ReceptorID", [](Simulation* S) { return S->LIFCReceptors.size(); }, {}, {}});
    R.push_back({"NetmorphLIFCReceptor", [](MI& M, const std::string& S) { return M.NetmorphLIFCReceptorCreate(S); }, Kind::LIFC, Prep::Neurons, nullptr,
                 NetmorphReceptorJson(1, 2), "ReceptorID", [](Simulation* S) { return S->LIFCReceptors.size(); }, {}, {}});
    R.push_back({"BSCompartment", [](MI& M, const std::string& S) { return M.BSCreate(S); }, Kind::None, Prep::Shapes, nullptr,
                 BSCompJson(0), "CompartmentID", [](Simulation* S) { return S->BSCompartments.size(); }, {}, {}});
    R.push_back({"SCCompartment", [](MI& M, const std::string& S) { return M.SCCreate(S); }, Kind::None, Prep::Shapes, nullptr,
                 BSCompJson(0), "CompartmentID", [](Simulation* S) { return S->BSCompartments.size(); }, {}, {}});
    R.push_back({"LIFCCompartment", [](MI& M, const std::string& S) { return M.LIFCCreate(S); }, Kind::None, Prep::Shapes, nullptr,
                 LIFCCompJson(0), "CompartmentID", [](Simulation* S) { return S->LIFCCompartments.size(); }, {}, {}});
    R.push_back({"BSNeuron", [](MI& M, const std::string& S) { return M.BSNeuronCreate(S); }, Kind::BS, Prep::Compartments, nullptr,
                 BSNeuronJson(0, 1), "NeuronID", [](Simulation* S) { return S->Neurons.size(); }, {}, {}});
    R.push_back({"SCNeuron", [](MI& M, const std::string& S) { return M.SCNeuronCreate(S); }, Kind::SC, Prep::Compartments, nullptr,
                 SCNeuronJson({0}, {2}, {1}), "NeuronID", [](Simulation* S) { return S->Neurons.size(); }, {}, {}});
    R.push_back({"LIFCNeuron", [](MI& M, const std::string& S) { return M.LIFCNeuronCreate(S); }, Kind::LIFC, Prep::Compartments, nullptr,
                 LIFCNeuronJson({0}, {2}, {1}), "NeuronID", [](Simulation* S) { return S->Neurons.size(); }, {}, {}});
    R.push_back({"SCNeuronEdit", [](MI& M, const std::string& S) { return M.SCNeuronEdit(S); }, Kind::SC, Prep::Neurons, nullptr,
                 {{"NeuronIDs", json::array({0})}, {"MembranePotential_mV", -55.0}}, "", nullptr,
                 {"MembranePotential_mV", "RestingPotential_mV", "SpikeThreshold_mV", "DecayTime_ms", "AfterHyperpolarizationAmplitude_mV"},
                 {"MembranePotential_mV"}});
    R.push_back({"LIFCNeuronEdit", [](MI& M, const std::string& S) { return M.LIFCNeuronEdit(S); }, Kind::LIFC, Prep::Neurons, nullptr,
                 {{"NeuronIDs", json::array({0})}, {"SpikeThreshold_mV", -45.0}}, "", nullptr,
                 {"RestingPotential_mV", "ResetPotential_mV", "SpikeThreshold_mV", "MembraneResistance_MOhm", "MembraneCapacitance_pF",
                  "RefractoryPeriod_ms", "SpikeDepolarization_mV"},
                 {"SpikeThreshold_mV"}});
    R.push_back({"PatchClampDACCreate", [](MI& M, const std::string& S) { return M.PatchClampDACCreate(S); }, Kind::None, Prep::Nothing, nullptr,
                 DacCreate, "PatchClampDACID", [](Simulation* S) { return S->PatchClampDACs.size(); }, {}, {}});
    R.push_back({"PatchClampDACSetOutputList", [](MI& M, const std::string& S) { return M.PatchClampDACSetOutputList(S); }, Kind::None, Prep::Nothing, DAC,
                 {{"PatchClampDACID", 0}, {"ControlData", json::array({json::array({0.0, -70.0}), json::array({5.0, -20.0})})}}, "", nullptr, {},
                 {"ControlData"}});
    R.push_back({"PatchClampADCCreate", [](MI& M, const std::string& S) { return M.PatchClampADCCreate(S); }, Kind::None, Prep::Nothing, nullptr,
                 AdcCreate, "PatchClampADCID", [](Simulation* S) { return S->PatchClampADCs.size(); }, {}, {}});
    R.push_back({"PatchClampADCSetSampleRate", [](MI& M, const std::string& S) { return M.PatchClampADCSetSampleRate(S); }, Kind::None, Prep::Nothing, ADC,
                 {{"PatchClampADCID", 0}, {"Timestep_ms", 0.5}}, "", nullptr, {}, {}});
    R.push_back({"PatchClampADCGetRecordedData", [](MI& M, const std::string& S) { return M.PatchClampADCGetRecordedData(S); }, Kind::None, Prep::Nothing, ADC,
                 {{"PatchClampADCID", 0}}, "", nullptr, {}, {}});
    R.push_back({"SetSpecificAPTimes", [](MI& M, const std::string& S) { return M.SetSpecificAPTimes(S); }, Kind::SC, Prep::Neurons, nullptr,
                 {{"TimeNeuronPairs", json::array({json::array({1.5, 0}), json::array({2.5, 1})})}}, "", nullptr, {}, {}});
    R.push_back({"SetSpontaneousActivity", [](MI& M, const std::string& S) { return M.SetSpontaneousActivity(S); }, Kind::SC, Prep::Neurons, nullptr,
                 {{"SpikeIntervalMean_ms", 10.0}, {"SpikeIntervalStDev_ms", 1.0}, {"NeuronIDs", json::array({0})}}, "", nullptr, {}, {}});
    R.push_back({"OptoModifyInjection", [](MI& M, const std::string& S) { return M.OptoModifyInjection(S); }, Kind::SC, Prep::Neurons, nullptr,
                 {{"InjectLocationX_um", 0.0}, {"InjectLocationY_um", 0.0}, {"InjectLocationZ_um", 0.0}, {"Radius", 10.0}, {"Wavelength", 470}, {"Type", 1}},
                 "", nullptr, {}, {}});
    R.push_back({"OptoActivation", [](MI& M, const std::string& S) { return M.OptoActivation(S); }, Kind::SC, Prep::Neurons, nullptr,
                 {{"LaserActivationTimes", json::array({1.0, 2.0})}, {"TargetWavelength", 470}}, "", nullptr, {}, {}});
    return R;
}

bool Contains(const std::vector<std::string>& V, const std::string& K) {
    for (auto& S : V) if (S == K) return true;
    return false;
}

class ModelGenericRouteTest : public ModelRouteTest {
protected:
    int Prepared(const RouteSpec& R) {
        int Sim = SimWith(R.K, R.P);
        if (R.ExtraPrep) R.ExtraPrep(H(), Sim);
        return Sim;
    }
    std::string CallRoute(const RouteSpec& R, const json& Req) { return R.Call(M(), Req.dump()); }

    // A value of the wrong JSON type for whatever is at Original.
    static std::vector<json> WrongTypes(const json& Original) {
        if (Original.is_string()) return {json(5), json(true), json::array()};
        if (Original.is_boolean()) return {json("x"), json(5)};
        if (Original.is_array()) {
            json BadElement = json::array({"x"});
            return {json("text"), BadElement};
        }
        return {json("text"), json(true), json(nullptr)};  // numbers
    }
};

TEST_F(ModelGenericRouteTest, ValidRequestSucceedsAndCreatesExactlyOneObject) {
    for (auto& R : AllRoutes()) {
        int Sim = Prepared(R);
        size_t Before = R.Count ? R.Count(H().Sim(Sim)) : 0;
        json Resp = Parse(CallRoute(R, With(R.Valid, Sim)));
        EXPECT_EQ(Resp["StatusCode"], 0) << R.Name << ": " << Resp.dump();
        if (!R.IDKey.empty()) {
            EXPECT_EQ(Resp[R.IDKey], int(Before)) << R.Name;
            EXPECT_EQ(R.Count(H().Sim(Sim)), Before + 1) << R.Name;
        }
    }
}

TEST_F(ModelGenericRouteTest, CreatedObjectsGetSequentialIds) {
    for (auto& R : AllRoutes()) {
        if (R.IDKey.empty() || R.Name.find("Receptor") != std::string::npos) continue;  // receptors need distinct pairs, see below
        int Sim = Prepared(R);
        size_t Before = R.Count(H().Sim(Sim));
        // Neurons: soma 0 is shared by the valid request but the handlers do not reject re-use.
        for (int I = 0; I < 3; I++) {
            json Resp = Parse(CallRoute(R, With(R.Valid, Sim)));
            ASSERT_EQ(Resp["StatusCode"], 0) << R.Name << " #" << I;
            EXPECT_EQ(Resp[R.IDKey], int(Before) + I) << R.Name;
        }
    }
}

TEST_F(ModelGenericRouteTest, EachMissingRequiredFieldIsStatus2AndChangesNothing) {
    for (auto& R : AllRoutes()) {
        int Sim = Prepared(R);
        std::string Before = Fingerprint(H().Sim(Sim));
        for (auto It = R.Valid.begin(); It != R.Valid.end(); ++It) {
            if (Contains(R.Optional, It.key())) continue;
            json Req = With(R.Valid, Sim);
            Req.erase(It.key());
            EXPECT_EQ(Parse(CallRoute(R, Req)), json({{"StatusCode", 2}})) << R.Name << " missing " << It.key();
        }
        EXPECT_EQ(Fingerprint(H().Sim(Sim)), Before) << R.Name;
    }
}

TEST_F(ModelGenericRouteTest, EachWronglyTypedFieldIsStatus2AndChangesNothing) {
    for (auto& R : AllRoutes()) {
        int Sim = Prepared(R);
        for (auto It = R.Valid.begin(); It != R.Valid.end(); ++It) {
            // Wrongly typed optional edit fields and the DAC ControlData are applied partially
            // before the error is returned: covered by ModelRouteKnownBug.
            bool CheckState = !Contains(R.NoStateCheck, It.key());
            // A wrongly typed *optional* float is reported as status 2 too (checked in the KnownBug suite).
            if (Contains(R.Optional, It.key())) continue;
            std::string Before = Fingerprint(H().Sim(Sim));
            for (const json& Wrong : WrongTypes(It.value())) {
                json Req = With(R.Valid, Sim);
                Req[It.key()] = Wrong;
                EXPECT_EQ(Parse(CallRoute(R, Req)), json({{"StatusCode", 2}})) << R.Name << " " << It.key() << " = " << Wrong.dump();
                if (CheckState) EXPECT_EQ(Fingerprint(H().Sim(Sim)), Before) << R.Name << " " << It.key() << " = " << Wrong.dump();
            }
        }
    }
}

TEST_F(ModelGenericRouteTest, MissingWronglyTypedOrUnknownSimulationIdIsStatus2) {
    for (auto& R : AllRoutes()) {
        int Sim = Prepared(R);
        json NoId = R.Valid;
        EXPECT_EQ(Parse(CallRoute(R, NoId)), json({{"StatusCode", 2}})) << R.Name << " no id";
        for (const json& Bad : {json("0"), json(nullptr), json(-1), json(Sim + 1000)}) {
            json Req = R.Valid;
            Req["SimulationID"] = Bad;
            EXPECT_EQ(Parse(CallRoute(R, Req)), json({{"StatusCode", 2}})) << R.Name << " id " << Bad.dump();
        }
    }
}

TEST_F(ModelGenericRouteTest, BusySimulationIsStatus5AndChangesNothingThenWorksWhenIdle) {
    for (auto& R : AllRoutes()) {
        int Sim = Prepared(R);
        std::string Before = Fingerprint(H().Sim(Sim));
        H().Sim(Sim)->IsProcessing = true;
        EXPECT_EQ(Parse(CallRoute(R, With(R.Valid, Sim))), json({{"StatusCode", 5}})) << R.Name;
        H().Sim(Sim)->IsProcessing = false;
        H().Sim(Sim)->WorkRequested = true;
        EXPECT_EQ(Parse(CallRoute(R, With(R.Valid, Sim))), json({{"StatusCode", 5}})) << R.Name << " (WorkRequested)";
        H().Sim(Sim)->WorkRequested = false;
        EXPECT_EQ(Fingerprint(H().Sim(Sim)), Before) << R.Name;
        EXPECT_EQ(Parse(CallRoute(R, With(R.Valid, Sim)))["StatusCode"], 0) << R.Name;
    }
}

const std::map<std::string, std::string>& RouteNames() {
    static const std::map<std::string, std::string> Names = {
        {"Staple", "Simulation/Staple/Create"}, {"Receptor", "Simulation/Receptor/Create"},
        {"LIFCReceptor", "Simulation/LIFCReceptor/Create"}, {"NetmorphLIFCReceptor", "Simulation/NetmorphLIFCReceptor/Create"},
        {"BSCompartment", "Simulation/Compartments/BS/Create"}, {"SCCompartment", "Simulation/Compartments/SC/Create"},
        {"LIFCCompartment", "Simulation/Compartments/LIFC/Create"}, {"BSNeuron", "Simulation/Neuron/BS/Create"},
        {"SCNeuron", "Simulation/Neuron/SC/Create"}, {"LIFCNeuron", "Simulation/Neuron/LIFC/Create"},
        {"SCNeuronEdit", "Simulation/Neuron/SC/Edit"}, {"LIFCNeuronEdit", "Simulation/Neuron/LIFC/Edit"},
        {"PatchClampDACCreate", "Simulation/PatchClampDAC/Create"}, {"PatchClampDACSetOutputList", "Simulation/PatchClampDAC/SetOutputList"},
        {"PatchClampADCCreate", "Simulation/PatchClampADC/Create"}, {"PatchClampADCSetSampleRate", "Simulation/PatchClampADC/SetSampleRate"},
        {"PatchClampADCGetRecordedData", "Simulation/PatchClampADC/GetRecordedData"}, {"SetSpecificAPTimes", "Simulation/SetSpecificAPTimes"},
        {"SetSpontaneousActivity", "Simulation/SetSpontaneousActivity"}, {"OptoModifyInjection", "Simulation/OptoModifyInjection"},
        {"OptoActivation", "Simulation/OptoActivation"}};
    return Names;
}

TEST_F(ModelGenericRouteTest, EveryRouteIsRegisteredOnTheManager) {
    for (auto& R : AllRoutes()) {
        ASSERT_EQ(RouteNames().count(R.Name), 1u) << R.Name;
        int Sim = Prepared(R);
        json Batch = json::array({json({{"ReqID", 3}, {RouteNames().at(R.Name), With(R.Valid, Sim)}})});
        json Resp = Parse(H().Manager().NESRequest(Batch.dump()));
        ASSERT_TRUE(Resp.is_array()) << R.Name;
        ASSERT_EQ(Resp.size(), 1u) << R.Name;
        EXPECT_EQ(Resp[0]["StatusCode"], 0) << R.Name << ": " << Resp.dump();
        EXPECT_EQ(Resp[0]["ReqID"], 3) << R.Name;
    }
}

// The model journal (StoredRequests) is what SaveModel writes and a later load replays, so a
// request must be stored under the route name it is registered with. The DAC create route
// stores "Simulation/OatchClampDAC/Create": see ModelRouteKnownBug.
TEST_F(ModelGenericRouteTest, ValidRequestIsJournaledUnderItsRegisteredRouteName) {
    for (auto& R : AllRoutes()) {
        if (R.Name == "PatchClampDACCreate") continue;
        int Sim = Prepared(R);
        size_t Before = H().Sim(Sim)->NumStoredRequests();
        ASSERT_EQ(Parse(CallRoute(R, With(R.Valid, Sim)))["StatusCode"], 0) << R.Name;
        json Journal = Parse(H().Sim(Sim)->StoredRequestsToNESRequestArray());
        ASSERT_TRUE(Journal.is_array()) << R.Name;
        ASSERT_EQ(Journal.size(), Before + 1) << R.Name;
        const json& Last = Journal.back();
        EXPECT_TRUE(Last.contains(RouteNames().at(R.Name))) << R.Name << ": " << Last.dump();
        EXPECT_EQ(Last.value(RouteNames().at(R.Name), json::object()).value("SimulationID", -1), Sim) << R.Name;
    }
}

// ---- Compartments -----------------------------------------------------------------------

TEST_F(ModelRouteTest, CompartmentsStoreTheirParametersAndShape) {
    int Sim = SimWith(Kind::None, Prep::Shapes);
    json Resp = Do(CompFn(Kind::BS), BSCompJson(2), Sim);
    EXPECT_EQ(Resp, json({{"StatusCode", 0}, {"CompartmentID", 0}}));
    ASSERT_EQ(H().Sim(Sim)->BSCompartments.size(), 1u);
    const auto& C = H().Sim(Sim)->BSCompartments[0];
    EXPECT_EQ(C.ShapeID, 2);
    EXPECT_EQ(C.ID, 0);
    EXPECT_NEAR(C.MembranePotential_mV, -65.0f, 1e-5);
    EXPECT_NEAR(C.SpikeThreshold_mV, -50.0f, 1e-5);
    EXPECT_NEAR(C.DecayTime_ms, 5.0f, 1e-5);
    EXPECT_NEAR(C.RestingPotential_mV, -70.0f, 1e-5);
    EXPECT_NEAR(C.AfterHyperpolarizationAmplitude_mV, -5.0f, 1e-5);
    EXPECT_EQ(C.Name, "bsc");
    ASSERT_NE(C.ShapePtr, nullptr);
    EXPECT_NEAR(C.ShapePtr->Center_um.x, 100.0f, 1e-5);
    EXPECT_EQ(H().Sim(Sim)->SimNeuronClass, BG::NES::Simulator::BSNEURONS);

    int Sim2 = SimWith(Kind::None, Prep::Shapes);
    EXPECT_EQ(Do(CompFn(Kind::SC), BSCompJson(1), Sim2)["CompartmentID"], 0);
    EXPECT_EQ(H().Sim(Sim2)->SimNeuronClass, BG::NES::Simulator::SCNEURONS);

    int Sim3 = SimWith(Kind::None, Prep::Shapes);
    EXPECT_EQ(Do(CompFn(Kind::LIFC), LIFCCompJson(3), Sim3)["CompartmentID"], 0);
    ASSERT_EQ(H().Sim(Sim3)->LIFCCompartments.size(), 1u);
    const auto& L = H().Sim(Sim3)->LIFCCompartments[0];
    EXPECT_EQ(L.ShapeID, 3);
    EXPECT_NEAR(L.ResetPotential_mV, -75.0f, 1e-5);
    EXPECT_NEAR(L.MembraneResistance_MOhm, 100.0f, 1e-4);
    EXPECT_NEAR(L.MembraneCapacitance_pF, 100.0f, 1e-4);
    EXPECT_EQ(H().Sim(Sim3)->SimNeuronClass, BG::NES::Simulator::LIFCNEURONS);
}

TEST_F(ModelRouteTest, CompartmentWithUnknownShapeIsStatus2) {
    int Sim = SimWith(Kind::None, Prep::Shapes);  // shape IDs 0..3 exist
    for (Kind K : {Kind::BS, Kind::SC, Kind::LIFC}) {
        for (int Shape : {4, 100, -1}) {
            json Req = (K == Kind::LIFC) ? LIFCCompJson(Shape) : BSCompJson(Shape);
            EXPECT_EQ(Do(CompFn(K), Req, Sim), json({{"StatusCode", 2}})) << int(K) << " shape " << Shape;
        }
    }
    EXPECT_EQ(H().Sim(Sim)->BSCompartments.size(), 0u);
    EXPECT_EQ(H().Sim(Sim)->LIFCCompartments.size(), 0u);
}

TEST_F(ModelRouteTest, CompartmentOnSimulationWithoutShapesIsStatus2) {
    int Sim = H().NewSim();
    EXPECT_EQ(Do(CompFn(Kind::BS), BSCompJson(0), Sim), json({{"StatusCode", 2}}));
}

TEST_F(ModelRouteTest, MixingNeuronClassesIsRejected) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);  // fixes the class to BS
    EXPECT_EQ(Do(CompFn(Kind::SC), BSCompJson(0), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(CompFn(Kind::LIFC), LIFCCompJson(0), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do([this](const std::string& R) { return M().SCNeuronCreate(R); }, SCNeuronJson({0}, {}, {1}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCNeuronCreate(R); }, LIFCNeuronJson({0}, {}, {1}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->BSCompartments.size(), 4u);
    EXPECT_EQ(H().Sim(Sim)->LIFCCompartments.size(), 0u);
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 0u);
}

// ---- Neurons ----------------------------------------------------------------------------

TEST_F(ModelRouteTest, BSNeuronStoresParametersAndLinksItsCompartments) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);
    auto Create = [this](const std::string& R) { return M().BSNeuronCreate(R); };
    EXPECT_EQ(Do(Create, BSNeuronJson(0, 1), Sim), json({{"StatusCode", 0}, {"NeuronID", 0}}));
    EXPECT_EQ(Do(Create, BSNeuronJson(2, 3), Sim), json({{"StatusCode", 0}, {"NeuronID", 1}}));
    ASSERT_EQ(H().Sim(Sim)->Neurons.size(), 2u);
    auto* N = dynamic_cast<BG::NES::Simulator::BallAndStick::BSNeuron*>(H().Sim(Sim)->Neurons[1].get());
    ASSERT_NE(N, nullptr);
    EXPECT_EQ(N->build_data.SomaCompartmentID, 2);
    EXPECT_EQ(N->build_data.AxonCompartmentID, 3);
    EXPECT_EQ(N->ID, 1);
    EXPECT_EQ(N->build_data.Name, "bsn");
    EXPECT_NEAR(N->build_data.SpikeThreshold_mV, -50.0f, 1e-5);
    EXPECT_NEAR(N->build_data.PostsynapticPotentialAmplitude_nA, 0.5f, 1e-5);
    EXPECT_EQ(H().Sim(Sim)->FindNeuronByCompartment(3), N);
    EXPECT_EQ(H().Sim(Sim)->FindNeuronByCompartment(2), N);
}

TEST_F(ModelRouteTest, BSNeuronWithUnknownCompartmentIsStatus2) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);  // compartments 0..3
    auto Create = [this](const std::string& R) { return M().BSNeuronCreate(R); };
    EXPECT_EQ(Do(Create, BSNeuronJson(4, 1), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(Create, BSNeuronJson(0, 4), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(Create, BSNeuronJson(-1, 1), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(Create, BSNeuronJson(0, -1), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 0u);
}

TEST_F(ModelRouteTest, SCNeuronStoresCompartmentListsAndRequiresASoma) {
    int Sim = SimWith(Kind::SC, Prep::Compartments);
    auto Create = [this](const std::string& R) { return M().SCNeuronCreate(R); };
    EXPECT_EQ(Do(Create, SCNeuronJson({0, 2}, {1}, {3}), Sim), json({{"StatusCode", 0}, {"NeuronID", 0}}));
    auto* N = dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(Sim)->Neurons.at(0).get());
    ASSERT_NE(N, nullptr);
    EXPECT_EQ(N->build_data.SomaCompartmentIDs, (std::vector<int>{0, 2}));
    EXPECT_EQ(N->build_data.DendriteCompartmentIDs, (std::vector<int>{1}));
    EXPECT_EQ(N->build_data.AxonCompartmentIDs, (std::vector<int>{3}));
    EXPECT_EQ(N->build_data.Name, "scn");
    EXPECT_NEAR(N->build_data.DecayTime_ms, 5.0f, 1e-5);
    EXPECT_EQ(H().Sim(Sim)->FindNeuronByCompartment(3), N);

    EXPECT_EQ(Do(Create, SCNeuronJson({}, {1}, {3}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 1u);
}

TEST_F(ModelRouteTest, LIFCNeuronStoresParametersAndRequiresASoma) {
    int Sim = SimWith(Kind::LIFC, Prep::Compartments);
    auto Create = [this](const std::string& R) { return M().LIFCNeuronCreate(R); };
    EXPECT_EQ(Do(Create, LIFCNeuronJson({0}, {1}, {3}), Sim), json({{"StatusCode", 0}, {"NeuronID", 0}}));
    auto* N = dynamic_cast<BG::NES::Simulator::LIFCNeuron*>(H().Sim(Sim)->Neurons.at(0).get());
    ASSERT_NE(N, nullptr);
    EXPECT_EQ(N->build_data.SomaCompartmentIDs, (std::vector<int>{0}));
    EXPECT_NEAR(N->build_data.MembraneResistance_MOhm, 100.0f, 1e-4);
    EXPECT_NEAR(N->build_data.RefractoryPeriod_ms, 2.0f, 1e-5);
    EXPECT_NEAR(N->build_data.SlowAfterHyperpolarizationDecay_ms, 50.0f, 1e-4);
    EXPECT_NEAR(N->build_data.AdaptiveThresholdFloor_mV, -50.0f, 1e-5);
    EXPECT_EQ(N->build_data.Name, "lifcn");
    EXPECT_EQ(Do(Create, LIFCNeuronJson({}, {1}, {3}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 1u);
}

TEST_F(ModelRouteTest, LIFCNeuronAcceptsEveryValidEnumString) {
    int Sim = SimWith(Kind::LIFC, Prep::Compartments);
    auto Create = [this](const std::string& R) { return M().LIFCNeuronCreate(R); };
    const std::map<std::string, CoreStructs::LIFCUpdateMethodEnum> Update = {
        {"ExpEulerCm", CoreStructs::EXPEULER_CM}, {"ExpEulerRm", CoreStructs::EXPEULER_RM},
        {"ForwardEuler", CoreStructs::FORWARD_EULER}, {"Classical", CoreStructs::CLASSICAL}};
    const std::map<std::string, CoreStructs::LIFCResetMethodEnum> Reset = {
        {"ToVm", CoreStructs::TOVM}, {"Onset", CoreStructs::ONSET}, {"After", CoreStructs::AFTER}};
    const std::map<std::string, CoreStructs::LIFCAHPSaturationModelEnum> Ahp = {
        {"clip", CoreStructs::AHPCLIP}, {"sigmoid", CoreStructs::AHPSIGMOID}};
    const std::map<std::string, CoreStructs::LIFCADPSaturationModelEnum> Adp = {
        {"clip", CoreStructs::ADPCLIP}, {"resource", CoreStructs::ADPRESOURCE}};
    int Expected = 0;
    auto CheckOne = [&](const char* Key, const std::string& Value) {
        json Req = LIFCNeuronJson({0}, {}, {1});
        Req[Key] = Value;
        json Resp = Do(Create, Req, Sim);
        EXPECT_EQ(Resp, json({{"StatusCode", 0}, {"NeuronID", Expected}})) << Key << "=" << Value;
        return dynamic_cast<BG::NES::Simulator::LIFCNeuron*>(H().Sim(Sim)->Neurons.at(Expected++).get());
    };
    for (auto& [Str, E] : Update) { auto* N = CheckOne("UpdateMethod", Str); ASSERT_NE(N, nullptr); EXPECT_EQ(N->build_data.UpdateMethod, E) << Str; }
    for (auto& [Str, E] : Reset) { auto* N = CheckOne("ResetMethod", Str); ASSERT_NE(N, nullptr); EXPECT_EQ(N->build_data.ResetMethod, E) << Str; }
    for (auto& [Str, E] : Ahp) { auto* N = CheckOne("AfterHyperpolarizationSaturationModel", Str); ASSERT_NE(N, nullptr); EXPECT_EQ(N->build_data.AfterHyperpolarizationSaturationModel, E) << Str; }
    for (auto& [Str, E] : Adp) { auto* N = CheckOne("AfterDepolarizationSaturationModel", Str); ASSERT_NE(N, nullptr); EXPECT_EQ(N->build_data.AfterDepolarizationSaturationModel, E) << Str; }
}

TEST_F(ModelRouteTest, LIFCNeuronRejectsEveryInvalidEnumString) {
    int Sim = SimWith(Kind::LIFC, Prep::Compartments);
    auto Create = [this](const std::string& R) { return M().LIFCNeuronCreate(R); };
    const std::vector<std::pair<const char*, std::vector<std::string>>> Bad = {
        {"UpdateMethod", {"", "expeulercm", "ExpEuler", "Euler", "ForwardEuler "}},
        {"ResetMethod", {"", "tovm", "Vm", "Never"}},
        {"AfterHyperpolarizationSaturationModel", {"", "Clip", "resource", "linear"}},   // "resource" is ADP only
        {"AfterDepolarizationSaturationModel", {"", "Clip", "sigmoid", "linear"}}};     // "sigmoid" is AHP only
    for (auto& [Key, Values] : Bad) {
        for (auto& V : Values) {
            json Req = LIFCNeuronJson({0}, {}, {1});
            Req[Key] = V;
            EXPECT_EQ(Do(Create, Req, Sim), json({{"StatusCode", 2}})) << Key << "='" << V << "'";
        }
    }
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 0u);
}

// ---- Edit -------------------------------------------------------------------------------

TEST_F(ModelRouteTest, SCEditWithEmptyIdListEditsAllNeuronsAndOnlyTheGivenFields) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto Edit = [this](const std::string& R) { return M().SCNeuronEdit(R); };
    json Req = {{"NeuronIDs", json::array()}, {"MembranePotential_mV", -40.0}, {"DecayTime_ms", 7.0}};
    EXPECT_EQ(Do(Edit, Req, Sim), json({{"StatusCode", 0}}));
    for (int I = 0; I < 2; I++) {
        auto* N = dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(Sim)->Neurons.at(I).get());
        ASSERT_NE(N, nullptr);
        EXPECT_NEAR(N->build_data.MembranePotential_mV, -40.0f, 1e-5) << I;
        EXPECT_NEAR(N->Vm_mV, -40.0f, 1e-5) << I;
        EXPECT_NEAR(N->build_data.DecayTime_ms, 7.0f, 1e-5) << I;
        EXPECT_NEAR(N->TauAHP_ms, 7.0f, 1e-5) << I;
        EXPECT_NEAR(N->build_data.SpikeThreshold_mV, -50.0f, 1e-5) << I;  // untouched
        EXPECT_NEAR(N->build_data.RestingPotential_mV, -70.0f, 1e-5) << I;
    }
}

TEST_F(ModelRouteTest, SCEditWithIdListEditsOnlyThoseNeurons) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto Edit = [this](const std::string& R) { return M().SCNeuronEdit(R); };
    json Req = {{"NeuronIDs", json::array({1})}, {"SpikeThreshold_mV", -42.0}, {"RestingPotential_mV", -72.0}, {"AfterHyperpolarizationAmplitude_mV", -9.0}};
    EXPECT_EQ(Do(Edit, Req, Sim), json({{"StatusCode", 0}}));
    auto* N0 = dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(Sim)->Neurons.at(0).get());
    auto* N1 = dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(Sim)->Neurons.at(1).get());
    ASSERT_NE(N0, nullptr);
    ASSERT_NE(N1, nullptr);
    EXPECT_NEAR(N1->VAct_mV, -42.0f, 1e-5);
    EXPECT_NEAR(N1->VRest_mV, -72.0f, 1e-5);
    EXPECT_NEAR(N1->VAHP_mV, -9.0f, 1e-5);
    EXPECT_NEAR(N0->VAct_mV, -50.0f, 1e-5);
    EXPECT_NEAR(N0->VRest_mV, -70.0f, 1e-5);
    EXPECT_NEAR(N0->VAHP_mV, -5.0f, 1e-5);
}

TEST_F(ModelRouteTest, LIFCEditWithEmptyIdListEditsAllAndWithIdListOnlyThose) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    auto Edit = [this](const std::string& R) { return M().LIFCNeuronEdit(R); };
    auto N = [&](int I) { return dynamic_cast<BG::NES::Simulator::LIFCNeuron*>(H().Sim(Sim)->Neurons.at(I).get()); };
    ASSERT_NE(N(0), nullptr);
    ASSERT_NE(N(1), nullptr);

    json One = {{"NeuronIDs", json::array({0})}, {"RefractoryPeriod_ms", 3.5}, {"MembraneResistance_MOhm", 250.0}};
    EXPECT_EQ(Do(Edit, One, Sim), json({{"StatusCode", 0}}));
    EXPECT_NEAR(N(0)->build_data.RefractoryPeriod_ms, 3.5f, 1e-5);
    EXPECT_NEAR(N(0)->build_data.MembraneResistance_MOhm, 250.0f, 1e-4);
    EXPECT_NEAR(N(0)->Rm_GOhm, 0.25f, 1e-6);  // unit conversion MOhm -> GOhm
    EXPECT_NEAR(N(1)->build_data.RefractoryPeriod_ms, 2.0f, 1e-5);
    EXPECT_NEAR(N(1)->build_data.MembraneResistance_MOhm, 100.0f, 1e-4);

    json All = {{"NeuronIDs", json::array()}, {"SpikeThreshold_mV", -44.0}, {"ResetPotential_mV", -80.0}};
    EXPECT_EQ(Do(Edit, All, Sim), json({{"StatusCode", 0}}));
    for (int I = 0; I < 2; I++) {
        EXPECT_NEAR(N(I)->build_data.SpikeThreshold_mV, -44.0f, 1e-5) << I;
        EXPECT_NEAR(N(I)->build_data.ResetPotential_mV, -80.0f, 1e-5) << I;
        EXPECT_NEAR(N(I)->VAct_mV, -44.0f, 1e-5) << I;
        EXPECT_NEAR(N(I)->VReset_mV, -80.0f, 1e-5) << I;
    }
}

TEST_F(ModelRouteTest, EditWithOutOfRangeNeuronIdIsStatus2AndEditsNothing) {
    int SimSC = SimWith(Kind::SC, Prep::Neurons);  // neuron IDs 0 and 1
    int SimLIFC = SimWith(Kind::LIFC, Prep::Neurons);
    for (int Bad : {2, 100, -1}) {
        // A valid ID in the same list must not be edited either (IDs are all checked first).
        json SC = {{"NeuronIDs", json::array({0, Bad})}, {"MembranePotential_mV", -10.0}};
        EXPECT_EQ(Do([this](const std::string& R) { return M().SCNeuronEdit(R); }, SC, SimSC), json({{"StatusCode", 2}})) << Bad;
        json LC = {{"NeuronIDs", json::array({0, Bad})}, {"SpikeThreshold_mV", -10.0}};
        EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCNeuronEdit(R); }, LC, SimLIFC), json({{"StatusCode", 2}})) << Bad;
    }
    EXPECT_NEAR(dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(SimSC)->Neurons.at(0).get())->build_data.MembranePotential_mV, -65.0f, 1e-5);
    EXPECT_NEAR(dynamic_cast<BG::NES::Simulator::LIFCNeuron*>(H().Sim(SimLIFC)->Neurons.at(0).get())->build_data.SpikeThreshold_mV, -50.0f, 1e-5);
}

TEST_F(ModelRouteTest, EditOnSimulationWithoutNeuronsAcceptsOnlyAnEmptyList) {
    int Sim = H().NewSim();
    auto Edit = [this](const std::string& R) { return M().SCNeuronEdit(R); };
    EXPECT_EQ(Do(Edit, {{"NeuronIDs", json::array()}, {"MembranePotential_mV", -1.0}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(Do(Edit, {{"NeuronIDs", json::array({0})}, {"MembranePotential_mV", -1.0}}, Sim), json({{"StatusCode", 2}}));
}

// ---- Staples and receptors --------------------------------------------------------------

TEST_F(ModelRouteTest, StapleStoresItsCompartmentsAndGetsSequentialIds) {
    int Sim = H().NewSim();
    auto Create = [this](const std::string& R) { return M().StapleCreate(R); };
    json Req = {{"SourceCompartmentID", 4}, {"DestinationCompartmentID", 9}, {"Name", "stp"}};
    EXPECT_EQ(Do(Create, Req, Sim), json({{"StatusCode", 0}, {"StapleID", 0}}));
    EXPECT_EQ(Do(Create, Req, Sim), json({{"StatusCode", 0}, {"StapleID", 1}}));
    const auto& S = H().Sim(Sim)->Staples.at(1);
    EXPECT_EQ(S.ID, 1);
    EXPECT_EQ(S.SourceCompartmentID, 4);
    EXPECT_EQ(S.DestinationCompartmentID, 9);
    EXPECT_EQ(S.Name, "stp");
}

TEST_F(ModelRouteTest, ReceptorStoresItsParametersAndLinksTheNeuronPair) {
    int Sim = SimWith(Kind::BS, Prep::Neurons);
    auto Create = [this](const std::string& R) { return M().ReceptorCreate(R); };
    EXPECT_EQ(Do(Create, ReceptorJson(1, 2), Sim), json({{"StatusCode", 0}, {"ReceptorID", 0}}));
    ASSERT_EQ(H().Sim(Sim)->Receptors.size(), 1u);
    const auto& R = *H().Sim(Sim)->Receptors[0];
    EXPECT_EQ(R.SourceCompartmentID, 1);
    EXPECT_EQ(R.DestinationCompartmentID, 2);
    EXPECT_NEAR(R.Conductance_nS, 2.0f, 1e-5);
    EXPECT_NEAR(R.TimeConstantRise_ms, 1.0f, 1e-5);
    EXPECT_NEAR(R.TimeConstantDecay_ms, 3.0f, 1e-5);
    EXPECT_STREQ(R.Neurotransmitter, "AMPA");
    EXPECT_EQ(R.Name, "rec");
    EXPECT_EQ(H().Sim(Sim)->ReceptorDataVec.size(), 1u);
    // The presynaptic neuron's type follows its neurotransmitter.
    EXPECT_EQ(H().Sim(Sim)->Neurons[0]->Type_, CoreStructs::GenericPrincipalNeuron);
}

TEST_F(ModelRouteTest, ReceptorWithUnknownCompartmentIsStatus2) {
    int Sim = SimWith(Kind::BS, Prep::Neurons);  // compartments 0..3 belong to neurons
    auto Create = [this](const std::string& R) { return M().ReceptorCreate(R); };
    EXPECT_EQ(Do(Create, ReceptorJson(9, 2), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(Create, ReceptorJson(1, 9), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(Do(Create, ReceptorJson(-1, 2), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->ReceptorDataVec.size(), 0u);
}

TEST_F(ModelRouteTest, LIFCReceptorStoresItsParametersAndAcceptsEveryNeurotransmitterAndStdpMethod) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    auto Create = [this](const std::string& R) { return M().LIFCReceptorCreate(R); };
    EXPECT_EQ(Do(Create, LIFCReceptorJson(1, 2), Sim), json({{"StatusCode", 0}, {"ReceptorID", 0}}));
    const auto& R = *H().Sim(Sim)->LIFCReceptors.at(0);
    EXPECT_EQ(R.SourceCompartmentID, 1);
    EXPECT_EQ(R.DestinationCompartmentID, 2);
    EXPECT_NEAR(R.PeakConductance_nS, 2.0f, 1e-5);
    EXPECT_NEAR(R.Weight, 0.5f, 1e-5);
    EXPECT_NEAR(R.OnsetDelay_ms, 1.5f, 1e-5);
    EXPECT_NEAR(R.PSPDecay_ms, 3.0f, 1e-5);
    EXPECT_EQ(R.Neurotransmitter, Connections::AMPA);
    EXPECT_EQ(R.STDP_Method, Connections::STDPNONE);
    EXPECT_FALSE(R.voltage_gated);

    // Each neurotransmitter / STDP method on its own (a fresh simulation each, so that the
    // receptor pair is new and no abstracted receptors are merged).
    const std::map<std::string, Connections::NeurotransmitterType> NT = {
        {"AMPA", Connections::AMPA}, {"GABA", Connections::GABA}, {"NMDA", Connections::NMDA}};
    for (auto& [Str, E] : NT) {
        int S = SimWith(Kind::LIFC, Prep::Neurons);
        json Req = LIFCReceptorJson(1, 2);
        Req["Neurotransmitter"] = Str;
        EXPECT_EQ(Do(Create, Req, S)["StatusCode"], 0) << Str;
        EXPECT_EQ(H().Sim(S)->LIFCReceptors.at(0)->Neurotransmitter, E) << Str;
    }
    const std::map<std::string, Connections::LIFCSTDPMethodEnum> STDP = {
        {"None", Connections::STDPNONE}, {"Hebbian", Connections::STDPHEBBIAN}, {"Anti-Hebbian", Connections::STDPANTIHEBBIAN}};
    for (auto& [Str, E] : STDP) {
        int S = SimWith(Kind::LIFC, Prep::Neurons);
        json Req = LIFCReceptorJson(1, 2);
        Req["STDP_Method"] = Str;
        EXPECT_EQ(Do(Create, Req, S)["StatusCode"], 0) << Str;
        EXPECT_EQ(H().Sim(S)->LIFCReceptors.at(0)->STDP_Method, E) << Str;
    }
}

TEST_F(ModelRouteTest, LIFCAndNetmorphReceptorsRejectInvalidEnumStringsWithoutLeavingAReceptor) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    const std::vector<std::pair<const char*, std::vector<std::string>>> Bad = {
        {"Neurotransmitter", {"", "ampa", "Glutamate", "AMPA "}},
        {"STDP_Method", {"", "hebbian", "Anti Hebbian", "STDP"}}};
    for (auto& [Key, Values] : Bad) {
        for (auto& V : Values) {
            json L = LIFCReceptorJson(1, 2);
            L[Key] = V;
            EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCReceptorCreate(R); }, L, Sim), json({{"StatusCode", 2}})) << Key << "='" << V << "'";
            json N = NetmorphReceptorJson(1, 2);
            N[Key] = V;
            EXPECT_EQ(Do([this](const std::string& R) { return M().NetmorphLIFCReceptorCreate(R); }, N, Sim), json({{"StatusCode", 2}})) << Key << "='" << V << "'";
        }
    }
    EXPECT_EQ(H().Sim(Sim)->LIFCReceptors.size(), 0u);
}

TEST_F(ModelRouteTest, NetmorphReceptorDerivesPeakConductanceFromQuantityAndKeepsTheRestOfTheLifcFields) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    auto Create = [this](const std::string& R) { return M().NetmorphLIFCReceptorCreate(R); };
    EXPECT_EQ(Do(Create, NetmorphReceptorJson(1, 2), Sim), json({{"StatusCode", 0}, {"ReceptorID", 0}}));
    const auto& R = *H().Sim(Sim)->LIFCReceptors.at(0);
    EXPECT_NEAR(R.PeakConductance_nS, 4 * 0.5f, 1e-5);  // ReceptorQuantity * ReceptorPeakConductance_nS
    EXPECT_EQ(R.Neurotransmitter, Connections::GABA);
    EXPECT_EQ(R.STDP_Method, Connections::STDPHEBBIAN);
    EXPECT_NEAR(R.Weight, 0.5f, 1e-5);
    EXPECT_NEAR(R.PSPRise_ms, 1.0f, 1e-5);
    EXPECT_EQ(H().Sim(Sim)->Neurons[0]->Type_, CoreStructs::GenericInterneuron);
}

TEST_F(ModelRouteTest, ReceptorsWithUnknownCompartmentAreStatus2ForLIFCAndNetmorph) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    for (auto Pair : {std::make_pair(9, 2), std::make_pair(1, 9), std::make_pair(-1, 2)}) {
        EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCReceptorCreate(R); }, LIFCReceptorJson(Pair.first, Pair.second), Sim), json({{"StatusCode", 2}}));
        EXPECT_EQ(Do([this](const std::string& R) { return M().NetmorphLIFCReceptorCreate(R); }, NetmorphReceptorJson(Pair.first, Pair.second), Sim), json({{"StatusCode", 2}}));
    }
    EXPECT_EQ(H().Sim(Sim)->LIFCReceptorDataVec.size(), 0u);
}

// ---- Patch clamp ------------------------------------------------------------------------

TEST_F(ModelRouteTest, DacAndAdcCreateStoreTheirFields) {
    int Sim = H().NewSim();
    json Dac = {{"DestinationCompartmentID", 3}, {"ClampPosX_um", 1.0}, {"ClampPosY_um", 2.0}, {"ClampPosZ_um", 3.0}, {"Name", "dac"}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Dac, Sim), json({{"StatusCode", 0}, {"PatchClampDACID", 0}}));
    const auto& D = H().Sim(Sim)->PatchClampDACs.at(0);
    EXPECT_EQ(D.ID, 0);
    EXPECT_EQ(D.DestinationCompartmentID, 3);
    EXPECT_NEAR(D.ClampPos_um.y, 2.0f, 1e-6);
    EXPECT_EQ(D.Name, "dac");
    EXPECT_TRUE(D.ControlData.empty());

    json Adc = {{"SourceCompartmentID", 5}, {"ClampPosX_um", 4.0}, {"ClampPosY_um", 5.0}, {"ClampPosZ_um", 6.0}, {"Name", "adc"}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Adc, Sim), json({{"StatusCode", 0}, {"PatchClampADCID", 0}}));
    const auto& A = H().Sim(Sim)->PatchClampADCs.at(0);
    EXPECT_EQ(A.ID, 0);
    EXPECT_EQ(A.SourceCompartmentID, 5);
    EXPECT_NEAR(A.ClampPos_um.z, 6.0f, 1e-6);
    EXPECT_NEAR(A.Timestep_ms, 0.0f, 1e-9);
    EXPECT_TRUE(A.RecordedData_mV.empty());
    // Separate ID series per tool kind.
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Adc, Sim)["PatchClampADCID"], 1);
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Dac, Sim)["PatchClampDACID"], 1);
}

TEST_F(ModelRouteTest, DacSetOutputListStoresReplacesAndClearsControlData) {
    int Sim = H().NewSim();
    json Dac = {{"DestinationCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "dac"}};
    Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Dac, Sim);
    Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Dac, Sim);
    auto Set = [this](const std::string& R) { return M().PatchClampDACSetOutputList(R); };

    json Three = {{"PatchClampDACID", 1}, {"ControlData", json::array({json::array({0.0, -70.0}), json::array({2.5, -20.0}), json::array({5, 10})})}};
    EXPECT_EQ(Do(Set, Three, Sim), json({{"StatusCode", 0}}));
    const auto& D = H().Sim(Sim)->PatchClampDACs.at(1);
    ASSERT_EQ(D.ControlData.size(), 3u);
    EXPECT_NEAR(D.ControlData[1].AtTime_ms, 2.5f, 1e-6);
    EXPECT_NEAR(D.ControlData[1].DACVoltage_mV, -20.0f, 1e-5);
    EXPECT_NEAR(D.ControlData[2].DACVoltage_mV, 10.0f, 1e-5);
    EXPECT_TRUE(H().Sim(Sim)->PatchClampDACs.at(0).ControlData.empty());  // other DAC untouched

    json One = {{"PatchClampDACID", 1}, {"ControlData", json::array({json::array({1.0, 1.0, 99.0})})}};  // extra elements ignored
    EXPECT_EQ(Do(Set, One, Sim), json({{"StatusCode", 0}}));
    ASSERT_EQ(D.ControlData.size(), 1u);
    EXPECT_NEAR(D.ControlData[0].DACVoltage_mV, 1.0f, 1e-6);

    json Empty = {{"PatchClampDACID", 1}, {"ControlData", json::array()}};
    EXPECT_EQ(Do(Set, Empty, Sim), json({{"StatusCode", 0}}));
    EXPECT_TRUE(D.ControlData.empty());
}

TEST_F(ModelRouteTest, DacSetOutputListRejectsUnknownDacAndBadPairs) {
    int Sim = H().NewSim();
    json Dac = {{"DestinationCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "dac"}};
    Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Dac, Sim);
    auto Set = [this](const std::string& R) { return M().PatchClampDACSetOutputList(R); };
    json Pairs = json::array({json::array({0.0, 1.0})});
    for (int Bad : {1, 7, -1}) {
        EXPECT_EQ(Do(Set, {{"PatchClampDACID", Bad}, {"ControlData", Pairs}}, Sim), json({{"StatusCode", 2}})) << Bad;
    }
    for (json BadPair : {json::array({1.0}), json::array({"a", 1.0}), json::array({1.0, "b"}), json::array({nullptr, 1.0}), json::array()}) {
        EXPECT_EQ(Do(Set, {{"PatchClampDACID", 0}, {"ControlData", json::array({BadPair})}}, Sim), json({{"StatusCode", 2}})) << BadPair.dump();
    }
}

TEST_F(ModelRouteTest, AdcSetSampleRateStoresTimestepAndClearsRecordedData) {
    int Sim = H().NewSim();
    json Adc = {{"SourceCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "adc"}};
    Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Adc, Sim);
    Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Adc, Sim);
    auto& A1 = H().Sim(Sim)->PatchClampADCs.at(1);
    A1.RecordedData_mV = {1.0f, 2.0f, 3.0f};
    H().Sim(Sim)->PatchClampADCs.at(0).RecordedData_mV = {9.0f};

    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampADCSetSampleRate(R); }, {{"PatchClampADCID", 1}, {"Timestep_ms", 0.25}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_NEAR(A1.Timestep_ms, 0.25f, 1e-7);
    EXPECT_TRUE(A1.RecordedData_mV.empty());                                      // invalidated by the new rate
    EXPECT_EQ(H().Sim(Sim)->PatchClampADCs.at(0).RecordedData_mV.size(), 1u);     // other ADC untouched

    for (int Bad : {2, 50, -1}) {
        EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampADCSetSampleRate(R); }, {{"PatchClampADCID", Bad}, {"Timestep_ms", 1.0}}, Sim), json({{"StatusCode", 2}})) << Bad;
    }
}

TEST_F(ModelRouteTest, AdcGetRecordedDataReturnsTimestepAndSamples) {
    int Sim = H().NewSim();
    json Adc = {{"SourceCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "adc"}};
    Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Adc, Sim);
    auto Get = [this](const std::string& R) { return M().PatchClampADCGetRecordedData(R); };

    json Empty = Do(Get, {{"PatchClampADCID", 0}}, Sim);
    EXPECT_EQ(Empty["StatusCode"], 0);
    EXPECT_NEAR(Empty["Timestep_ms"].get<float>(), 0.0f, 1e-9);
    EXPECT_TRUE(Empty["RecordedData_mV"].is_array());
    EXPECT_TRUE(Empty["RecordedData_mV"].empty());

    Do([this](const std::string& R) { return M().PatchClampADCSetSampleRate(R); }, {{"PatchClampADCID", 0}, {"Timestep_ms", 0.5}}, Sim);
    H().Sim(Sim)->PatchClampADCs.at(0).RecordedData_mV = {-70.0f, -69.5f, -60.25f};
    json Data = Do(Get, {{"PatchClampADCID", 0}}, Sim);
    EXPECT_NEAR(Data["Timestep_ms"].get<float>(), 0.5f, 1e-7);
    ASSERT_EQ(Data["RecordedData_mV"].size(), 3u);
    EXPECT_NEAR(Data["RecordedData_mV"][0].get<float>(), -70.0f, 1e-5);
    EXPECT_NEAR(Data["RecordedData_mV"][2].get<float>(), -60.25f, 1e-5);

    for (int Bad : {1, 50, -1}) {
        EXPECT_EQ(Do(Get, {{"PatchClampADCID", Bad}}, Sim), json({{"StatusCode", 2}})) << Bad;
    }
}

// ---- Spike times, spontaneous activity, opto --------------------------------------------

TEST_F(ModelRouteTest, SetSpecificAPTimesAddsDirectStimToTheNamedNeurons) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto Set = [this](const std::string& R) { return M().SetSpecificAPTimes(R); };
    json Req = {{"TimeNeuronPairs", json::array({json::array({1.5, 0}), json::array({2.5, 1}), json::array({3.5, 0, "ignored"})})}};
    EXPECT_EQ(Do(Set, Req, Sim), json({{"StatusCode", 0}}));
    auto& N = H().Sim(Sim)->Neurons;
    ASSERT_EQ(N[0]->TDirectStim_ms.size(), 2u);
    EXPECT_NEAR(N[0]->TDirectStim_ms[0], 1.5f, 1e-6);
    EXPECT_NEAR(N[0]->TDirectStim_ms[1], 3.5f, 1e-6);
    ASSERT_EQ(N[1]->TDirectStim_ms.size(), 1u);
    EXPECT_NEAR(N[1]->TDirectStim_ms[0], 2.5f, 1e-6);

    EXPECT_EQ(Do(Set, {{"TimeNeuronPairs", json::array()}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[0]->TDirectStim_ms.size(), 2u);
}

TEST_F(ModelRouteTest, SetSpecificAPTimesRejectsUnknownNeuronAndMalformedPairs) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);  // neurons 0 and 1
    auto Set = [this](const std::string& R) { return M().SetSpecificAPTimes(R); };
    for (json Bad : {json::array({1.0, 2}), json::array({1.0}), json::array({"1", 0}), json::array({1.0, "0"}), json::array({1.0, 2}), json::array({1.0, -1})}) {
        // Each is a single bad pair, the first in the list, so nothing is applied before the error.
        EXPECT_EQ(Do(Set, {{"TimeNeuronPairs", json::array({Bad})}}, Sim), json({{"StatusCode", 2}})) << Bad.dump();
    }
    EXPECT_EQ(H().Sim(Sim)->Neurons[0]->TDirectStim_ms.size(), 0u);
    EXPECT_EQ(H().Sim(Sim)->Neurons[1]->TDirectStim_ms.size(), 0u);
}

TEST_F(ModelRouteTest, SetSpontaneousActivitySetsTheDistributionOnTheNamedOrAllNeurons) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto Set = [this](const std::string& R) { return M().SetSpontaneousActivity(R); };
    auto N = [&](int I) { return dynamic_cast<BG::NES::Simulator::BallAndStick::BSNeuron*>(H().Sim(Sim)->Neurons.at(I).get()); };
    ASSERT_NE(N(0), nullptr);
    EXPECT_EQ(N(1)->DtSpontDist, nullptr);

    EXPECT_EQ(Do(Set, {{"SpikeIntervalMean_ms", 10.0}, {"SpikeIntervalStDev_ms", 2.0}, {"NeuronIDs", json::array({1})}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_NEAR(N(1)->TauSpont_ms.mean, 10.0f, 1e-6);
    EXPECT_NEAR(N(1)->TauSpont_ms.stdev, 2.0f, 1e-6);
    EXPECT_NE(N(1)->DtSpontDist, nullptr);
    EXPECT_EQ(N(0)->DtSpontDist, nullptr);

    EXPECT_EQ(Do(Set, {{"SpikeIntervalMean_ms", 20.0}, {"SpikeIntervalStDev_ms", 0.0}, {"NeuronIDs", json::array()}}, Sim), json({{"StatusCode", 0}}));
    for (int I = 0; I < 2; I++) {
        EXPECT_NEAR(N(I)->TauSpont_ms.mean, 20.0f, 1e-6) << I;
        EXPECT_NEAR(N(I)->TauSpont_ms.stdev, 0.0f, 1e-6) << I;
    }
}

TEST_F(ModelRouteTest, SetSpontaneousActivityRejectsNonPositiveMeanAndNegativeStdev) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto Set = [this](const std::string& R) { return M().SetSpontaneousActivity(R); };
    for (auto MS : {std::make_pair(0.0, 1.0), std::make_pair(-5.0, 1.0), std::make_pair(10.0, -0.5)}) {
        EXPECT_EQ(Do(Set, {{"SpikeIntervalMean_ms", MS.first}, {"SpikeIntervalStDev_ms", MS.second}, {"NeuronIDs", json::array()}}, Sim), json({{"StatusCode", 2}}))
            << MS.first << "," << MS.second;
    }
    EXPECT_EQ(H().Sim(Sim)->Neurons[0]->TDirectStim_ms.size(), 0u);
    EXPECT_EQ(dynamic_cast<BG::NES::Simulator::BallAndStick::BSNeuron*>(H().Sim(Sim)->Neurons[0].get())->DtSpontDist, nullptr);
}

TEST_F(ModelRouteTest, OptoModifyInjectionTagsNeuronsOfTheTypeWithinTheRadius) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);  // neuron 0 soma centre (0,0,0), neuron 1 at (100,0,0)
    auto& N = H().Sim(Sim)->Neurons;
    N[0]->Type_ = CoreStructs::GenericPrincipalNeuron;
    N[1]->Type_ = CoreStructs::GenericPrincipalNeuron;
    auto Opto = [this](const std::string& R) { return M().OptoModifyInjection(R); };
    auto Req = [](float X, float Radius, int Wavelength, int Type) {
        return json({{"InjectLocationX_um", X}, {"InjectLocationY_um", 0.0}, {"InjectLocationZ_um", 0.0},
                     {"Radius", Radius}, {"Wavelength", Wavelength}, {"Type", Type}});
    };
    EXPECT_EQ(Do(Opto, Req(0.0f, 10.0f, 470, 1), Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[0]->opto_wavelength_nm, 470);
    EXPECT_EQ(N[1]->opto_wavelength_nm, 0);

    // A different neuron type is not touched even when inside the radius.
    EXPECT_EQ(Do(Opto, Req(100.0f, 10.0f, 590, 2), Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[1]->opto_wavelength_nm, 0);

    // The radius is inclusive and a big radius covers both.
    EXPECT_EQ(Do(Opto, Req(50.0f, 50.0f, 620, 1), Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[0]->opto_wavelength_nm, 620);
    EXPECT_EQ(N[1]->opto_wavelength_nm, 620);
}

TEST_F(ModelRouteTest, OptoActivationAddsEveryLaserTimeToNeuronsWithTheTargetWavelength) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    auto& N = H().Sim(Sim)->Neurons;
    N[0]->opto_wavelength_nm = 470;
    N[1]->opto_wavelength_nm = 590;
    auto Act = [this](const std::string& R) { return M().OptoActivation(R); };
    EXPECT_EQ(Do(Act, {{"LaserActivationTimes", json::array({1.0, 2.5, 4})}, {"TargetWavelength", 470}}, Sim), json({{"StatusCode", 0}}));
    ASSERT_EQ(N[0]->TDirectStim_ms.size(), 3u);
    EXPECT_NEAR(N[0]->TDirectStim_ms[1], 2.5f, 1e-6);
    EXPECT_NEAR(N[0]->TDirectStim_ms[2], 4.0f, 1e-6);
    EXPECT_TRUE(N[1]->TDirectStim_ms.empty());

    EXPECT_EQ(Do(Act, {{"LaserActivationTimes", json::array()}, {"TargetWavelength", 470}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[0]->TDirectStim_ms.size(), 3u);
    EXPECT_EQ(Do(Act, {{"LaserActivationTimes", json::array({1.0})}, {"TargetWavelength", 999}}, Sim), json({{"StatusCode", 0}}));
    EXPECT_EQ(N[0]->TDirectStim_ms.size(), 3u);
    EXPECT_TRUE(N[1]->TDirectStim_ms.empty());
}

// ---- Known bugs (dev_frame D8): each asserts the CORRECT behaviour and fails today ------

class ModelRouteKnownBug : public ModelRouteTest {
protected:
    // Calls Fn and returns its parsed response, or a json null when the handler throws.
    json DoNoThrow(const std::function<std::string(const std::string&)>& Fn, json Req, int Sim, bool* Threw) {
        *Threw = false;
        try {
            return Parse(Fn(With(std::move(Req), Sim).dump()));
        } catch (const std::exception&) {
            *Threw = true;
            return nullptr;
        }
    }
    static json PatchClampDac() {
        return {{"DestinationCompartmentID", 0}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "dac"}};
    }
};

// Bug: dev_frame #132
TEST_F(ModelRouteKnownBug, PatchClampDacCreateIsJournaledUnderItsRegisteredRoute) {
    int Sim = H().NewSim();
    ASSERT_EQ(Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, PatchClampDac(), Sim)["StatusCode"], 0);
    json Journal = Parse(H().Sim(Sim)->StoredRequestsToNESRequestArray());
    ASSERT_TRUE(Journal.is_array());
    ASSERT_EQ(Journal.size(), 1u);
    EXPECT_TRUE(Journal[0].contains("Simulation/PatchClampDAC/Create")) << Journal[0].dump();
}

// Bug: dev_frame #133
TEST_F(ModelRouteKnownBug, StapleRejectsCompartmentsThatDoNotExist) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);  // compartments 0..3
    json Req = {{"SourceCompartmentID", 98}, {"DestinationCompartmentID", 99}, {"Name", "ghost"}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().StapleCreate(R); }, Req, Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Staples.size(), 0u);
}

// Bug: dev_frame #133
TEST_F(ModelRouteKnownBug, PatchClampDacCreateRejectsACompartmentThatDoesNotExist) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);
    json Req = PatchClampDac();
    Req["DestinationCompartmentID"] = 99;
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, Req, Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->PatchClampDACs.size(), 0u);
}

// Bug: dev_frame #133
TEST_F(ModelRouteKnownBug, PatchClampAdcCreateRejectsACompartmentThatDoesNotExist) {
    int Sim = SimWith(Kind::BS, Prep::Compartments);
    json Req = {{"SourceCompartmentID", 99}, {"ClampPosX_um", 0.0}, {"ClampPosY_um", 0.0}, {"ClampPosZ_um", 0.0}, {"Name", "adc"}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().PatchClampADCCreate(R); }, Req, Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->PatchClampADCs.size(), 0u);
}

// Bug: dev_frame #133
TEST_F(ModelRouteKnownBug, SCNeuronCreateRejectsCompartmentsThatDoNotExist) {
    int Sim = SimWith(Kind::SC, Prep::Compartments);  // compartments 0..3
    EXPECT_EQ(Do([this](const std::string& R) { return M().SCNeuronCreate(R); }, SCNeuronJson({50}, {51}, {52}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 0u);
}

// Bug: dev_frame #133
TEST_F(ModelRouteKnownBug, LIFCNeuronCreateRejectsCompartmentsThatDoNotExist) {
    int Sim = SimWith(Kind::LIFC, Prep::Compartments);
    EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCNeuronCreate(R); }, LIFCNeuronJson({50}, {51}, {52}), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Neurons.size(), 0u);
}

// Bug: dev_frame #134
TEST_F(ModelRouteKnownBug, SetSpontaneousActivityRejectsUnknownNeuronIdsInsteadOfThrowing) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);  // neurons 0 and 1
    for (int Bad : {2, 99, -1}) {
        bool Threw = false;
        json Resp = DoNoThrow([this](const std::string& R) { return M().SetSpontaneousActivity(R); },
                              {{"SpikeIntervalMean_ms", 10.0}, {"SpikeIntervalStDev_ms", 1.0}, {"NeuronIDs", json::array({Bad})}}, Sim, &Threw);
        EXPECT_FALSE(Threw) << "NeuronIDs [" << Bad << "] threw";
        if (!Threw) EXPECT_EQ(Resp, json({{"StatusCode", 2}})) << Bad;
    }
}

// Bug: dev_frame #135
TEST_F(ModelRouteKnownBug, SetOutputListRejectsAnObjectElementInsteadOfThrowing) {
    int Sim = H().NewSim();
    Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, PatchClampDac(), Sim);
    bool Threw = false;
    json Resp = DoNoThrow([this](const std::string& R) { return M().PatchClampDACSetOutputList(R); },
                          {{"PatchClampDACID", 0}, {"ControlData", json::array({json({{"t", 1.0}, {"v", 2.0}})})}}, Sim, &Threw);
    EXPECT_FALSE(Threw);
    if (!Threw) EXPECT_EQ(Resp, json({{"StatusCode", 2}}));
}

// Bug: dev_frame #135
TEST_F(ModelRouteKnownBug, SetSpecificAPTimesRejectsAnObjectElementInsteadOfThrowing) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    bool Threw = false;
    json Resp = DoNoThrow([this](const std::string& R) { return M().SetSpecificAPTimes(R); },
                          {{"TimeNeuronPairs", json::array({json({{"t", 1.0}, {"n", 0}})})}}, Sim, &Threw);
    EXPECT_FALSE(Threw);
    if (!Threw) EXPECT_EQ(Resp, json({{"StatusCode", 2}}));
}

// Bug: dev_frame #136
TEST_F(ModelRouteKnownBug, SCEditWithAWronglyTypedOptionalFieldChangesNothing) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    json Req = {{"NeuronIDs", json::array({0})}, {"MembranePotential_mV", "not a number"}, {"SpikeThreshold_mV", -42.0}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().SCNeuronEdit(R); }, Req, Sim), json({{"StatusCode", 2}}));
    auto* N = dynamic_cast<BG::NES::Simulator::SCNeuron*>(H().Sim(Sim)->Neurons.at(0).get());
    ASSERT_NE(N, nullptr);
    EXPECT_NEAR(N->build_data.SpikeThreshold_mV, -50.0f, 1e-5);  // the rejected request must not have edited the neuron
    EXPECT_NEAR(N->VAct_mV, -50.0f, 1e-5);
}

// Bug: dev_frame #136
TEST_F(ModelRouteKnownBug, LIFCEditWithAWronglyTypedOptionalFieldChangesNothing) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    json Req = {{"NeuronIDs", json::array({0})}, {"RefractoryPeriod_ms", "not a number"}, {"SpikeThreshold_mV", -42.0}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().LIFCNeuronEdit(R); }, Req, Sim), json({{"StatusCode", 2}}));
    auto* N = dynamic_cast<BG::NES::Simulator::LIFCNeuron*>(H().Sim(Sim)->Neurons.at(0).get());
    ASSERT_NE(N, nullptr);
    EXPECT_NEAR(N->build_data.SpikeThreshold_mV, -50.0f, 1e-5);
    EXPECT_NEAR(N->VAct_mV, -50.0f, 1e-5);
}

// Bug: dev_frame #136
TEST_F(ModelRouteKnownBug, SetOutputListWithABadPairKeepsThePreviousControlData) {
    int Sim = H().NewSim();
    Do([this](const std::string& R) { return M().PatchClampDACCreate(R); }, PatchClampDac(), Sim);
    auto Set = [this](const std::string& R) { return M().PatchClampDACSetOutputList(R); };
    ASSERT_EQ(Do(Set, {{"PatchClampDACID", 0}, {"ControlData", json::array({json::array({1.0, 5.0}), json::array({2.0, 6.0})})}}, Sim)["StatusCode"], 0);
    EXPECT_EQ(Do(Set, {{"PatchClampDACID", 0}, {"ControlData", json::array({json::array({9.0, 9.0}), json::array({"x", 1.0})})}}, Sim), json({{"StatusCode", 2}}));
    const auto& D = H().Sim(Sim)->PatchClampDACs.at(0);
    ASSERT_EQ(D.ControlData.size(), 2u);
    EXPECT_NEAR(D.ControlData[0].AtTime_ms, 1.0f, 1e-6);
    EXPECT_NEAR(D.ControlData[1].DACVoltage_mV, 6.0f, 1e-6);
}

// Bug: dev_frame #136
TEST_F(ModelRouteKnownBug, SetSpecificAPTimesWithABadPairAppliesNoneOfThem) {
    int Sim = SimWith(Kind::SC, Prep::Neurons);
    json Req = {{"TimeNeuronPairs", json::array({json::array({1.5, 0}), json::array({2.5, 99})})}};
    EXPECT_EQ(Do([this](const std::string& R) { return M().SetSpecificAPTimes(R); }, Req, Sim), json({{"StatusCode", 2}}));
    EXPECT_TRUE(H().Sim(Sim)->Neurons[0]->TDirectStim_ms.empty());
}

// Bug: dev_frame #137
TEST_F(ModelRouteKnownBug, RejectedRequestsAreNotJournaled) {
    int Sim = SimWith(Kind::None, Prep::Shapes);
    ASSERT_EQ(H().Sim(Sim)->NumStoredRequests(), 0u);  // shapes were made by direct calls below: reset
    H().Sim(Sim)->ClearStoredRequests();
    EXPECT_EQ(Do(CompFn(Kind::BS), BSCompJson(99), Sim), json({{"StatusCode", 2}}));  // unknown shape
    H().Sim(Sim)->IsProcessing = true;
    EXPECT_EQ(Do(CompFn(Kind::BS), BSCompJson(0), Sim), json({{"StatusCode", 5}}));   // busy
    H().Sim(Sim)->IsProcessing = false;
    EXPECT_EQ(H().Sim(Sim)->NumStoredRequests(), 0u);
}

// Bug: dev_frame #116
TEST_F(ModelRouteKnownBug, RejectedCompartmentDoesNotFixTheNeuronClass) {
    int Sim = SimWith(Kind::None, Prep::Shapes);
    EXPECT_EQ(Do(CompFn(Kind::BS), BSCompJson(99), Sim), json({{"StatusCode", 2}}));  // rejected: unknown shape
    EXPECT_EQ(H().Sim(Sim)->SimNeuronClass, BG::NES::Simulator::UNDETERMINED);
    EXPECT_EQ(Do(CompFn(Kind::LIFC), LIFCCompJson(0), Sim), json({{"StatusCode", 0}, {"CompartmentID", 0}}));
}

// Bug: dev_frame #106
TEST_F(ModelRouteKnownBug, RejectedReceptorLeavesNoOrphanAndTheNextGetsTheNextId) {
    int Sim = SimWith(Kind::BS, Prep::Neurons);
    auto Create = [this](const std::string& R) { return M().ReceptorCreate(R); };
    EXPECT_EQ(Do(Create, ReceptorJson(99, 2), Sim), json({{"StatusCode", 2}}));
    EXPECT_EQ(H().Sim(Sim)->Receptors.size(), 0u);
    EXPECT_EQ(Do(Create, ReceptorJson(1, 2), Sim), json({{"StatusCode", 0}, {"ReceptorID", 0}}));
}

// Bug: dev_frame #107
TEST_F(ModelRouteKnownBug, NetmorphReceptorOnsetDelayAddsConductionTimeInMilliseconds) {
    int Sim = SimWith(Kind::LIFC, Prep::Neurons);
    json Req = NetmorphReceptorJson(1, 2);  // 0.5 ms synaptic delay + 100 um at 1 m/s (= 0.1 ms)
    ASSERT_EQ(Do([this](const std::string& R) { return M().NetmorphLIFCReceptorCreate(R); }, Req, Sim)["StatusCode"], 0);
    EXPECT_NEAR(H().Sim(Sim)->LIFCReceptors.at(0)->OnsetDelay_ms, 0.6f, 1e-4);
}

}  // namespace
