//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for API::HandlerData (RPCHandlerHelper.cpp): SimulationID
                 validation, busy handling, RequestUsername handling, typed parameter
                 getters and response shapes. (dev_frame NT-10)
*/

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <RPC/APIStatusCode.h>
#include <RPC/RPCHandlerHelper.h>
#include <Simulator/Structs/RecordingElectrode.h>  // complete type for ~Simulation
#include <Simulator/Structs/Simulation.h>
#include <TestUtil/TestUtil.h>
#include <Util/StoragePaths.h>

using nlohmann::json;
using BG::NES::API::HandlerData;
using BG::NES::API::BGStatusCode;
using BG::NES::Simulator::Simulation;
using BG::NES::Simulator::Geometries::Vec3D;

namespace {

// Silent logger + temp output root (HandlerData may create user directories) + one registry.
class HandlerDataTest : public BG::NES::TestUtil::TempOutputRoot {
protected:
    BG::NES::ConcurrentUniquePtrRegistry<Simulation> Registry;

    size_t AddSim() {
        return Registry.append(std::make_unique<Simulation>(Logger_.get()));
    }

    HandlerData Make(const json& Request, bool PermitBusy = false, bool NoSimulation = false) {
        return HandlerData(Request.dump(), Logger_.get(), "TestRoute", &Registry, PermitBusy, NoSimulation);
    }
};

TEST_F(HandlerDataTest, ValidSimulationIdSucceedsAndBindsSimulation) {
    AddSim();
    size_t Second = AddSim();
    HandlerData H = Make({{"SimulationID", int(Second)}});
    EXPECT_FALSE(H.HasError());
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusSuccess);
    EXPECT_EQ(H.SimID(), 1);
    EXPECT_EQ(H.SimIDStr(), "1");
    EXPECT_EQ(H.Sim(), Registry.read(1));
    EXPECT_NE(H.Sim(), Registry.read(0));
}

TEST_F(HandlerDataTest, ResetThisSimulationClearsBinding) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    ASSERT_NE(H.Sim(), nullptr);
    H.ResetThisSimulation();
    EXPECT_EQ(H.Sim(), nullptr);
}

TEST_F(HandlerDataTest, MissingSimulationIdIsInvalidParameters) {
    AddSim();
    HandlerData H = Make({{"Other", 1}});
    EXPECT_TRUE(H.HasError());
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_EQ(H.Sim(), nullptr);
}

TEST_F(HandlerDataTest, NonNumericSimulationIdIsInvalidParameters) {
    AddSim();
    for (const json& Bad : {json("0"), json(nullptr), json(true), json::array({0}), json::object()}) {
        HandlerData H = Make({{"SimulationID", Bad}});
        EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed) << Bad.dump();
        EXPECT_EQ(H.Sim(), nullptr) << Bad.dump();
    }
}

TEST_F(HandlerDataTest, NegativeSimulationIdIsInvalidParameters) {
    AddSim();
    HandlerData H = Make({{"SimulationID", -1}});
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_EQ(H.Sim(), nullptr);
}

