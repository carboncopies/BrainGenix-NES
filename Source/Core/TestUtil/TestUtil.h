//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Shared helpers for unit tests (dev_frame NT-1). Header-only, compiled
                 only into the test binary (never into the library). See README.md.
*/

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <BG/Common/Logger/Logger.h>

#include <Config/Config.h>
#include <Util/StoragePaths.h>

namespace BG {
namespace NES {
namespace TestUtil {

/**
 * @brief A LoggingSystem with all output disabled (no console output, no log file).
 */
inline std::unique_ptr<BG::Common::Logger::LoggingSystem> MakeSilentLogger() {
    auto Logger = std::make_unique<BG::Common::Logger::LoggingSystem>(false);
    Logger->SetKeepVectorLogs(false);
    return Logger;
}

/**
 * @brief Ask the kernel for a free TCP port on 127.0.0.1. There is an unavoidable small
 *        window between closing the probe socket and the caller binding the port.
 */
inline int FindFreePort() {
    int Fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (Fd < 0) throw std::runtime_error("TestUtil::FindFreePort: socket() failed");
    sockaddr_in Addr{};
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    Addr.sin_port = 0;
    socklen_t Len = sizeof(Addr);
    if (::bind(Fd, reinterpret_cast<sockaddr*>(&Addr), sizeof(Addr)) != 0 ||
        ::getsockname(Fd, reinterpret_cast<sockaddr*>(&Addr), &Len) != 0) {
        ::close(Fd);
        throw std::runtime_error("TestUtil::FindFreePort: bind/getsockname failed");
    }
    int Port = ntohs(Addr.sin_port);
    ::close(Fd);
    return Port;
}

/**
 * @brief A Config bound to 127.0.0.1 on a free port, with OutputBasePath under the given
 *        directory (empty = a path that is never created). Never binds 0.0.0.0.
 */
inline Config::Config MakeLoopbackConfig(const std::string& OutputBasePath = "") {
    Config::Config Cfg;
    Cfg.Host = "127.0.0.1";
    Cfg.PortNumber = FindFreePort();
    Cfg.MaxVoxelArraySize_ = 256;
    Cfg.VoxelArrayPercentOfSystemMemory_ = 0.01f;
    if (!OutputBasePath.empty()) Cfg.OutputBasePath = OutputBasePath;
    return Cfg;
}

/**
 * @brief RAII: unique temp dir, points Util::Storage at it, restores the previous base path
 *        and removes the dir on destruction. Never writes under /var/lib/BrainGenix/NES.
 */
class ScopedTempOutputRoot {
public:
    ScopedTempOutputRoot() {
        std::string Template = (std::filesystem::temp_directory_path() / "nes-test-XXXXXX").string();
        if (::mkdtemp(Template.data()) == nullptr) throw std::runtime_error("TestUtil: mkdtemp failed");
        Path_ = Template;
        Previous_ = Util::Storage::GetOutputBasePath();
        Util::Storage::SetOutputBasePath(Path_);
    }
    ~ScopedTempOutputRoot() {
        // SetOutputBasePath ignores empty strings and creates its target, so restore only
        // when there was a previous value; it already existed in that case or is the default.
        std::error_code Ec;
        if (!Previous_.empty()) Util::Storage::SetOutputBasePath(Previous_);
        std::filesystem::remove_all(Path_, Ec);
    }
    ScopedTempOutputRoot(const ScopedTempOutputRoot&) = delete;
    ScopedTempOutputRoot& operator=(const ScopedTempOutputRoot&) = delete;

    const std::string& Path() const { return Path_; }

private:
    std::string Path_;
    std::string Previous_;
};

/**
 * @brief gtest fixture: silent logger + temp output root + loopback Config for each test.
 *        Derive from it (or use TEST_F directly).
 */
class TempOutputRoot : public ::testing::Test {
protected:
    void SetUp() override {
        Root_ = std::make_unique<ScopedTempOutputRoot>();
        Logger_ = MakeSilentLogger();
        Config_ = MakeLoopbackConfig(Root_->Path());
    }
    void TearDown() override {
        Logger_.reset();
        Root_.reset();
    }

    const std::string& RootPath() const { return Root_->Path(); }

    std::unique_ptr<ScopedTempOutputRoot> Root_;
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger_;
    Config::Config Config_;
};

/**
 * @brief JSON request helpers. NES requests are JSON objects; responses carry "StatusCode"
 *        (0 = success) per the API conventions.
 */
inline nlohmann::json MakeRequest(const nlohmann::json& Fields = nlohmann::json::object()) {
    return nlohmann::json(Fields);
}

inline nlohmann::json MakeRequestWithId(const std::string& IdKey, int Id, nlohmann::json Fields = nlohmann::json::object()) {
    Fields[IdKey] = Id;
    return Fields;
}

inline std::string Dump(const nlohmann::json& J) { return J.dump(); }

/** Parse a response string; returns discarded() JSON on malformed input instead of throwing. */
inline nlohmann::json ParseResponse(const std::string& Text) {
    return nlohmann::json::parse(Text, nullptr, /*allow_exceptions=*/false);
}

inline bool ResponseOk(const nlohmann::json& Response) {
    return Response.is_object() && Response.contains("StatusCode") && Response["StatusCode"] == 0;
}

}; // namespace TestUtil
}; // namespace NES
}; // namespace BG
