//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/RPCRoutes/EM.h>
#include <VSDA/RPCRoutes/Ca.h>
#include <Simulator/RPC/SimulationRPCInterface.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/EngineController.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>

#include <Profiling/ProfilingManager.h>

namespace BG {
namespace NES {
namespace Profiling {


int Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, Simulator::SimulationRPCInterface* _SimManager, Simulator::VSDA::RenderPool* _RenderPool, Simulator::VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);


    // Start Timer
    std::chrono::time_point Start = std::chrono::high_resolution_clock::now();


    std::vector<std::unique_ptr<Simulator::Simulation>>* Simulations = _SimManager->GetSimulationVectorPtr();

    // Profiling Stuff
    if (_Config->ProfilingStatus_ == Config::PROFILE_CALCIUM_END_TO_END_TEST_1) {

        _Logger->Log("Running Calcium Imaging Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 1k spheres
        for (unsigned int i = 0; i < 10000; i++) {

            std::string Name = "Sphere " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/8.));
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
        ::BG::NES::VSDA::Calcium::CaMicroscopeParameters Params;
        Params.VoxelResolution_um = 0.3;
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
        Params.ScanRegionOverlap_percent = 0;
        Params.NumVoxelsPerSlice = 4;
        Params.NumPixelsPerVoxel_px = 2;
        
        Simulator::ScanRegion Region;
        Region.Point1X_um = 0;
        Region.Point1Y_um = 0;
        Region.Point1Z_um = 0;
        Region.Point2X_um = 200;
        Region.Point2Y_um = 200;
        Region.Point2Z_um = 200;
        int RegionID;

        ::BG::NES::VSDA::Calcium::VSDA_CA_Initialize(_Logger, Sim);
        ::BG::NES::VSDA::Calcium::VSDA_CA_SetupMicroscope(_Logger, Sim, Params);
        ::BG::NES::VSDA::Calcium::VSDA_CA_DefineScanRegion(_Logger, Sim, Region, &RegionID);
        ::BG::NES::VSDA::Calcium::VSDA_CA_QueueRenderOperation(_Logger, Sim, RegionID);

        while (Sim->CaData_.State_ != ::BG::NES::VSDA::Calcium::CA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Join Threads
        StopThreads = true;
        SimThread.join();

    }


    else if (_Config->ProfilingStatus_ == Config::PROFILE_NEW_API_TEST) {
        _Logger->Log("Running New API Test", 6);

        // Parse Request
        std::string _JSONRequest(R"(
  [
    {
      "ReqID": 237654,
      "SimID": 3,
      "AddBSNeuron": {
        "Name": "1",
        "SomaID": 12,
        "AxonID": 14
      }
    }
  ]
)");
        _Logger->Log(_JSONRequest, 3);
        nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

        _Logger->Log("DEBUG ---> Testing how to pull out lower level components of a JSON object...", 3);

        // For each request in the JSON list:

        int ReqID = -1;
        int SimID = -1;
        for (const auto & req : RequestJSON) {
            for (const auto & [req_key, req_value]: req.items()) {
                if (req_key == "ReqID") {
                    ReqID = req_value.template get<int>();
                } else if (req_key == "SimID") {
                    SimID = req_value.template get<int>();
                } else {
                    std::string ReqFunc = req_key;
                    // Typically would call a specific handler from here, but let's just keep parsing.
                    std::cout << "Request function found: " << ReqFunc << '\n';
                    for (const auto & [key, value]: req_value.items()) {
                        std::cout << "Found parameter: " << key << " with value: " << value << '\n';
                    }
                }
            }
        }
    }

    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_10K_SPHERES) {

        _Logger->Log("Running 100K Sphere Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 1k spheres
        for (unsigned int i = 0; i < 100000; i++) {

            std::string Name = "Sphere " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/8.));
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
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
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

        while (Sim->VSDAData_.State_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Join Threads
        StopThreads = true;
        SimThread.join();

    }

    // Profiling Stuff
    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_10K_CYLINDERS) {

        _Logger->Log("Running 10K Cylinder Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 10k cylinders
        for (unsigned int i = 0; i < 10000; i++) {

            std::string Name = "Cylinder " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float Radius2_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPos2X = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPos2Y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPos2Z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));

            // -- Create Sphere -- //
            Simulator::Geometries::Cylinder S;
            S.Name = Name;
            S.End0Radius_um = Radius_um;
            S.End1Radius_um = Radius_um;
            S.End0Pos_um = Simulator::Geometries::Vec3D(CenterPosX, CenterPosY, CenterPosZ);
            S.End1Pos_um = Simulator::Geometries::Vec3D(CenterPos2X, CenterPos2Y, CenterPos2Z);
            Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
            S.ID = ThisSimulation->Collection.Geometries.size();
            ThisSimulation->Collection.Geometries.push_back(S);


            // -- Create Compartments -- //
            Simulator::Compartments::BS C;
            C.ShapeID = S.ID;
            C.ID  = ThisSimulation->BSCompartments.size();
            ThisSimulation->BSCompartments.push_back(C);


        }


