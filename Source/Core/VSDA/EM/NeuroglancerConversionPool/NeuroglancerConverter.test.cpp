//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the Neuroglancer helpers and the info-file part of
    ExecuteConversionOperation (dev_frame NT-13). No conversion pool or igneous process is used.
*/

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <TestUtil/TestUtil.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Util/StoragePaths.h>
#include <VSDA/EM/NeuroglancerConversionPool/NeuroglancerConverter.h>

// Defined in NeuroglancerConverter.cpp but not declared in its header.
namespace BG { namespace NES { namespace Simulator { namespace VSDA {
std::string urlEncode(const std::string& value);
std::string NGurlEncode(const std::string& value);
std::string GenerateNeuroglancerJson(const std::string& imageDatasetUrl, const std::string& segmentationDatasetUrl, bool enableSegmentation);
}}}}

using namespace BG::NES::Simulator;
using namespace BG::NES::Simulator::VSDA;
using nlohmann::json;

namespace {
std::string Decode(const std::string& S) {
    std::string Out;
    for (size_t i = 0; i < S.size(); i++) {
        if (S[i] == '%') { Out += char(std::stoi(S.substr(i + 1, 2), nullptr, 16)); i += 2; }
        else Out += S[i];
    }
    return Out;
}
}

TEST(NGurlEncode, KeepsUnreservedCharacters) {
    EXPECT_EQ(NGurlEncode("abcXYZ019-_.~"), "abcXYZ019-_.~");
    EXPECT_EQ(NGurlEncode(""), "");
}

TEST(NGurlEncode, EncodesReservedCharactersAsUppercasePercentHex) {
    EXPECT_EQ(NGurlEncode(" "), "%20");
    EXPECT_EQ(NGurlEncode("/"), "%2F");
    EXPECT_EQ(NGurlEncode(":"), "%3A");
    EXPECT_EQ(NGurlEncode("%"), "%25");
    EXPECT_EQ(NGurlEncode("\n"), "%0A");
    EXPECT_EQ(NGurlEncode("{\"a\":[1,2]}"), "%7B%22a%22%3A%5B1%2C2%5D%7D");
}

TEST(NGurlEncode, EncodesNonAsciiBytesIndividually) {
    EXPECT_EQ(NGurlEncode("\xC3\xA9"), "%C3%A9");
    EXPECT_EQ(NGurlEncode("a\xFF" "b"), "a%FFb");
}

TEST(NGurlEncode, RoundTripsArbitraryBytes) {
    std::string All;
    for (int c = 1; c < 256; c++) All += char(c);
    EXPECT_EQ(Decode(NGurlEncode(All)), All);
}

TEST(UrlEncode, KeepsOnlyAlphanumericsAndEncodesTheRestAsLowercaseHex) {
    EXPECT_EQ(urlEncode("abcXYZ019"), "abcXYZ019");
    EXPECT_EQ(urlEncode(" /"), "%20%2f");
    EXPECT_EQ(urlEncode("-_.~"), "%2d%5f%2e%7e");
    EXPECT_EQ(urlEncode("\n"), "%0a");
    EXPECT_EQ(urlEncode(""), "");
}

TEST(UrlEncode, EncodesNonAsciiBytes) {
    EXPECT_EQ(urlEncode("\xC3\xA9"), "%c3%a9");
}

TEST(GenerateNeuroglancerJson, WithoutSegmentationHasOnlyTheImageLayer) {
    json J = json::parse(GenerateNeuroglancerJson("http://h/img", "http://h/seg", false));
    ASSERT_EQ(J["layers"].size(), 1u);
    EXPECT_EQ(J["layers"][0]["type"], "image");
    EXPECT_EQ(J["layers"][0]["source"], "precomputed://http://h/img");
    EXPECT_EQ(J["layers"][0]["name"], "Microscopy Data");
    EXPECT_FALSE(J.contains("selectedLayer"));
    EXPECT_EQ(J["layout"], "xy-3d");
}

TEST(GenerateNeuroglancerJson, WithSegmentationAddsSegmentationLayerAndSelectsIt) {
    json J = json::parse(GenerateNeuroglancerJson("http://h/img", "http://h/seg", true));
    ASSERT_EQ(J["layers"].size(), 2u);
    EXPECT_EQ(J["layers"][0]["source"], "precomputed://http://h/img");
    EXPECT_EQ(J["layers"][1]["type"], "segmentation");
    EXPECT_EQ(J["layers"][1]["source"], "precomputed://http://h/seg");
    EXPECT_EQ(J["layers"][1]["name"], "Ground Truth Segmentation");
    EXPECT_EQ(J["selectedLayer"]["layer"], "Ground Truth Segmentation");
    EXPECT_EQ(J["selectedLayer"]["visible"], true);
}