TEST_F(HandlerDataTest, SimulationIdAtOrBeyondSizeIsInvalidParameters) {
    AddSim();
    EXPECT_EQ(Make({{"SimulationID", 1}}).GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_EQ(Make({{"SimulationID", 1000000}}).GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_EQ(Make({{"SimulationID", 0}}).GetStatus(), BGStatusCode::BGStatusSuccess);
}

TEST_F(HandlerDataTest, EmptyRegistryRejectsEveryId) {
    EXPECT_EQ(Make({{"SimulationID", 0}}).GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataTest, DeletedSimulationIsInvalidParameters) {
    AddSim();
    AddSim();
    Registry.remove(0);
    HandlerData Deleted = Make({{"SimulationID", 0}});
    EXPECT_EQ(Deleted.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_EQ(Deleted.Sim(), nullptr);
    // The other slot keeps its index and still works.
    EXPECT_EQ(Make({{"SimulationID", 1}}).GetStatus(), BGStatusCode::BGStatusSuccess);
}

TEST_F(HandlerDataTest, SimulationBeingDeletedIsInvalidParameters) {
    AddSim();
    Registry.read(0)->Deleting = true;
    EXPECT_EQ(Make({{"SimulationID", 0}}).GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    // PermitBusy does not override Deleting.
    EXPECT_EQ(Make({{"SimulationID", 0}}, true).GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataTest, BusySimulationIsRejectedWithStatus5) {
    AddSim();
    Registry.read(0)->IsProcessing = true;
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_TRUE(H.HasError());
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusSimulationBusy);
    EXPECT_EQ(int(H.GetStatus()), 5);
    EXPECT_EQ(json::parse(H.ErrResponse()), json({{"StatusCode", 5}}));
}

TEST_F(HandlerDataTest, WorkRequestedCountsAsBusy) {
    AddSim();
    Registry.read(0)->WorkRequested = true;
    EXPECT_EQ(Make({{"SimulationID", 0}}).GetStatus(), BGStatusCode::BGStatusSimulationBusy);
}

TEST_F(HandlerDataTest, PermitBusyAllowsBusySimulation) {
    AddSim();
    Registry.read(0)->IsProcessing = true;
    Registry.read(0)->WorkRequested = true;
    HandlerData H = Make({{"SimulationID", 0}}, /*PermitBusy=*/true);
    EXPECT_FALSE(H.HasError());
    EXPECT_NE(H.Sim(), nullptr);
}

TEST_F(HandlerDataTest, IdleSimulationIsNotBusy) {
    AddSim();
    EXPECT_FALSE(Make({{"SimulationID", 0}}).HasError());
}

TEST_F(HandlerDataTest, NoSimulationRouteNeedsNeitherIdNorRegistry) {
    HandlerData H(json::object().dump(), Logger_.get(), "NoSim", nullptr, false, /*NoSimulation=*/true);
    EXPECT_FALSE(H.HasError());
    EXPECT_EQ(H.Sim(), nullptr);
    EXPECT_EQ(H.SimID(), -1);
}

TEST_F(HandlerDataTest, NoSimulationIgnoresSimulationIdEvenIfInvalid) {
    HandlerData H = Make({{"SimulationID", -5}}, false, /*NoSimulation=*/true);
    EXPECT_FALSE(H.HasError());
    EXPECT_EQ(H.Sim(), nullptr);
}

TEST_F(HandlerDataTest, SimulationRouteWithNullRegistryIsInvalidParameters) {
    HandlerData H(json({{"SimulationID", 0}}).dump(), Logger_.get(), "Sim", nullptr, false, false);
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataTest, RequestJSONIsKeptVerbatim) {
    AddSim();
    json Req = {{"SimulationID", 0}, {"Name", "n"}, {"Nested", {{"a", 1}}}};
    HandlerData H = Make(Req);
    EXPECT_EQ(H.ReqJSON(), Req);
}

// ---- RequestUsername -------------------------------------------------------------------

TEST_F(HandlerDataTest, UsernameDefaultsToAnonymous) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(H.RequestUsername(), "anonymous");
    EXPECT_EQ(H.Sim()->OwnerUsername, "anonymous");
}

TEST_F(HandlerDataTest, EmptyUsernameBecomesAnonymousAndCreatesNoDirectory) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}, {"RequestUsername", ""}});
    EXPECT_EQ(H.RequestUsername(), "anonymous");
    EXPECT_EQ(H.Sim()->OwnerUsername, "anonymous");
    EXPECT_FALSE(std::filesystem::exists(BG::NES::Util::Storage::UserRoot("alice")));
}

TEST_F(HandlerDataTest, NonStringUsernameIsIgnored) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}, {"RequestUsername", 42}});
    EXPECT_EQ(H.RequestUsername(), "anonymous");
}

