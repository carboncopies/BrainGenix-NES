//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Test-only harness for the Simulator RPC interfaces (dev_frame NT-11).
                 One process-wide RPCManager on an ephemeral 127.0.0.1 port, a simulation
                 registry, and one GeometryRPCInterface and one ModelRPCInterface wired to
                 them exactly as production does. Handlers are then called directly with
                 JSON strings. The manager is only needed because the interface
                 constructors register their routes on it; it is torn down once, after the
                 last test (RPCManager's destructor hangs unless SetupCallback was called,
                 dev_frame #105, so a callback target is set). Header-only, test binary only.
*/

#pragma once

#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <BG/Common/Logger/Logger.h>

#include <Config/Config.h>
#include <RPC/RPCManager.h>
#include <Simulator/RPC/GeometryRPCInterface.h>
#include <Simulator/RPC/ModelRPCInterface.h>
#include <Simulator/Structs/RecordingElectrode.h>  // complete type for ~Simulation
#include <Simulator/Structs/Simulation.h>
#include <TestUtil/TestUtil.h>
#include <Util/SafeContainers.h>

namespace BG {
namespace NES {
namespace TestUtil {

class RouteHarness {
public:
    static RouteHarness& Get() {
        if (!Instance_) Instance_ = std::make_unique<RouteHarness>();
        return *Instance_;
    }
    static void Release() { Instance_.reset(); }

    RouteHarness() {
        Root_ = std::make_unique<ScopedTempOutputRoot>();
        Logger_ = MakeSilentLogger();
        Config_ = MakeLoopbackConfig(Root_->Path());
        Manager_ = std::make_unique<API::RPCManager>(&Config_, Logger_.get());
        Manager_->SetupCallback(nlohmann::json({{"CallbackHost", "127.0.0.1"}, {"CallbackPort", Config_.PortNumber}}).dump());
        Geometry_ = std::make_unique<Simulator::GeometryRPCInterface>(Logger_.get(), &Registry_, Manager_.get());
        Model_ = std::make_unique<Simulator::ModelRPCInterface>(Logger_.get(), &Registry_, Manager_.get());
    }
    ~RouteHarness() {
        Model_.reset();
        Geometry_.reset();
        Manager_.reset();
    }

    /** Adds a fresh seeded simulation and returns its ID (IDs are never reused). */
    int NewSim(int Seed = 1234) {
        auto Sim = std::make_unique<Simulator::Simulation>(Logger_.get());
        Sim->SetRandomSeed(Seed);
        return static_cast<int>(Registry_.append(std::move(Sim)));
    }
    Simulator::Simulation* Sim(int ID) { return Registry_.read(ID); }

    Simulator::GeometryRPCInterface& Geometry() { return *Geometry_; }
    Simulator::ModelRPCInterface& Model() { return *Model_; }
    API::RPCManager& Manager() { return *Manager_; }
    BG::Common::Logger::LoggingSystem* Logger() { return Logger_.get(); }

private:
    static inline std::unique_ptr<RouteHarness> Instance_;

    std::unique_ptr<ScopedTempOutputRoot> Root_;
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger_;
    Config::Config Config_;
    ConcurrentUniquePtrRegistry<Simulator::Simulation> Registry_;
    std::unique_ptr<API::RPCManager> Manager_;
    std::unique_ptr<Simulator::GeometryRPCInterface> Geometry_;
    std::unique_ptr<Simulator::ModelRPCInterface> Model_;
};

namespace detail {
class RouteHarnessEnvironment : public ::testing::Environment {
public:
    void TearDown() override { RouteHarness::Release(); }
};
inline ::testing::Environment* const RouteHarnessEnv = ::testing::AddGlobalTestEnvironment(new RouteHarnessEnvironment);
}  // namespace detail

}; // namespace TestUtil
}; // namespace NES
}; // namespace BG