TEST(GenerateNeuroglancerJson, CarriesDimensionsPositionAndOrientation) {
    json J = json::parse(GenerateNeuroglancerJson("i", "s", false));
    EXPECT_EQ(J["dimensions"]["x"], json::parse("[100,\"nm\"]"));
    EXPECT_EQ(J["dimensions"]["y"], json::parse("[100,\"nm\"]"));
    EXPECT_EQ(J["dimensions"]["z"], json::parse("[200,\"nm\"]"));
    ASSERT_EQ(J["position"].size(), 3u);
    for (auto& P : J["position"]) EXPECT_DOUBLE_EQ(P.get<double>(), 0.0);
    ASSERT_EQ(J["projectionOrientation"].size(), 4u);
    EXPECT_NEAR(J["projectionOrientation"][3].get<double>(), 0.9358897, 1e-6);
}

TEST(GenerateNeuroglancerURL, UsesDefaultBaseAndEncodesTheJsonFragment) {
    std::string U = GenerateNeuroglancerURL("abc-123", false);
    const std::string Prefix = "http://neuroglancer-demo.appspot.com/#!";
    ASSERT_EQ(U.compare(0, Prefix.size(), Prefix), 0) << U;
    std::string Frag = U.substr(Prefix.size());
    for (char c : Frag) EXPECT_TRUE(std::isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '%') << c;
    json J = json::parse(Decode(Frag));
    ASSERT_EQ(J["layers"].size(), 1u);
    std::string Src = J["layers"][0]["source"];
    EXPECT_EQ(Src.rfind("precomputed://", 0), 0u);
    EXPECT_NE(Src.find("/Dataset/abc-123/Data"), std::string::npos);
}

TEST(GenerateNeuroglancerURL, SegmentationFlagControlsSegmentationLayer) {
    json Off = json::parse(Decode(GenerateNeuroglancerURL("h", false).substr(std::string("http://neuroglancer-demo.appspot.com/#!").size())));
    json On = json::parse(Decode(GenerateNeuroglancerURL("h", true).substr(std::string("http://neuroglancer-demo.appspot.com/#!").size())));
    EXPECT_EQ(Off["layers"].size(), 1u);
    ASSERT_EQ(On["layers"].size(), 2u);
    EXPECT_NE(std::string(On["layers"][1]["source"]).find("/Dataset/h/Segmentation"), std::string::npos);
}

TEST(GenerateNeuroglancerURL, CustomBaseUrlIsUsed) {
    std::string U = GenerateNeuroglancerURL("h", false, "https://ng.example.test");
    EXPECT_EQ(U.rfind("https://ng.example.test/#!", 0), 0u);
}

// ----------------------------------------------- ExecuteConversionOperation (info files only)

class ConversionInfoTest : public BG::NES::TestUtil::TempOutputRoot {
protected:
    std::unique_ptr<Simulation> Sim;
    ScanRegion* Region = nullptr;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Sim = std::make_unique<Simulation>(Logger_.get());
        auto& D = *Sim->VSDAData_;
        D.State_ = VSDA_CONVERSION_REQUESTED;
        D.Params_ = MicroscopeParameters{};
        D.Params_.VoxelResolution_um = 0.5f;
        D.Params_.SliceThickness_um = 1.0f;
        D.Params_.ImageWidth_px = 100;
        D.Params_.ImageHeight_px = 50;
        D.Regions_.push_back(ScanRegion{});
        D.ActiveRegionID_ = 0;
        Region = &D.Regions_[0];
        Region->RegionIndexInfo_ = VoxelIndexInfo{0, 250, 0, 120, 0, 7};
    }

    json ReadInfo(const std::string& Rel) {
        std::string Path = BG::NES::Util::Storage::Resolve(Sim->OwnerUsername, "NeuroglancerDatasets/" + Region->NeuroglancerDatasetHandle_ + "/" + Rel);
        std::ifstream F(Path);
        EXPECT_TRUE(F.good()) << Path;
        std::stringstream S; S << F.rdbuf();
        return json::parse(S.str());
    }
};

TEST_F(ConversionInfoTest, RefusesWhenNoConversionWasRequested) {
    Sim->VSDAData_->State_ = VSDA_RENDER_DONE;
    EXPECT_FALSE(ExecuteConversionOperation(Logger_.get(), Sim.get(), nullptr, 2));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_RENDER_DONE);
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
}