TEST_F(HandlerDataTest, NamedUserOwnsSimulationAndGetsDirectoryUnderTempRoot) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}, {"RequestUsername", "alice"}});
    EXPECT_EQ(H.RequestUsername(), "alice");
    EXPECT_EQ(H.Sim()->OwnerUsername, "alice");
    std::string AliceRoot = BG::NES::Util::Storage::UserRoot("alice");
    EXPECT_TRUE(std::filesystem::is_directory(AliceRoot));
    EXPECT_EQ(AliceRoot.rfind(RootPath(), 0), 0u) << "user root must be under the temp output root: " << AliceRoot;
}

TEST_F(HandlerDataTest, AnonymousRequestKeepsExistingOwner) {
    AddSim();
    Registry.read(0)->OwnerUsername = "bob";
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(H.Sim()->OwnerUsername, "bob");
}

TEST_F(HandlerDataTest, AnonymousRequestFillsEmptyOwner) {
    AddSim();
    Registry.read(0)->OwnerUsername.clear();
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(H.Sim()->OwnerUsername, "anonymous");
}

TEST_F(HandlerDataTest, NoSimulationRouteReadsUsernameButTouchesNoStorage) {
    HandlerData H = Make({{"RequestUsername", "carol"}}, false, true);
    EXPECT_EQ(H.RequestUsername(), "carol");
    EXPECT_FALSE(std::filesystem::exists(BG::NES::Util::Storage::UserRoot("carol")));
}

// ---- Typed getters ---------------------------------------------------------------------

class HandlerDataGetterTest : public HandlerDataTest {
protected:
    // NoSimulation handler so only the getters are under test.
    HandlerData Getters(const json& Request) { return Make(Request, false, true); }
};

TEST_F(HandlerDataGetterTest, GetParIntReadsIntegers) {
    HandlerData H = Getters({{"N", 42}, {"Neg", -7}});
    int V = 0;
    EXPECT_TRUE(H.GetParInt("N", V));
    EXPECT_EQ(V, 42);
    EXPECT_TRUE(H.GetParInt("Neg", V));
    EXPECT_EQ(V, -7);
    EXPECT_FALSE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParIntTruncatesFloatsTowardZero) {
    HandlerData H = Getters({{"Pos", 5.9}, {"Neg", -5.9}});
    int V = 0;
    EXPECT_TRUE(H.GetParInt("Pos", V));
    EXPECT_EQ(V, 5);
    EXPECT_TRUE(H.GetParInt("Neg", V));
    EXPECT_EQ(V, -5);
}

