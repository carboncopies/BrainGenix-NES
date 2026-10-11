//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for ProcessIgneousPipeline (dev_frame NT-14). The "virtualenv" is a temp directory whose
    bin/python is a /bin/sh stub, so no Python or Igneous is needed. Failure logging is checked on stdout through a
    logger with output enabled.
*/

#include <sys/stat.h>

#include <string>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <VSDA/EM/NeuroglancerConversionPool/IgneousPipeline.h>

using namespace BG::NES;
using namespace BG::NES::TestUtil;

namespace {

struct IgneousFixture : TempOutputRoot {
    std::string Venv() const { return RootPath() + "/venv"; }
    std::string Out() const { return RootPath() + "/meshes/out"; }
    std::string Dataset() const { return RootPath() + "/dataset"; }
    bool Run(const std::string& VenvDir, BG::Common::Logger::LoggingSystem* L = nullptr) {
        return ProcessIgneousPipeline(L ? L : Logger_.get(), Dataset(), Out(), true, 0, 4, VenvDir);
    }
};

} // namespace

TEST_F(IgneousFixture, ExitZeroReturnsTrue) {
    MakeStubVenv(Venv(), "exit 0");
    EXPECT_TRUE(Run(Venv()));
}

TEST_F(IgneousFixture, NonZeroExitReturnsFalse) {
    MakeStubVenv(Venv(), "echo boom; exit 1");
    EXPECT_FALSE(Run(Venv()));
}

TEST_F(IgneousFixture, AnyNonZeroExitCodeIsAFailure) {
    MakeStubVenv(Venv(), "exit 42");
    EXPECT_FALSE(Run(Venv()));
}

TEST_F(IgneousFixture, MissingInterpreterReturnsFalse) {
    std::filesystem::create_directories(Venv() + "/bin");  // venv exists, bin/python does not
    EXPECT_FALSE(Run(Venv()));
}

TEST_F(IgneousFixture, MissingVenvDirectoryReturnsFalse) {
    EXPECT_FALSE(Run(RootPath() + "/no-such-venv"));
}

TEST_F(IgneousFixture, NonExecutableInterpreterReturnsFalse) {
    MakeStubVenv(Venv(), "exit 0");
    ::chmod((Venv() + "/bin/python").c_str(), 0644);
    EXPECT_FALSE(Run(Venv()));
}

TEST_F(IgneousFixture, OutputDirectoryIsCreatedWorldWritableEvenWhenTheScriptFails) {
    MakeStubVenv(Venv(), "exit 3");
    EXPECT_FALSE(Run(Venv()));
    struct stat St{};
    ASSERT_EQ(::stat(Out().c_str(), &St), 0);
    EXPECT_TRUE(S_ISDIR(St.st_mode));
    EXPECT_EQ(St.st_mode & 0777, 0777u);
}

TEST_F(IgneousFixture, ScriptIsRunWithTheVenvInterpreterTheDatasetPathAndUmaskZero) {
    const std::string Args = RootPath() + "/args.txt";
    MakeStubVenv(Venv(), "echo \"$@\" > '" + Args + "'; umask >> '" + Args + "'; exit 0");
    ASSERT_TRUE(Run(Venv()));
    std::string Got = ReadFileBytes(Args);
    EXPECT_NE(Got.find("./Python/igneous_local.py --datapath " + Dataset() + " --parallel 60"), std::string::npos) << Got;
    EXPECT_NE(Got.find("0000"), std::string::npos) << Got;
}

TEST_F(IgneousFixture, VenvPathWithSpacesWorks) {
    const std::string Dir = RootPath() + "/my venv dir";
    MakeStubVenv(Dir, "exit 0");
    EXPECT_TRUE(Run(Dir));
}

TEST_F(IgneousFixture, RelativeVenvPathIsResolvedAgainstTheWorkingDirectory) {
    MakeStubVenv(Venv(), "exit 0");
    std::filesystem::path Old = std::filesystem::current_path();
    std::filesystem::current_path(RootPath());
    bool Ok = ProcessIgneousPipeline(Logger_.get(), "dataset", "meshes/rel-out", true, 0, 4, "venv");
    std::filesystem::current_path(Old);
    EXPECT_TRUE(Ok);
    EXPECT_TRUE(std::filesystem::is_directory(RootPath() + "/meshes/rel-out"));
}

TEST_F(IgneousFixture, FailureLogsOnlyTheLastTwentyLinesOfOutputIncludingStderr) {
    std::string Body;
    for (int i = 1; i <= 30; i++) {
        char Buf[64];
        snprintf(Buf, sizeof(Buf), "echo OUT-%03d; ", i);
        Body += Buf;
    }
    Body += "echo ERR-LAST >&2; exit 1";
    MakeStubVenv(Venv(), Body);

    BG::Common::Logger::LoggingSystem Loud(true);
    testing::internal::CaptureStdout();
    bool Ok = Run(Venv(), &Loud);
    std::string Log = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(Ok);
    EXPECT_NE(Log.find("Igneous: ERR-LAST"), std::string::npos) << Log;
    EXPECT_NE(Log.find("Igneous: OUT-030"), std::string::npos);
    EXPECT_NE(Log.find("Igneous: OUT-012"), std::string::npos);  // 19 stdout lines + ERR-LAST = 20
    EXPECT_EQ(Log.find("Igneous: OUT-011"), std::string::npos);
    EXPECT_EQ(Log.find("Igneous: OUT-001"), std::string::npos);
}

TEST_F(IgneousFixture, SuccessDoesNotDumpScriptOutputToTheLog) {
    MakeStubVenv(Venv(), "echo NOISY-OUTPUT; exit 0");
    BG::Common::Logger::LoggingSystem Loud(true);
    testing::internal::CaptureStdout();
    bool Ok = Run(Venv(), &Loud);
    std::string Log = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(Ok);
    EXPECT_EQ(Log.find("NOISY-OUTPUT"), std::string::npos);
}
