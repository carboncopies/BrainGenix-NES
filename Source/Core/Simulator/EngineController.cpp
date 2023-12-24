#include <Simulator/EngineController.h>



namespace BG {
namespace NES {
namespace Simulator {


void SimulationEngineThread(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, BG::NES::Renderer::Interface* _Renderer, std::atomic<bool>* _StopThreads) {

    // Log Init message
    std::cout<<"[Info] Starting Simulation Updater Thread\n";

    // Setup Simulation Engine
    Engine SE;

    // Enter into loop until thread should stop
    while (!(*_StopThreads)) {

        if (_Sim->WorkRequested) {
            _Logger->Log("Simulation Work Requested, Identifiying Task", 2);
            _Sim->IsProcessing = true;

            if (_Sim->CurrentTask == SIMULATION_RESET) {
                std::cout<<"[Info] Worker Performing Simulation Reset For Simulation "<<_Sim->ID<<std::endl;
                SE.Reset(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_RUNFOR) {
                std::cout<<"[Info] Worker Performing Simulation RunFor For Simulation "<<_Sim->ID<<std::endl;
                SE.RunFor(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_VSDA) {
                std::cout<<"[Info] Worker Performing Simulation VSDA Call For Simulation "<<_Sim->ID<<std::endl;
                VSDA::ExecuteRenderOperations(_Logger, _Sim, _Renderer);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            }
            _Sim->IsProcessing = false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
        }

    }

    // Log Shutdown Message
    _Logger->Log("Simulation Engine Thread Exiting", 4);

}



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG