//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for API::RPCManager: the "NES" batch dispatcher, route
                 registration, API checksum / manifest, background request bookkeeping
                 and the static routes. Uses a loopback ephemeral-port Config. (dev_frame NT-10)
*/

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <mutex>
#include <thread>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <rpc/client.h>
#include <rpc/rpc_error.h>

#include <RPC/RPCManager.h>
#include <RPC/StaticRoutes.h>
#include <TestUtil/TestUtil.h>
#include <Version.h>

using nlohmann::json;
using BG::NES::API::RPCManager;

namespace {

// Independent FNV-1a 64 (offset basis / prime from the FNV spec), lowercase hex, 16 digits.
std::string Fnv1a64Hex(const std::string& Data) {
    uint64_t H = 0xcbf29ce484222325ULL;
    for (unsigned char C : Data) {
        H ^= C;
        H *= 0x100000001b3ULL;
    }
    std::ostringstream Out;
    Out << std::hex << std::setw(16) << std::setfill('0') << H;
    return Out.str();
}

// RPCManager's destructor blocks until SafeClient's thread exits, and that thread only checks its
// exit flag after SetupCallback() has been called (see RPCManagerKnownBug.DestructorReturnsWithoutCallbackSetup).
// So: every manager under test gets a callback target (itself) and is retired into a graveyard that
// is torn down in parallel at the end of the run, which keeps the whole file at a few seconds.
// The logger must outlive the managers, hence one process-wide silent logger.
BG::Common::Logger::LoggingSystem* SharedLogger() {
    static auto* Logger = BG::NES::TestUtil::MakeSilentLogger().release();
    return Logger;
}

std::unique_ptr<RPCManager> StartManager(BG::NES::Config::Config& Cfg) {
    auto Mgr = std::make_unique<RPCManager>(&Cfg, SharedLogger());
    Mgr->SetupCallback(json({{"CallbackHost", "127.0.0.1"}, {"CallbackPort", Cfg.PortNumber}}).dump());
    return Mgr;
}

std::mutex GraveMutex;
std::vector<std::unique_ptr<RPCManager>> Grave;

void Retire(std::unique_ptr<RPCManager> Mgr) {
    std::lock_guard<std::mutex> Lock(GraveMutex);
    Grave.push_back(std::move(Mgr));
}

class GraveEnvironment : public ::testing::Environment {
public:
    void TearDown() override {
        std::vector<std::thread> Workers;
        for (auto& Mgr : Grave) {
            Workers.emplace_back([M = std::move(Mgr)]() mutable { M.reset(); });
        }
        for (auto& W : Workers) W.join();
        Grave.clear();
    }
};
::testing::Environment* const GraveEnv = ::testing::AddGlobalTestEnvironment(new GraveEnvironment);

// One manager shared by all dispatch tests (routes are given per-test unique names).
class RPCManagerTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        SharedConfig = BG::NES::TestUtil::MakeLoopbackConfig();
        Shared = StartManager(SharedConfig);
    }
    static void TearDownTestSuite() {
        Retire(std::move(Shared));
    }

    // Unique route name per test, so the shared manager's route table never collides.
    static std::string Route(const std::string& Base) {
        return std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + "_" + Base;
    }

    json Batch(const json& Requests, int SimulationIDOverride = -1) {
        return json::parse(Shared->NESRequest(Requests.dump(), SimulationIDOverride));
    }

    static BG::NES::Config::Config SharedConfig;
    static std::unique_ptr<RPCManager> Shared;
};
BG::NES::Config::Config RPCManagerTest::SharedConfig;
std::unique_ptr<RPCManager> RPCManagerTest::Shared;

// A fresh manager (own port, no routes) per test, for exact checksum / manifest expectations.
class FreshManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Cfg = BG::NES::TestUtil::MakeLoopbackConfig();
        Mgr = StartManager(Cfg);
    }
    void TearDown() override { Retire(std::move(Mgr)); }

    BG::NES::Config::Config Cfg;
    std::unique_ptr<RPCManager> Mgr;
};

// ---- Batch dispatch --------------------------------------------------------------------

const std::string kOk = "{\"StatusCode\":0}";

TEST_F(RPCManagerTest, EmptyArrayGivesEmptyArray) {
    EXPECT_EQ(Shared->NESRequest("[]"), "[]");
}

TEST_F(RPCManagerTest, NonArrayRequestIsInvalidParameters) {
    for (const char* Bad : {"{}", "{\"ReqID\":1}", "5", "\"str\"", "null", "true"}) {
        EXPECT_EQ(json::parse(Shared->NESRequest(Bad)), json({{"StatusCode", 2}})) << Bad;
    }
}