TEST_F(HandlerDataGetterTest, GetParIntRejectsWrongTypesAndMissing) {
    HandlerData H = Getters({{"S", "5"}, {"B", true}, {"Nul", nullptr}});
    int V = 99;
    for (const char* Name : {"S", "B", "Nul", "Missing"}) {
        EXPECT_FALSE(H.GetParInt(Name, V)) << Name;
        EXPECT_EQ(V, 99) << "value must be untouched on failure: " << Name;
    }
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataGetterTest, GetParFloatAcceptsIntsAndFloats) {
    HandlerData H = Getters({{"F", 1.5}, {"I", 3}});
    float V = 0;
    EXPECT_TRUE(H.GetParFloat("F", V));
    EXPECT_NEAR(V, 1.5f, 1e-6);
    EXPECT_TRUE(H.GetParFloat("I", V));
    EXPECT_NEAR(V, 3.0f, 1e-6);
    EXPECT_FALSE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParFloatRejectsStringsAndMissing) {
    HandlerData H = Getters({{"S", "1.5"}});
    float V = -1;
    EXPECT_FALSE(H.GetParFloat("S", V));
    EXPECT_TRUE(H.HasError());
    HandlerData H2 = Getters(json::object());
    EXPECT_FALSE(H2.GetParFloat("Missing", V));
    EXPECT_EQ(H2.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
    EXPECT_NEAR(V, -1.0f, 1e-6);
}

TEST_F(HandlerDataGetterTest, OptionalMissingFloatReturnsFalseWithoutError) {
    HandlerData H = Getters(json::object());
    float V = 7;
    EXPECT_FALSE(H.GetParFloat("Missing", V, /*_Optional=*/true));
    EXPECT_FALSE(H.HasError());
    EXPECT_NEAR(V, 7.0f, 1e-6);
}

TEST_F(HandlerDataGetterTest, OptionalPresentButWrongTypeStillErrors) {
    HandlerData H = Getters({{"S", "x"}});
    float V = 0;
    EXPECT_FALSE(H.GetParFloat("S", V, true));
    EXPECT_TRUE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParBoolOnlyAcceptsBooleans) {
    HandlerData H = Getters({{"T", true}, {"F", false}, {"One", 1}, {"S", "true"}});
    bool V = false;
    EXPECT_TRUE(H.GetParBool("T", V));
    EXPECT_TRUE(V);
    EXPECT_TRUE(H.GetParBool("F", V));
    EXPECT_FALSE(V);
    EXPECT_FALSE(H.HasError());
    V = true;
    EXPECT_FALSE(H.GetParBool("One", V));
    EXPECT_FALSE(H.GetParBool("S", V));
    EXPECT_FALSE(H.GetParBool("Missing", V));
    EXPECT_TRUE(V);
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataGetterTest, GetParStringOnlyAcceptsStrings) {
    HandlerData H = Getters({{"S", "hello"}, {"E", ""}, {"N", 5}});
    std::string V = "keep";
    EXPECT_TRUE(H.GetParString("S", V));
    EXPECT_EQ(V, "hello");
    EXPECT_TRUE(H.GetParString("E", V));
    EXPECT_EQ(V, "");
    EXPECT_FALSE(H.HasError());
    V = "keep";
    EXPECT_FALSE(H.GetParString("N", V));
    EXPECT_FALSE(H.GetParString("Missing", V));
    EXPECT_EQ(V, "keep");
    EXPECT_TRUE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParVec3ReadsDefaultMicrometerSuffixes) {
    HandlerData H = Getters({{"PosX_um", 1.0}, {"PosY_um", -2.5}, {"PosZ_um", 3}});
    Vec3D V;
    EXPECT_TRUE(H.GetParVec3("Pos", V));
    EXPECT_NEAR(V.x, 1.0f, 1e-6);
    EXPECT_NEAR(V.y, -2.5f, 1e-6);
    EXPECT_NEAR(V.z, 3.0f, 1e-6);
}

TEST_F(HandlerDataGetterTest, GetParVec3HonoursUnitsAndRequiresAllThree) {
    HandlerData H = Getters({{"PosX_nm", 1}, {"PosY_nm", 2}, {"PosX_um", 9}, {"PosY_um", 9}});
    Vec3D V;
    EXPECT_FALSE(H.GetParVec3("Pos", V, "nm"));  // Z missing
    EXPECT_TRUE(H.HasError());
    EXPECT_FALSE(H.GetParVec3("Pos", V));        // um: Z missing
}

TEST_F(HandlerDataGetterTest, GetParVec3WrongComponentTypeFails) {
    HandlerData H = Getters({{"PX_um", 1}, {"PY_um", "two"}, {"PZ_um", 3}});
    Vec3D V;
    EXPECT_FALSE(H.GetParVec3("P", V));
    EXPECT_EQ(H.GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed);
}

TEST_F(HandlerDataGetterTest, GetParVecIntReadsArraysAndTruncates) {
    HandlerData H = Getters({{"A", {1, 2, 3}}, {"F", {1.9, -1.9}}, {"Bad", {1, "x"}}, {"NotArr", 5}});
    std::vector<int> V;
    EXPECT_TRUE(H.GetParVecInt("A", V));
    EXPECT_EQ(V, (std::vector<int>{1, 2, 3}));
    V.clear();
    EXPECT_TRUE(H.GetParVecInt("F", V));
    EXPECT_EQ(V, (std::vector<int>{1, -1}));
    V.clear();
    EXPECT_FALSE(H.GetParVecInt("Bad", V));
    EXPECT_FALSE(H.GetParVecInt("NotArr", V));
    EXPECT_TRUE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParVecIntOptionalMissingIsNotAnError) {
    HandlerData H = Getters(json::object());
    std::vector<int> V;
    EXPECT_FALSE(H.GetParVecInt("A", V, true));
    EXPECT_FALSE(H.HasError());
    EXPECT_FALSE(H.GetParVecInt("A", V, false));
    EXPECT_TRUE(H.HasError());
}

TEST_F(HandlerDataGetterTest, GetParVecFloatReadsNamedAndTopLevelArrays) {
    HandlerData H = Getters({{"A", {0.5, 1, 2.5}}, {"Bad", {0.5, "x"}}});
    std::vector<float> V;
    EXPECT_TRUE(H.GetParVecFloat("A", V));
    ASSERT_EQ(V.size(), 3u);
    EXPECT_NEAR(V[0], 0.5f, 1e-6);
    EXPECT_NEAR(V[1], 1.0f, 1e-6);
    EXPECT_NEAR(V[2], 2.5f, 1e-6);

    json Top = json::array({1.5, 2.5});
    std::vector<float> W;
    EXPECT_TRUE(H.GetParVecFloat("", W, Top));
    ASSERT_EQ(W.size(), 2u);
    EXPECT_NEAR(W[1], 2.5f, 1e-6);

    json NotArray = {{"x", 1}};
    EXPECT_FALSE(H.GetParVecFloat("", W, NotArray));
    EXPECT_TRUE(H.HasError());

    std::vector<float> X;
    EXPECT_FALSE(H.GetParVecFloat("Bad", X));
    EXPECT_FALSE(H.GetParVecFloat("Missing", X));
}

TEST_F(HandlerDataGetterTest, ExplicitJsonOverloadsReadFromTheGivenObjectNotTheRequest) {
    HandlerData H = Getters({{"N", 1}});
    json Other = {{"N", 2}, {"S", "s"}, {"B", true}, {"F", 0.25}};
    int I = 0; std::string S; bool B = false; float F = 0;
    EXPECT_TRUE(H.GetParInt("N", I, Other));
    EXPECT_EQ(I, 2);
    EXPECT_TRUE(H.GetParString("S", S, Other));
    EXPECT_EQ(S, "s");
    EXPECT_TRUE(H.GetParBool("B", B, Other));
    EXPECT_TRUE(B);
    EXPECT_TRUE(H.GetParFloat("F", F, Other));
    EXPECT_NEAR(F, 0.25f, 1e-6);
}

TEST_F(HandlerDataGetterTest, FindParLocatesKeysAndHonoursOptional) {
    HandlerData H = Getters({{"K", 1}});
    json::iterator It;
    EXPECT_TRUE(H.FindPar("K", It));
    EXPECT_EQ(It.value(), 1);
    EXPECT_FALSE(H.FindPar("Nope", It, true));
    EXPECT_FALSE(H.HasError());
    EXPECT_FALSE(H.FindPar("Nope", It));
    EXPECT_TRUE(H.HasError());
}

// ---- Responses -------------------------------------------------------------------------

TEST_F(HandlerDataTest, ErrResponseWithIntSetsStatusAndShape) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    ASSERT_FALSE(H.HasError());
    json R = json::parse(H.ErrResponse(3));
    EXPECT_EQ(R, json({{"StatusCode", 3}}));
    EXPECT_TRUE(H.HasError());
    EXPECT_EQ(int(H.GetStatus()), 3);
}

