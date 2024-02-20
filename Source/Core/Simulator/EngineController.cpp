#include <Simulator/EngineController.h>



namespace BG {
namespace NES {
namespace Simulator {


void SimulationEngineThread(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, std::atomic<bool>* _StopThreads) {
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    assert(_RenderPool != nullptr);
    assert(_StopThreads != nullptr);

    // Set thread Name
    pthread_setname_np(pthread_self(), std::string("Simulation Engine Thread " + std::to_string(_Sim->ID)).c_str());

    // Log Init message
    _Logger->Log("Starting Simulation Updater Thread", 3);

    // Setup Simulation Engine
    Engine SE;

    // Enter into loop until thread should stop
    while (!(*_StopThreads)) {

        if (_Sim->WorkRequested) {
            _Logger->Log("Simulation Work Requested, Identifiying Task", 2);
            _Sim->IsProcessing = true;

            if (_Sim->CurrentTask == SIMULATION_RESET) {
                _Logger->Log("Worker Performing Simulation Reset For Simulation " + std::to_string(_Sim->ID), 4);
                SE.Reset(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_RUNFOR) {
                _Logger->Log("Worker Performing Simulation RunFor For Simulation " + std::to_string(_Sim->ID), 4);
                SE.RunFor(_Sim);
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_VSDA) {
                _Logger->Log("Worker Performing Simulation VSDA EM Call For Simulation " + std::to_string(_Sim->ID), 4);
                _Sim->IsRendering = true;
                _RenderPool->QueueRenderOperation(_Sim);
                while (_Sim->IsRendering) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
                }
                _Sim->VSDAData_.State_ = VSDA_RENDER_DONE;
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_VISUALIZATION) {
                _Logger->Log("Worker Performing Simulation Visualization Call For Simulation " + std::to_string(_Sim->ID), 4);
                _Sim->IsRendering = true;
                _VisualizerPool->QueueRenderOperation(_Sim);
                while (_Sim->IsRendering) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
                }
                _Sim->VisualizerParams.State = VISUALIZER_DONE;
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else if (_Sim->CurrentTask == SIMULATION_CALCIUM) {
                _Logger->Log("Worker Performing Simulation VSDA Calcium Call For Simulation " + std::to_string(_Sim->ID), 4);
                _Sim->IsRendering = true;
                _RenderPool->QueueRenderOperation(_Sim);
                while (_Sim->IsRendering) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
                }
                _Sim->VSDAData_.State_ = VSDA_RENDER_DONE;
                _Sim->CurrentTask = SIMULATION_NONE;
                _Sim->WorkRequested = false;
            } else {
                _Logger->Log("Unknown Simulation Work Task Enum, Did You Add Something And Forget To Put It Into The EngineController.cpp?", 10);
            }

            _Sim->IsProcessing = false;
            _Logger->Log("Worker Completed Work On Simulation " + std::to_string(_Sim->ID), 4);
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