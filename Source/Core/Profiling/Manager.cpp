//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/RPCRoutes/EM.h>
#include <Simulator/Manager.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/EngineController.h>

namespace BG {
namespace NES {
namespace Profiling {


int Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, Simulator::Manager* _SimManager, Simulator::VSDA::RenderPool* _RenderPool, API::Manager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);


    std::vector<std::unique_ptr<Simulator::Simulation>>* Simulations = _SimManager->GetSimulationVectorPtr();

    // Profiling Stuff
    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_1K_SPHERES) {

        _Logger->Log("Running 1K Sphere Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, &StopThreads);


        // Create 1k spheres
        for (unsigned int i = 0; i < 10000; i++) {

            std::string Name = "Sphere " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));


            // -- Create Sphere -- //
            Simulator::Geometries::Sphere S;
            S.Name = Name;
            S.Radius_um = Radius_um;
            S.Center_um.x = CenterPosX;
            S.Center_um.y = CenterPosY;
            S.Center_um.z = CenterPosZ;
            Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
            S.ID = ThisSimulation->Collection.Geometries.size();
            ThisSimulation->Collection.Geometries.push_back(S);


            // -- Create Compartments -- //
            Simulator::Compartments::BS C;
            C.ShapeID = S.ID;
            C.ID  = ThisSimulation->BSCompartments.size();
            ThisSimulation->BSCompartments.push_back(C);

            // _Logger->Log("Created Sphere " + std::to_string(i), 3);


        }


        // -- Setup Rendering Operation -- //
        Simulator::MicroscopeParameters Params;
        Params.VoxelResolution_um = 0.1;
        Params.ImageWidth_px = 512;
        Params.ImageHeight_px = 512;
        Params.ScanRegionOverlap_percent = 0;
        Params.SliceThickness_um = 0.1;
        Params.NumPixelsPerVoxel_px = 2;
        
        Simulator::ScanRegion Region;
        Region.Point1X_um = 0;
        Region.Point1Y_um = 0;
        Region.Point1Z_um = 0;
        Region.Point2X_um = 200;
        Region.Point2Y_um = 200;
        Region.Point2Z_um = 200;
        int RegionID;

        VSDA_EM_Initialize(_Logger, Sim);
        VSDA_EM_SetupMicroscope(_Logger, Sim, Params);
        VSDA_EM_DefineScanRegion(_Logger, Sim, Region, &RegionID);
        VSDA_EM_QueueRenderOperation(_Logger, Sim, RegionID);

        while (Sim->VSDAData_.CurrentSlice_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


    }


    return 0;

}

}; // Close Namespace Profiling
}; // Close Namespace NES
}; // Close Namespace BG
