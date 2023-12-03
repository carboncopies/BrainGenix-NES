#include <VSDA/UpdaterManager.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



bool ExecuteRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation) {



}


// UpdaterManager::UpdaterManager(BG::Common::Logger::LoggingSystem* _Logger, API::Manager* _RPCManager, BG::NES::Renderer::Interface* _Renderer, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer) {

//     // Check Preconditions
//     assert(_Logger != nullptr);
//     assert(_RPCManager != nullptr);
//     assert(_Renderer != nullptr);
//     assert(_SimulationsVectorPointer != nullptr);

//     // Copy Parameters To Member Variables
//     Renderer_ = _Renderer;
//     Logger_ = _Logger;
//     SimulationsPtr_ = _SimulationsVectorPointer;


//     // Log Initialization
//     Logger_->Log("Initializing VSDA Subsystem Updater Thread", 4);

//     // Register Callback For CreateSim


// }

// UpdaterManager::~UpdaterManager() {

// }



// bool UpdaterManager::ExecuteRenderOperations() {

//     for (unsigned int i = 0; i < SimulationsPtr_->size(); i++) {

//         Simulation* ThisSimulation = (*SimulationsPtr_)[i].get();

//         if (ThisSimulation->VSDAData_.State_ == VSDA_RENDER_REQUESTED) {

            

//         }

//     }


// }



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
