#include <Main.h>



void LogLogo(BG::Common::Logger::LoggingSystem* Logger) {

    // Log BG Logo Text
    Logger->Log("Starting BrainGenix-NES Instance", 2);
    Logger->Log("", 5);
    Logger->Log("---------------------------------------------------------------------------", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╗ ██████╗  █████╗ ██╗███╗   ██╗\x1b[38;2;130;68;208m ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗", 5);
    Logger->Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║\x1b[38;2;130;68;208m██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╔╝██████╔╝███████║██║██╔██╗ ██║\x1b[38;2;130;68;208m██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ", 5);
    Logger->Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║\x1b[38;2;130;68;208m██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╔╝██║  ██║██║  ██║██║██║ ╚████║\x1b[38;2;130;68;208m╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗", 5);
    Logger->Log("\x1b[38;2;0;128;55m╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝\x1b[38;2;130;68;208m ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝", 5);
    Logger->Log("---------------------------------------------------------------------------", 5);
    Logger->Log("", 5);
    Logger->Log("    +-------------------------------------------------------+", 4);
    Logger->Log("    |     BrainGenix Platform For Whole Brain Emulation     |", 4);
    Logger->Log("    +-------------------------------------------------------+", 4);
    Logger->Log("", 4);


}


int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup Logging System
    BG::Common::Logger::LoggingSystem Logger; // NOTE THAT THIS IS TEMPORARY; IT NEEDS TO BE EVENTUALLY FED
    // CONFIGURATION INFO FROM THE CONFIG SUBSYS - CURRENTLY IT IS OPERATING IN UNIT-TEST MODE!!!!

    Logger.SetKeepVectorLogs(false); // this causes a segfault for some reason

    // Setup API Server
    BG::NES::API::Manager APIManager(&SystemConfiguration, &Logger);

    // Setup Renderer
    BG::NES::Renderer::Interface RenderingInterface(&Logger);
    if (!RenderingInterface.Initialize(NumArguments,  ArgumentValues)) { 
        Logger.Log("Error During Renderer Initialization, Aborting", 10);
        return -1;
    }

    // Setup Simulator (Adding the routes here - will need a proper way to do this later on, but works for now)
    BG::NES::Simulator::Manager SimulationManager(&Logger, &SystemConfiguration, &APIManager, &RenderingInterface);
    BG::NES::Simulator::VSDA::RPCInterface VSDA_RPCInterface(&Logger, &APIManager, &RenderingInterface, SimulationManager.GetSimulationVectorPtr());



    LogLogo(&Logger);


    
    // block forever while servers are running
    // while (true) {}
    
    // Run the Renderer
    while (true) {
        if (!RenderingInterface.DrawFrame()) {
            break;
        }
        // break; // stop after one frame
    }

    return 0;


}


