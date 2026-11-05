//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
     Description: Unit tests for Simulation::GetSpikeTimesJSON, the backend of
                  Simulation/GetSpikeTimes (optional SinceTime_ms and NeuronIDs).
     Additional Notes: Kept apart from Simulation.test.cpp, which does not
                       compile against the current API on master.
*/

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>

struct SimulationSpikeTimesTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<BG::NES::Simulator::Simulation> Sim{};
    const float All = std::numeric_limits<float>::lowest();

    void SetUp() override {
        Sim = std::make_unique<BG::NES::Simulator::Simulation>(&Logger);
        Sim->Collection.AddBox();
        for (int i = 0; i < 3; i++) {
            BG::NES::Simulator::CoreStructs::SCNeuronStruct N{};
            N.Name = "test";
            N.SomaCompartmentIDs = {0};
            ASSERT_EQ(Sim->AddSCNeuron(N), i);
        }
        ASSERT_EQ(Sim->Neurons.size(), 3u);
        // Neuron 2 never spiked.
        Sim->Neurons[0]->TAct_ms = {1.0f, 5.0f, 9.0f};
        Sim->Neurons[1]->TAct_ms = {2.0f, 6.0f};
    }

    static std::vector<float> Spikes(const nlohmann::json& J, const char* id) {
        return J.at(id).at("tSpike_ms").get<std::vector<float>>();
    }
};

TEST_F(SimulationSpikeTimesTest, no_fields_returns_full_history) {
    for (const auto& J : {Sim->GetSpikeTimesJSON(), Sim->GetSpikeTimesJSON(All)}) {
        ASSERT_EQ(J.size(), 3u);
        EXPECT_EQ(Spikes(J, "0"), (std::vector<float>{1.0f, 5.0f, 9.0f}));
        EXPECT_EQ(Spikes(J, "1"), (std::vector<float>{2.0f, 6.0f}));
        EXPECT_TRUE(J.at("2").at("tSpike_ms").is_array());
        EXPECT_TRUE(J.at("2").at("tSpike_ms").empty());
    }
    // Exactly the legacy shape, byte for byte.
    EXPECT_EQ(Sim->GetSpikeTimesJSON().dump(),
              R"({"0":{"tSpike_ms":[1.0,5.0,9.0]},"1":{"tSpike_ms":[2.0,6.0]},"2":{"tSpike_ms":[]}})");
}

TEST_F(SimulationSpikeTimesTest, since_time_filters_inclusively) {
    auto J = Sim->GetSpikeTimesJSON(5.0f);
    ASSERT_EQ(J.size(), 3u); // every neuron stays present
    EXPECT_EQ(Spikes(J, "0"), (std::vector<float>{5.0f, 9.0f}));
    EXPECT_EQ(Spikes(J, "1"), (std::vector<float>{6.0f}));
    EXPECT_TRUE(Spikes(J, "2").empty());
}

TEST_F(SimulationSpikeTimesTest, since_time_past_end_keeps_shape) {
    auto J = Sim->GetSpikeTimesJSON(100.0f);
    ASSERT_EQ(J.size(), 3u);
    for (const char* id : {"0", "1", "2"}) {
        ASSERT_TRUE(J.at(id).at("tSpike_ms").is_array());
        EXPECT_TRUE(J.at(id).at("tSpike_ms").empty());
    }
}

TEST_F(SimulationSpikeTimesTest, neuron_ids_filter) {
    std::vector<int> ids{1};
    auto J = Sim->GetSpikeTimesJSON(All, &ids);
    ASSERT_EQ(J.size(), 1u);
    EXPECT_FALSE(J.contains("0"));
    EXPECT_EQ(Spikes(J, "1"), (std::vector<float>{2.0f, 6.0f}));

    ids = {1, 1}; // duplicates collapse
    EXPECT_EQ(Sim->GetSpikeTimesJSON(All, &ids).size(), 1u);
}

TEST_F(SimulationSpikeTimesTest, neuron_ids_and_since_time_combined) {
    std::vector<int> ids{0, 1};
    auto J = Sim->GetSpikeTimesJSON(5.0f, &ids);
    ASSERT_EQ(J.size(), 2u);
    EXPECT_FALSE(J.contains("2"));
    EXPECT_EQ(Spikes(J, "0"), (std::vector<float>{5.0f, 9.0f}));
    EXPECT_EQ(Spikes(J, "1"), (std::vector<float>{6.0f}));
}

TEST_F(SimulationSpikeTimesTest, empty_id_list_gives_empty_object) {
    std::vector<int> ids;
    auto J = Sim->GetSpikeTimesJSON(All, &ids);
    EXPECT_TRUE(J.is_object());
    EXPECT_TRUE(J.empty());
    EXPECT_EQ(J.dump(), "{}");
}
