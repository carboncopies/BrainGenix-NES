#include <Simulator/EngineController.h>



namespace BG {
namespace NES {
namespace Simulator {


void SimulationEngineThread(Simulation* _Sim, std::atomic<bool>* _StopThreads) {

    // Log Init message
    std::cout<<"[Info] Starting Simulation Updater Thread\n";

    // Setup Engine
    Engine SimulationEngine();

    // Enter into loop until thread should stop
    while (!_StopThreads) {

        if (_Sim->WorkRequested) {
            _Sim->IsProcessing = true;

            if (_Sim->CurrentTask == SIMULATION_RESET) {
                std::cout<<"[Info] Worker Performing Simulation Reset For Simulation "<<_Sim->ID<<std::endl;
                SimulationEngine.ResetSimulation(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;

            } else if (_Sim->CurrentTask == SIMULATION_RUNFOR) {
                std::cout<<"[Info] Worker Performing Simulation RunFor For Simulation "<<_Sim->ID<<std::endl;
                SimulationEngine.RunSimulationFor(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            }

            _Sim->IsProcessing = false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
        }

    }


}



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG