//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Smoke tests for the shared test kit (TestUtil.h).
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include <TestUtil/TestUtil.h>

using namespace BG::NES;

TEST(TestUtil, SilentLoggerConstructsAndLogsWithoutThrowing) {
    auto Logger = TestUtil::MakeSilentLogger();
    ASSERT_NE(Logger, nullptr);
    testing::internal::CaptureStdout();
    Logger->Log("this must not be printed", 5);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(TestUtil, FindFreePortIsBindableOnLoopback) {
    int Port = TestUtil::FindFreePort();
    EXPECT_GT(Port, 0);
    int Fd = ::socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_GE(Fd, 0);
    sockaddr_in Addr{};
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    Addr.sin_port = htons(Port);
    EXPECT_EQ(::bind(Fd, reinterpret_cast<sockaddr*>(&Addr), sizeof(Addr)), 0);
    ::close(Fd);
}

TEST(TestUtil, LoopbackConfigBindsLoopbackOnlyWithGivenRoot) {
    Config::Config Cfg = TestUtil::MakeLoopbackConfig("/tmp/nes-not-created");
    EXPECT_EQ(Cfg.Host, "127.0.0.1");
    EXPECT_GT(Cfg.PortNumber, 0);
    EXPECT_EQ(Cfg.OutputBasePath, "/tmp/nes-not-created");
    EXPECT_FALSE(std::filesystem::exists("/tmp/nes-not-created"));
}

TEST(TestUtil, ScopedTempOutputRootRedirectsAndCleansUp) {
    std::string Path;
    {
        TestUtil::ScopedTempOutputRoot Root;
        Path = Root.Path();
        EXPECT_TRUE(std::filesystem::is_directory(Path));
        EXPECT_EQ(Util::Storage::GetOutputBasePath(), Path);
        std::ofstream(std::filesystem::path(Path) / "x.txt") << "x";
    }
    EXPECT_FALSE(std::filesystem::exists(Path));
    EXPECT_NE(Util::Storage::GetOutputBasePath(), Path);
}

TEST(TestUtil, UniqueRoots) {
    TestUtil::ScopedTempOutputRoot A;
    std::string PathA = A.Path();
    {
        TestUtil::ScopedTempOutputRoot B;
        EXPECT_NE(PathA, B.Path());
    }
}

class TestUtilFixture : public TestUtil::TempOutputRoot {};

TEST_F(TestUtilFixture, FixtureProvidesRootLoggerAndConfig) {
    EXPECT_EQ(Util::Storage::GetOutputBasePath(), RootPath());
    EXPECT_EQ(Config_.OutputBasePath, RootPath());
    ASSERT_NE(Logger_, nullptr);
    std::string Dir = Util::Storage::CreateDirectories("alice", "Renders/a");
    EXPECT_EQ(Dir.rfind(RootPath(), 0), 0u);
    EXPECT_TRUE(std::filesystem::is_directory(Dir));
}

TEST(TestUtil, JsonHelpers) {
    auto Req = TestUtil::MakeRequestWithId("SimulationID", 3, {{"Name", "n"}});
    EXPECT_EQ(Req["SimulationID"], 3);
    EXPECT_EQ(Req["Name"], "n");
    EXPECT_EQ(TestUtil::ParseResponse(TestUtil::Dump(Req)), Req);
    EXPECT_TRUE(TestUtil::ParseResponse("{not json").is_discarded());
    EXPECT_TRUE(TestUtil::ResponseOk(TestUtil::ParseResponse("{\"StatusCode\":0}")));
    EXPECT_FALSE(TestUtil::ResponseOk(TestUtil::ParseResponse("{\"StatusCode\":1}")));
    EXPECT_FALSE(TestUtil::ResponseOk(TestUtil::ParseResponse("[]")));
}
