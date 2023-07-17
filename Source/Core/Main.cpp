#include <Main.h>


int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup API Server
    BG::NES::API::Manager APIManager(&SystemConfiguration);

    // Setup Simulator
    BG::NES::Simulator::Manager SimulationManager(&SystemConfiguration, &APIManager);

    
    // block forever while servers are running
    while (true) {}

    return 0;


}

