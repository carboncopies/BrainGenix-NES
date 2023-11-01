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
    if (!RenderingInterface.Initialize(NumArguments,  ArgumentValues)) { 
        Logger.Log("Error During Renderer Initialization, Aborting", 10);
        return -1;
    }

    
    // block forever while servers are running
    // while (true) {}
    
    // draw a thousand frames
    for (unsigned int i = 0; i < 1000; i++) {
        RenderingInterface.DrawFrame();
    }

    return 0;


}