TEST_F(RPCManagerTest, UnknownRouteReportsStatus1AndEchoesReqID) {
    json R = Batch(json::array({{{"ReqID", 7}, {"NoSuchRoute", json::object()}}}));
    ASSERT_EQ(R.size(), 1u);
    EXPECT_EQ(R[0], json({{"ReqID", 7}, {"StatusCode", 1}}));
}

TEST_F(RPCManagerTest, MissingReqIdIsReportedAsMinusOne) {
    json R = Batch(json::array({{{"NoSuchRoute", json::object()}}}));
    EXPECT_EQ(R[0]["ReqID"], -1);
    EXPECT_EQ(R[0]["StatusCode"], 1);
}

TEST_F(RPCManagerTest, MissingReqIdOnRegisteredRouteIsAlsoMinusOne) {
    std::string Name = Route("Ok");
    Shared->AddRoute(Name, [](std::string) { return kOk; });
    json R = Batch(json::array({{{Name, json::object()}}}));
    EXPECT_EQ(R[0], json({{"ReqID", -1}, {"StatusCode", 0}}));
}

TEST_F(RPCManagerTest, ItemWithOnlyReqIdIsUnknownRoute) {
    json R = Batch(json::array({{{"ReqID", 3}}}));
    EXPECT_EQ(R[0], json({{"ReqID", 3}, {"StatusCode", 1}}));
}

TEST_F(RPCManagerTest, RegisteredRouteReceivesExactlyItsParams) {
    std::string Name = Route("Probe");
    std::vector<std::string> Seen;
    Shared->AddRoute(Name, [&](std::string Req) { Seen.push_back(Req); return kOk; });
    json Params = {{"SimulationID", 4}, {"Name", "n"}, {"Arr", {1, 2}}, {"Nested", {{"k", 1.5}}}};
    json R = Batch(json::array({{{"ReqID", 1}, {Name, Params}}}));
    ASSERT_EQ(Seen.size(), 1u);
    EXPECT_EQ(json::parse(Seen[0]), Params);  // no ReqID, no extra keys
    EXPECT_EQ(R[0], json({{"ReqID", 1}, {"StatusCode", 0}}));
}

TEST_F(RPCManagerTest, HandlerResponseFieldsPassThroughAndReqIdOverridesHandlersOwn) {
    std::string Name = Route("Make");
    Shared->AddRoute(Name, [](std::string) { return std::string("{\"StatusCode\":0,\"NeuronID\":9,\"ReqID\":999}"); });
    json R = Batch(json::array({{{"ReqID", 12}, {Name, json::object()}}}));
    EXPECT_EQ(R[0], json({{"ReqID", 12}, {"StatusCode", 0}, {"NeuronID", 9}}));
}

TEST_F(RPCManagerTest, ErrorStatusFromHandlerIsPreserved) {
    std::string Name = Route("Fail");
    Shared->AddRoute(Name, [](std::string) { return std::string("{\"StatusCode\":5}"); });
    EXPECT_EQ(Batch(json::array({{{"ReqID", 1}, {Name, json::object()}}}))[0]["StatusCode"], 5);
}

TEST_F(RPCManagerTest, ResultsComeBackInRequestOrderAndHandlersRunInOrder) {
    std::string A = Route("A"), B = Route("B");
    std::vector<std::string> CallOrder;
    Shared->AddRoute(A, [&](std::string) { CallOrder.push_back("A"); return std::string("{\"StatusCode\":0,\"Who\":\"A\"}"); });
    Shared->AddRoute(B, [&](std::string) { CallOrder.push_back("B"); return std::string("{\"StatusCode\":0,\"Who\":\"B\"}"); });
    json Req = json::array({
        {{"ReqID", 30}, {B, json::object()}},
        {{"ReqID", 10}, {A, json::object()}},
        {{"ReqID", 20}, {"Missing", json::object()}},
        {{"ReqID", 0}, {A, json::object()}},
    });
    json R = Batch(Req);
    ASSERT_EQ(R.size(), 4u);
    EXPECT_EQ(R[0]["ReqID"], 30);
    EXPECT_EQ(R[0]["Who"], "B");
    EXPECT_EQ(R[1]["ReqID"], 10);
    EXPECT_EQ(R[1]["Who"], "A");
    EXPECT_EQ(R[2]["ReqID"], 20);
    EXPECT_EQ(R[2]["StatusCode"], 1);
    EXPECT_EQ(R[3]["ReqID"], 0);
    EXPECT_EQ(CallOrder, (std::vector<std::string>{"B", "A", "A"}));
}

TEST_F(RPCManagerTest, SimulationIdOverrideIsInjectedIntoEveryItem) {
    std::string Name = Route("Probe");
    std::vector<json> Seen;
    Shared->AddRoute(Name, [&](std::string Req) { Seen.push_back(json::parse(Req)); return kOk; });
    Batch(json::array({
        {{"ReqID", 1}, {Name, {{"SimulationID", 99}, {"X", 1}}}},
        {{"ReqID", 2}, {Name, json::object()}},
    }), /*SimulationIDOverride=*/3);
    ASSERT_EQ(Seen.size(), 2u);
    EXPECT_EQ(Seen[0], json({{"SimulationID", 3}, {"X", 1}}));
    EXPECT_EQ(Seen[1], json({{"SimulationID", 3}}));
}

TEST_F(RPCManagerTest, NoOverrideLeavesParamsUntouched) {
    std::string Name = Route("Probe");
    json Seen;
    Shared->AddRoute(Name, [&](std::string Req) { Seen = json::parse(Req); return kOk; });
    Batch(json::array({{{"ReqID", 1}, {Name, {{"SimulationID", 99}}}}}));
    EXPECT_EQ(Seen, json({{"SimulationID", 99}}));
}

TEST_F(RPCManagerTest, FirstRegistrationOfARouteWins) {
    std::string Name = Route("Dup");
    Shared->AddRoute(Name, [](std::string) { return std::string("{\"StatusCode\":0,\"V\":1}"); });
    Shared->AddRoute(Name, [](std::string) { return std::string("{\"StatusCode\":0,\"V\":2}"); });
    EXPECT_EQ(Batch(json::array({{{"ReqID", 1}, {Name, json::object()}}}))[0]["V"], 1);
}

// ---- Checksum / manifest (fresh manager: exact route table) ----------------------------

const auto kNoop = [](std::string) { return std::string("{}"); };

TEST_F(FreshManagerTest, ChecksumWithNoRegisteredRoutesIsFnvOfEmptyString) {
    EXPECT_EQ(Mgr->ComputeChecksum(), "cbf29ce484222325");
    EXPECT_EQ(Mgr->ComputeChecksum(), Fnv1a64Hex(""));
}

TEST_F(FreshManagerTest, ChecksumIsFnv1aOfSortedRouteNamesEachFollowedByNewline) {
    // The implementation appends "\n" after every name (including the last).
    for (const char* Name : {"Zeta", "Alpha", "Mid", "alpha"}) Mgr->AddRoute(Name, kNoop);
    // Byte order: uppercase before lowercase.
    EXPECT_EQ(Mgr->ComputeChecksum(), Fnv1a64Hex("Alpha\nMid\nZeta\nalpha\n"));
}

TEST_F(FreshManagerTest, ChecksumIndependentOfRegistrationOrder) {
    Mgr->AddRoute("B", kNoop);
    Mgr->AddRoute("A", kNoop);
    std::string First = Mgr->ComputeChecksum();

    BG::NES::Config::Config OtherConfig = BG::NES::TestUtil::MakeLoopbackConfig();
    auto Other = StartManager(OtherConfig);
    Other->AddRoute("A", kNoop);
    Other->AddRoute("B", kNoop);
    EXPECT_EQ(Other->ComputeChecksum(), First);
    EXPECT_EQ(First, Fnv1a64Hex("A\nB\n"));
    Retire(std::move(Other));
}

TEST_F(FreshManagerTest, ChecksumChangesWhenARouteIsAdded) {
    Mgr->AddRoute("A", kNoop);
    std::string Before = Mgr->ComputeChecksum();
    Mgr->AddRoute("B", kNoop);
    EXPECT_NE(Mgr->ComputeChecksum(), Before);
}

TEST_F(FreshManagerTest, ManifestIsSortedJsonArrayOfRegisteredRoutes) {
    EXPECT_EQ(Mgr->GetManifestJSON(), "[]");
    for (const char* Name : {"Zeta", "Alpha", "Mid"}) Mgr->AddRoute(Name, kNoop);
    EXPECT_EQ(json::parse(Mgr->GetManifestJSON()), json::array({"Alpha", "Mid", "Zeta"}));
}

TEST_F(FreshManagerTest, BuiltInRoutesAreNotPartOfChecksumOrManifest) {
    // GetAPIVersion, Echo, NES, SetCallback, GetAPIChecksum, GetAPIManifest are bound on the RPC
    // server directly, not through AddRoute, so they are not in the route table.
    EXPECT_EQ(Mgr->GetManifestJSON(), "[]");
}

