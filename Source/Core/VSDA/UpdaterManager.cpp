#include <VSDA/UpdaterManager.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



bool ExecuteRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, BG::NES::Renderer::Interface* _Renderer) {

    // Check that the simulation has been initialized and everything is ready to have work done
    if (_Simulation->VSDAData_.State_ != VSDA_RENDER_REQUESTED) {
        return false;
    }

    _Logger->Log("Executing Render Job For Requested Simulation", 4);

    // BoundingBox BB = BoundingBox();
    // BB.bb_point1[0] = -1.;
    // BB.bb_point1[1] = -1.;
    // BB.bb_point1[2] = -1.;

    // BB.bb_point2[0] = 9.;
    // BB.bb_point2[1] = 6.;
    // BB.bb_point2[2] = 6.;

    
    // float VoxelSize = _Simulation->VSDAData_.Array_->GetResolution();
    // VoxelArray Arr(BB, VoxelSize);



    // MicroscopeParameters MParams;
    // MParams.VoxelResolution_um = VoxelSize;


    // Create Voxel Array
    ScanRegion RequestedRegion = _Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];
    _Simulation->VSDAData_.Array_ = std::make_unique<VoxelArray>(RequestedRegion, _Simulation->VSDAData_.Params_.VoxelResolution_um);
    CreateVoxelArrayFromSimulation(_Logger, _Simulation, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get());

    // for (unsigned int i = 0; i < Arr.GetZ(); i++) {
    //     Arr.SetVoxel(i, i, i, FILLED);
    // }


    // Clear Scene In Preperation For Rendering
    _Renderer->ResetScene();
    for (unsigned int i = 0; i < _Simulation->VSDAData_.Array_.get()->GetZ(); i++) {
        RenderSliceFromArray(_Logger, _Renderer, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get(), i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay so our mutex is easier to get by another thread if needed
    }

    return true;

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
