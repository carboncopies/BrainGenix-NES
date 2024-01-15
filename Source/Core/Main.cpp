#include <Main.h>


/**
 * @brief The main entry point for the BrainGenix-NES Neuron Emulation System.
 * 
 * This function initializes the system components, including configuration, logging, API server,
 * rendering pool, and simulator. It also prints the BrainGenix logo to the console and runs
 * the main loop to keep the servers running.
 * 
 * @param NumArguments The number of command-line arguments.
 * @param ArgumentValues An array containing the command-line arguments.
 * @return int The exit code of the program.
 */
int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup Logging System
    BG::Common::Logger::LoggingSystem Logger;
    Logger.SetKeepVectorLogs(false);

    // Setup API Server
    BG::NES::API::Manager APIManager(&SystemConfiguration, &Logger);

    // Setup RenderPool
    BG::NES::Simulator::VSDA::RenderPool RenderPool(&Logger, false);

    // Setup Simulator (Adding the routes here)
    BG::NES::Simulator::Manager SimulationManager(&Logger, &SystemConfiguration, &RenderPool, &APIManager);
    BG::NES::Simulator::VSDA::RPCInterface VSDA_RPCInterface(&Logger, &APIManager, SimulationManager.GetSimulationVectorPtr());

    // Print ASCII BrainGenix Logo To Console
    BG::NES::Util::LogLogo(&Logger);

    // Block forever while servers are running
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