// ---- Background request bookkeeping ----------------------------------------------------

TEST_F(FreshManagerTest, BgRequestIdsIncreaseFromZero) {
    EXPECT_EQ(Mgr->GetBgRequestID(), 0);
    EXPECT_EQ(Mgr->GetBgRequestID(), 1);
    EXPECT_EQ(Mgr->GetBgRequestID(), 2);
}

TEST_F(FreshManagerTest, BgProcessRegisterAndUnregister) {
    json Status;
    long Id = Mgr->GetBgRequestID();
    EXPECT_FALSE(Mgr->UnRegisterBgAPIProcess(Id));
    Mgr->RegisterBgAPIProcess(Id, &Status);
    EXPECT_TRUE(Mgr->UnRegisterBgAPIProcess(Id));
    EXPECT_FALSE(Mgr->UnRegisterBgAPIProcess(Id));
}

// ---- Static routes and a real round trip over loopback ---------------------------------

TEST(StaticRoutes, VersionAndEcho) {
    EXPECT_EQ(BG::NES::API::GetAPIVersion(), std::string(NES_API_VERSION));
    EXPECT_FALSE(BG::NES::API::GetAPIVersion().empty());
    EXPECT_EQ(BG::NES::API::Echo("hello"), "hello");
    EXPECT_EQ(BG::NES::API::Echo(""), "");
    EXPECT_EQ(BG::NES::API::Echo(std::string("a\0b", 3)), std::string("a\0b", 3));
}

TEST_F(RPCManagerTest, BuiltInRoutesAnswerOverLoopback) {
    std::string Name = Route("Probe");
    Shared->AddRoute(Name, [](std::string) { return kOk; });
    rpc::client Client("127.0.0.1", SharedConfig.PortNumber);
    Client.set_timeout(5000);
    EXPECT_EQ(Client.call("GetAPIVersion").as<std::string>(), std::string(NES_API_VERSION));
    EXPECT_EQ(Client.call("Echo", "ping").as<std::string>(), "ping");
    EXPECT_EQ(Client.call("GetAPIChecksum").as<std::string>(), Shared->ComputeChecksum());
    json Manifest = json::parse(Client.call("GetAPIManifest").as<std::string>());
    EXPECT_EQ(Manifest, json::parse(Shared->GetManifestJSON()));
    EXPECT_NE(std::find(Manifest.begin(), Manifest.end(), Name), Manifest.end());
    json Req = json::array({{{"ReqID", 5}, {Name, {{"a", 1}}}}});
    json R = json::parse(Client.call("NES", Req.dump()).as<std::string>());
    ASSERT_EQ(R.size(), 1u);
    EXPECT_EQ(R[0], json({{"ReqID", 5}, {"StatusCode", 0}}));
}

TEST_F(RPCManagerTest, UnknownRpcRouteIsAnRpcError) {
    rpc::client Client("127.0.0.1", SharedConfig.PortNumber);
    Client.set_timeout(5000);
    EXPECT_THROW(Client.call("NoSuchRpcRoute"), rpc::rpc_error);
}

// ---- Known bugs ------------------------------------------------------------------------

using RPCManagerKnownBug = RPCManagerTest;

// Bug: dev_frame #102
TEST_F(RPCManagerKnownBug, MalformedBatchJsonDoesNotThrow) {
    for (const char* Bad : {"{not json", "", "[{\"ReqID\":1"}) {
        std::string R;
        EXPECT_NO_THROW(R = Shared->NESRequest(Bad)) << Bad;
        if (!R.empty()) EXPECT_EQ(json::parse(R, nullptr, false), json({{"StatusCode", 2}})) << Bad;
    }
}

// Bug: dev_frame #102
// A bad item must be reported for that item; the other items in the batch must still be answered.
TEST_F(RPCManagerKnownBug, NonIntegerReqIdDoesNotAbortTheBatch) {
    json Req = json::array({
        {{"ReqID", "abc"}, {"Echo", json::object()}},
        {{"ReqID", 2}, {"NoSuchRoute", json::object()}},
    });
    std::string Raw;
    ASSERT_NO_THROW(Raw = Shared->NESRequest(Req.dump()));
    json R = json::parse(Raw);
    ASSERT_TRUE(R.is_array());
    ASSERT_EQ(R.size(), 2u);
    EXPECT_NE(R[0]["StatusCode"], 0);
    EXPECT_EQ(R[1]["ReqID"], 2);
}