TEST_F(HandlerDataTest, ErrResponseWithEnumAndNoArg) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(json::parse(H.ErrResponse(BGStatusCode::BGStatusGeneralFailure)), json({{"StatusCode", 1}}));
    EXPECT_EQ(json::parse(H.ErrResponse()), json({{"StatusCode", 1}}));  // reuses the stored status
}

TEST_F(HandlerDataTest, ResponseWithIdCarriesStatusAndId) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(json::parse(H.ResponseWithID("NeuronID", 12)), json({{"StatusCode", 0}, {"NeuronID", 12}}));
    EXPECT_EQ(json::parse(H.ResponseWithID("Handle", std::string("abc"))), json({{"StatusCode", 0}, {"Handle", "abc"}}));
}

TEST_F(HandlerDataTest, ResponseWithIdReportsCurrentErrorStatus) {
    AddSim();
    Registry.read(0)->IsProcessing = true;
    HandlerData H = Make({{"SimulationID", 0}});
    EXPECT_EQ(json::parse(H.ResponseWithID("X", 1)), json({{"StatusCode", 5}, {"X", 1}}));
}

TEST_F(HandlerDataTest, StringResponseShape) {
    HandlerData H = Make(json::object(), false, true);
    EXPECT_EQ(json::parse(H.StringResponse("Key", "Value")), json({{"StatusCode", 0}, {"Key", "Value"}}));
}