TEST_F(ConversionInfoTest, WritesImageInfoWithOneScalePerReductionLevel) {
    ASSERT_TRUE(ExecuteConversionOperation(Logger_.get(), Sim.get(), nullptr, 2));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_RENDER_DONE);
    EXPECT_FALSE(Region->NeuroglancerDatasetHandle_.empty());
    json Info = ReadInfo("Data/info");
    EXPECT_EQ(Info["type"], "image");
    EXPECT_EQ(Info["data_type"], "uint8");
    ASSERT_EQ(Info["scales"].size(), 3u);  // levels 0..2
    for (int L = 0; L < 3; L++) {
        auto& S = Info["scales"][L];
        int F = 1 << L;
        EXPECT_EQ(S["key"], "ReductionLevel-" + std::to_string(L));
        EXPECT_EQ(S["encoding"], "jpeg");
        EXPECT_EQ(S["resolution"], json::parse("[" + std::to_string(500 * F) + "," + std::to_string(500 * F) + ",1000]"));
        // EndX=250 rounds up to 3 images of 100 px, EndY=120 up to 3 of 50 px, divided by the reduction factor
        EXPECT_EQ(S["size"], json::parse("[" + std::to_string(300 / F) + "," + std::to_string(150 / F) + ",7]"));
        EXPECT_EQ(S["chunk_sizes"], json::parse("[[" + std::to_string(100 / F) + "," + std::to_string(50 / F) + ",1]]"));
        EXPECT_EQ(S["voxel_offset"], json::parse("[0,0,0]"));
    }
}

TEST_F(ConversionInfoTest, WritesSegmentationInfoWithSingleScale) {
    ASSERT_TRUE(ExecuteConversionOperation(Logger_.get(), Sim.get(), nullptr, 2));
    json Info = ReadInfo("Segmentation/info");
    EXPECT_EQ(Info["type"], "segmentation");
    EXPECT_EQ(Info["data_type"], "uint64");
    ASSERT_EQ(Info["scales"].size(), 1u);
    EXPECT_EQ(Info["scales"][0]["encoding"], "compressed_segmentation");
    EXPECT_EQ(Info["scales"][0]["key"], "Segmentation");
    EXPECT_EQ(Info["scales"][0]["size"], json::parse("[300,150,7]"));
}

TEST_F(ConversionInfoTest, WritesProvenanceFilesAndKeepsHandleDistinctPerRun) {
    ASSERT_TRUE(ExecuteConversionOperation(Logger_.get(), Sim.get(), nullptr, 1));
    std::string First = Region->NeuroglancerDatasetHandle_;
    EXPECT_NO_THROW(ReadInfo("Data/provenance"));
    EXPECT_NO_THROW(ReadInfo("Segmentation/provenance"));
    Sim->VSDAData_->State_ = VSDA_CONVERSION_REQUESTED;
    ASSERT_TRUE(ExecuteConversionOperation(Logger_.get(), Sim.get(), nullptr, 1));
    EXPECT_NE(First, Region->NeuroglancerDatasetHandle_);
}

// Bug: dev_frame #126
TEST(ConversionInfoKnownBug, NumChannelsIsAJsonNumber) {
    BG::NES::TestUtil::ScopedTempOutputRoot Root;
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    Simulation Sim(Logger.get());
    auto& D = *Sim.VSDAData_;
    D.State_ = VSDA_CONVERSION_REQUESTED;
    D.Params_ = MicroscopeParameters{};
    D.Params_.VoxelResolution_um = 0.5f; D.Params_.SliceThickness_um = 1.0f;
    D.Params_.ImageWidth_px = 100; D.Params_.ImageHeight_px = 50;
    D.Regions_.push_back(ScanRegion{});
    D.ActiveRegionID_ = 0;
    D.Regions_[0].RegionIndexInfo_ = VoxelIndexInfo{0, 250, 0, 120, 0, 7};
    ASSERT_TRUE(ExecuteConversionOperation(Logger.get(), &Sim, nullptr, 1));
    for (const char* Sub : {"Data/info", "Segmentation/info"}) {
        std::ifstream F(BG::NES::Util::Storage::Resolve(Sim.OwnerUsername, "NeuroglancerDatasets/" + D.Regions_[0].NeuroglancerDatasetHandle_ + "/" + Sub));
        std::stringstream S; S << F.rdbuf();
        json Info = json::parse(S.str());
        // Neuroglancer's precomputed spec: num_channels is an integer
        EXPECT_TRUE(Info["num_channels"].is_number_integer()) << Sub << ": " << Info["num_channels"];
    }
}