// Bug: dev_frame #102
TEST_F(RPCManagerKnownBug, HandlerReturningNonJsonDoesNotAbortTheBatch) {
    std::string Name = Route("Broken");
    Shared->AddRoute(Name, [](std::string) { return std::string("not json"); });
    json Req = json::array({
        {{"ReqID", 1}, {Name, json::object()}},
        {{"ReqID", 2}, {"NoSuchRoute", json::object()}},
    });
    std::string Raw;
    ASSERT_NO_THROW(Raw = Shared->NESRequest(Req.dump()));
    json R = json::parse(Raw);
    ASSERT_EQ(R.size(), 2u);
    EXPECT_NE(R[0]["StatusCode"], 0);
    EXPECT_EQ(R[1]["ReqID"], 2);
}

// Bug: dev_frame #102
TEST_F(RPCManagerKnownBug, ThrowingHandlerDoesNotAbortTheBatch) {
    std::string Name = Route("Throws");
    Shared->AddRoute(Name, [](std::string) -> std::string { throw std::runtime_error("boom"); });
    json Req = json::array({
        {{"ReqID", 1}, {Name, json::object()}},
        {{"ReqID", 2}, {"NoSuchRoute", json::object()}},
    });
    std::string Raw;
    ASSERT_NO_THROW(Raw = Shared->NESRequest(Req.dump()));
    json R = json::parse(Raw);
    ASSERT_EQ(R.size(), 2u);
    EXPECT_NE(R[0]["StatusCode"], 0);
    EXPECT_EQ(R[1]["ReqID"], 2);
}

// Bug: dev_frame #104
// A registered-but-empty handler is skipped with only a log line; the item has no StatusCode,
// so a client reading StatusCode cannot tell that nothing ran.
TEST_F(RPCManagerKnownBug, NullHandlerItemReportsAnErrorStatus) {
    std::string Name = Route("Empty");
    Shared->AddRoute(Name, std::function<std::string(std::string)>());
    json R = Batch(json::array({{{"ReqID", 4}, {Name, json::object()}}}));
    ASSERT_EQ(R.size(), 1u);
    EXPECT_EQ(R[0]["ReqID"], 4);
    ASSERT_TRUE(R[0].contains("StatusCode"));
    EXPECT_NE(R[0]["StatusCode"], 0);
}

// Bug: dev_frame #72 (#103 was closed as a duplicate of it)
// RPCManager reads Config::Host but never uses it: rpc::server(port) listens on every interface.
// The Config asks for 127.0.0.1, so the listening socket must be bound to 127.0.0.1 only.
TEST_F(RPCManagerKnownBug, ListensOnlyOnConfiguredHost) {
    std::ifstream Tcp("/proc/net/tcp");
    ASSERT_TRUE(Tcp.is_open());
    std::ostringstream PortHex;
    PortHex << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << SharedConfig.PortNumber;
    std::string Line, LocalAddr;
    std::getline(Tcp, Line);  // header
    while (std::getline(Tcp, Line)) {
        std::istringstream Fields(Line);
        std::string Sl, Local, Remote, State;
        Fields >> Sl >> Local >> Remote >> State;
        if (State == "0A" && Local.size() > 9 && Local.substr(Local.find(':') + 1) == PortHex.str()) {
            LocalAddr = Local.substr(0, Local.find(':'));
        }
    }
    ASSERT_FALSE(LocalAddr.empty()) << "no listening socket found for port " << SharedConfig.PortNumber;
    EXPECT_EQ(LocalAddr, "0100007F") << "expected 127.0.0.1, got hex address " << LocalAddr;
}

// Bug: dev_frame #105
// ~RPCManager joins SafeClient's thread, which spins in "while (RPCHost_ == "")" without looking at
// RequestExit_ until SetCallback has been called, so a manager that never got a callback never
// shuts down. The destruction runs on a detached thread so a hang fails the test instead of the run.
TEST(RPCManagerShutdownKnownBug, DestructorReturnsWithoutCallbackSetup) {
    auto Done = std::make_shared<std::promise<void>>();
    std::future<void> Finished = Done->get_future();
    std::thread([Done]() {
        BG::NES::Config::Config Cfg = BG::NES::TestUtil::MakeLoopbackConfig();
        {
            RPCManager Mgr(&Cfg, SharedLogger());  // no SetupCallback
        }
        Done->set_value();
    }).detach();
    EXPECT_EQ(Finished.wait_for(std::chrono::seconds(3)), std::future_status::ready)
        << "~RPCManager did not return within 3 s";
}

}  // namespace