TEST_F(HandlerDataTest, ResponseAndStoreRequestRecordsOnSimulationOnly) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    Simulation* Sim = H.Sim();
    ASSERT_EQ(Sim->NumStoredRequests(), 0u);
    json Body = {{"StatusCode", 0}};
    EXPECT_EQ(H.ResponseAndStoreRequest(Body), "{\"StatusCode\":0}");
    EXPECT_EQ(Sim->NumStoredRequests(), 1u);
    H.ResponseWithID("A", 1);
    EXPECT_EQ(Sim->NumStoredRequests(), 2u);
    H.ErrResponse(0);  // ErrResponse(0) is the success reply, so it is journaled
    EXPECT_EQ(Sim->NumStoredRequests(), 3u);
    H.StringResponse("K", "V");  // does not store
    EXPECT_EQ(Sim->NumStoredRequests(), 3u);
}

TEST_F(HandlerDataTest, NoSimulationHandlerStoresNothingAndDoesNotCrash) {
    HandlerData H = Make(json::object(), false, true);
    json Body = {{"StatusCode", 0}};
    EXPECT_EQ(H.ResponseAndStoreRequest(Body), "{\"StatusCode\":0}");
    EXPECT_EQ(H.ErrResponse(2), "{\"StatusCode\":2}");
}

// ---- Known bug -------------------------------------------------------------------------

// Bug: dev_frame #137
// A rejected request must not be written into the model journal: replaying it on load
// would re-run a request the server refused. Only successful responses are recorded.
class HandlerDataJournalKnownBug : public HandlerDataTest {};

TEST_F(HandlerDataJournalKnownBug, ErrorResponseIsNotJournaled) {
    AddSim();
    HandlerData H = Make({{"SimulationID", 0}});
    Simulation* Sim = H.Sim();
    ASSERT_EQ(Sim->NumStoredRequests(), 0u);
    H.ErrResponse(2);
    EXPECT_EQ(Sim->NumStoredRequests(), 0u);
}

// Bug: dev_frame #102
// Malformed JSON should produce an error status, not an exception out of the constructor.
TEST(HandlerDataKnownBug, MalformedJsonDoesNotThrowAndReportsInvalidParameters) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    for (const char* Bad : {"{not json", "", "[1,2", "\xff\xfe"}) {
        std::unique_ptr<HandlerData> H;
        EXPECT_NO_THROW(H = std::make_unique<HandlerData>(Bad, Logger.get(), "R", nullptr, true, true)) << Bad;
        if (H) {
            EXPECT_EQ(H->GetStatus(), BGStatusCode::BGStatusInvalidParametersPassed) << Bad;
        }
    }
}

}  // namespace