        // -- Setup Rendering Operation -- //
        Simulator::MicroscopeParameters Params;
        Params.VoxelResolution_um = 0.1;
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
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

        while (Sim->VSDAData_.State_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


        // Join Threads
        StopThreads = true;
        SimThread.join();

    }


    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_10K_BOXES) {

        _Logger->Log("Running 10K Box Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 10k Box
        for (unsigned int i = 0; i < 10000; i++) {

            std::string Name = "Box " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float Radius2_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/200));
            float SizeX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));
            float SizeY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));
            float SizeZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));

            // -- Create Box -- //
            Simulator::Geometries::Box S;
            S.Name = Name;
            S.Center_um = Simulator::Geometries::Vec3D(CenterPosX, CenterPosY, CenterPosZ);
            S.Dims_um = Simulator::Geometries::Vec3D(SizeX, SizeY, SizeZ);
            Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
            S.ID = ThisSimulation->Collection.Geometries.size();
            ThisSimulation->Collection.Geometries.push_back(S);


            // -- Create Compartments -- //
            Simulator::Compartments::BS C;
            C.ShapeID = S.ID;
            C.ID  = ThisSimulation->BSCompartments.size();
            ThisSimulation->BSCompartments.push_back(C);


        }


        // -- Setup Rendering Operation -- //
        Simulator::MicroscopeParameters Params;
        Params.VoxelResolution_um = 0.1;
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
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

        while (Sim->VSDAData_.State_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


        // Join Threads
        StopThreads = true;
        SimThread.join();


    }



    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_500K_SHAPES) {

        _Logger->Log("Running 500K shape Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 1k spheres
        for (unsigned int i = 0; i < 250000; i++) {

            std::string Name = "Sphere " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/8.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));


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


        }



        // // Create 100k cylinders
        // for (unsigned int i = 0; i < 100000; i++) {

        //     std::string Name = "Cylinder " + std::to_string(i);
        //     float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
        //     float Radius2_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
        //     float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
        //     float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
        //     float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
        //     float CenterPos2X = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
        //     float CenterPos2Y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
        //     float CenterPos2Z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));

        //     // -- Create Sphere -- //
        //     Simulator::Geometries::Cylinder S;
        //     S.Name = Name;
        //     S.End0Radius_um = Radius_um;
        //     S.End1Radius_um = Radius_um;
        //     S.End0Pos_um = Simulator::Geometries::Vec3D(CenterPosX, CenterPosY, CenterPosZ);
        //     S.End1Pos_um = Simulator::Geometries::Vec3D(CenterPos2X, CenterPos2Y, CenterPos2Z);
        //     Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
        //     S.ID = ThisSimulation->Collection.Geometries.size();
        //     ThisSimulation->Collection.Geometries.push_back(S);


        //     // -- Create Compartments -- //
        //     Simulator::Compartments::BS C;
        //     C.ShapeID = S.ID;
        //     C.ID  = ThisSimulation->BSCompartments.size();
        //     ThisSimulation->BSCompartments.push_back(C);


        // }



        // Create 100k Box
        for (unsigned int i = 0; i < 250000; i++) {

            std::string Name = "Box " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float Radius2_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float SizeX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));
            float SizeY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));
            float SizeZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/20));

            // -- Create Box -- //
            Simulator::Geometries::Box S;
            S.Name = Name;
            S.Center_um = Simulator::Geometries::Vec3D(CenterPosX, CenterPosY, CenterPosZ);
            S.Dims_um = Simulator::Geometries::Vec3D(SizeX, SizeY, SizeZ);
            Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
            S.ID = ThisSimulation->Collection.Geometries.size();
            ThisSimulation->Collection.Geometries.push_back(S);


            // -- Create Compartments -- //
            Simulator::Compartments::BS C;
            C.ShapeID = S.ID;
            C.ID  = ThisSimulation->BSCompartments.size();
            ThisSimulation->BSCompartments.push_back(C);


        }


        // -- Setup Rendering Operation -- //
        Simulator::MicroscopeParameters Params;
        Params.VoxelResolution_um = 0.1;
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
        Params.ScanRegionOverlap_percent = 0;
        Params.SliceThickness_um = 0.1;
        Params.NumPixelsPerVoxel_px = 1;
        
        Simulator::ScanRegion Region;
        Region.Point1X_um = 0;
        Region.Point1Y_um = 0;
        Region.Point1Z_um = 0;
        Region.Point2X_um = 1000;
        Region.Point2Y_um = 1000;
        Region.Point2Z_um = 1000;
        int RegionID;

        VSDA_EM_Initialize(_Logger, Sim);
        VSDA_EM_SetupMicroscope(_Logger, Sim, Params);
        VSDA_EM_DefineScanRegion(_Logger, Sim, Region, &RegionID);
        VSDA_EM_QueueRenderOperation(_Logger, Sim, RegionID);

        while (Sim->VSDAData_.State_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Join Threads
        StopThreads = true;
        SimThread.join();

    }


    if (_Config->ProfilingStatus_ == Config::PROFILE_VOXEL_ARRAY_GENERATOR_2000K_SHAPES) {

        _Logger->Log("Running 500K shape Profiling Test", 6);

        // Create A Simulation
        Simulations->push_back(std::make_unique<Simulator::Simulation>(_Logger));
        int SimID = Simulations->size()-1;
        Simulator::Simulation* Sim = (*Simulations)[SimID].get();
        Sim->Name = "Profiling Sim";
        Sim->ID = SimID;
        Sim->CurrentTask = Simulator::SIMULATION_NONE;

        // Start Thread
        std::atomic_bool StopThreads = false;
        std::thread SimThread(&Simulator::SimulationEngineThread, _Logger, Sim, _RenderPool, _VisualizerPool, &StopThreads);


        // Create 100k Boxes
        for (unsigned int i = 0; i < 1000000; i++) {

            std::string Name = "Box " + std::to_string(i);
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float SizeX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10));
            float SizeY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10));
            float SizeZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10));

            // -- Create Box -- //
            Simulator::Geometries::Box S;
            S.Name = Name;
            S.Center_um = Simulator::Geometries::Vec3D(CenterPosX, CenterPosY, CenterPosZ);
            S.Dims_um = Simulator::Geometries::Vec3D(SizeX, SizeY, SizeZ);
            Simulator::Simulation* ThisSimulation = (*Simulations)[0].get();
            S.ID = ThisSimulation->Collection.Geometries.size();
            ThisSimulation->Collection.Geometries.push_back(S);


            // -- Create Compartments -- //
            Simulator::Compartments::BS C;
            C.ShapeID = S.ID;
            C.ID  = ThisSimulation->BSCompartments.size();
            ThisSimulation->BSCompartments.push_back(C);


        }



        // Create 100k spheres
        for (unsigned int i = 0; i < 1000000; i++) {

            std::string Name = "Sphere " + std::to_string(i);
            float Radius_um = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.));
            float CenterPosX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
            float CenterPosZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));


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


        }



        // -- Setup Rendering Operation -- //
        Simulator::MicroscopeParameters Params;
        Params.VoxelResolution_um = 0.05;
        Params.ImageWidth_px = 1500;
        Params.ImageHeight_px = 1500;
        Params.ScanRegionOverlap_percent = 0;
        Params.SliceThickness_um = 0.1;
        Params.NumPixelsPerVoxel_px = 1;
        
        Simulator::ScanRegion Region;
        Region.Point1X_um = 0;
        Region.Point1Y_um = 0;
        Region.Point1Z_um = 0;
        Region.Point2X_um = 1000;
        Region.Point2Y_um = 1000;
        Region.Point2Z_um = 1000;
        int RegionID;

        VSDA_EM_Initialize(_Logger, Sim);
        VSDA_EM_SetupMicroscope(_Logger, Sim, Params);
        VSDA_EM_DefineScanRegion(_Logger, Sim, Region, &RegionID);
        VSDA_EM_QueueRenderOperation(_Logger, Sim, RegionID);

        while (Sim->VSDAData_.State_ != Simulator::VSDA_RENDER_DONE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Join Threads
        StopThreads = true;
        SimThread.join();

    }



    // Mesure Time, Exit
    double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
    _Logger->Log("Done Profiling, Test Completed In " + std::to_string(Duration_ms) + "ms", 5);



    return 0;

}

}; // Close Namespace Profiling
}; // Close Namespace NES
}; // Close Namespace BG
