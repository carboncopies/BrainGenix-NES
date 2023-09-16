#include <Main.h>


int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup Logging System
    BG::Common::Logger::LoggingSystem Logger(true); // NOTE THAT THIS IS TEMPORARY; IT NEEDS TO BE EVENTUALLY FED
    // CONFIGURATION INFO FROM THE CONFIG SUBSYS - CURRENTLY IT IS OPERATING IN UNIT-TEST MODE!!!!

    // Setup API Server
    BG::NES::API::Manager APIManager(&SystemConfiguration);

    // Setup Simulator
    BG::NES::Simulator::Manager SimulationManager(&SystemConfiguration, &APIManager);

    // Setup Renderer
    BG::NES::Renderer::Interface RenderingInterface(&Logger);
    RenderingInterface.Initialize(false); // NOTE THAT THE 'true' PARAM INDICATES THAT
    // THE DEBUG WINDOW IS ENABLED. THIS SHOULD BE EVENTUALLY HANDLED BY THE CONFIG SUBSYS (FROM FILE/COMMAND LINE ARG)


    
    // block forever while servers are running
    while (true) {}

    return 0;


}


