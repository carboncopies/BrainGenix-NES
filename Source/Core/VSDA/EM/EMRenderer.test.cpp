#include <gtest/gtest.h>
#include <VSDA/EM/EMRenderer.h>
#include <VSDA/EM/EMRenderer.cpp> // For getTotalSystemMemory if needed

using namespace BG::NES::Simulator::VSDA;

// Mock or minimal stub classes for dependencies
class DummyLogger : public BG::Common::Logger::LoggingSystem {
public:
    void Log(const std::string&, int) override {}
};

class DummyImageProcessorPool : public ImageProcessorPool {
public:
    std::atomic<double> TotalConsumedMemory_MB{0};
};

class DummyArrayGeneratorPool : public VoxelArrayGenerator::ArrayGeneratorPool {};

TEST(EMRendererTest, ReturnsFalseIfNotRequested) {
    Config::Config config;
    DummyLogger logger;
    Simulation simulation;
    DummyImageProcessorPool imageProcessorPool;
    DummyArrayGeneratorPool generatorPool;

    simulation.VSDAData_.State_ = VSDA_RENDER_DONE; // Not VSDA_RENDER_REQUESTED

    bool result = ExecuteSubRenderOperations(&config, &logger, &simulation, &imageProcessorPool, &generatorPool);

    EXPECT_FALSE(result);
}