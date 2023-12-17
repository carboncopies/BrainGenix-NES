#include <Simulator/Manager.h>



namespace BG {
namespace NES {
namespace Simulator {


Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, API::Manager* _RPCManager, BG::NES::Renderer::Interface* _Renderer) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);
    assert(_RPCManager != nullptr);
    assert(_Renderer != nullptr);

    Config_ = _Config;
    Renderer_ = _Renderer;
    Logger_ = _Logger;


    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Simulation/Reset", [this](std::string RequestJSON){ return SimulationReset(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RunFor", [this](std::string RequestJSON){ return SimulationRunFor(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RecordAll", [this](std::string RequestJSON){ return SimulationRecordAll(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetRecording", [this](std::string RequestJSON){ return SimulationGetRecording(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetStatus", [this](std::string RequestJSON){ return SimulationGetStatus(RequestJSON);});
    _RPCManager->AddRoute("Simulation/BuildMesh", [this](std::string RequestJSON){ return SimulationBuildMesh(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/Create", [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/Create", [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/Create", [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/Create", [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Staple/Create", [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Receptor/Create", [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/Create", [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/SetOutputList", [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/Create", [this](std::string RequestJSON){ return PatchClampADCCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/SetSampleRate", [this](std::string RequestJSON){ return PatchClampADCSetSampleRate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/GetRecordedData", [this](std::string RequestJSON){ return PatchClampADCGetRecordedData(RequestJSON);});


    _RPCManager->AddRoute("Debug", [this](std::string RequestJSON){ return Debug(RequestJSON);});

    // Start SE Thread
    StopThreads_ = false;

}

Manager::~Manager() {

    std::cout<<"[Info] Signaling To Worker Threads To Stop\n";
    StopThreads_ = true;

    std::cout<<"[Info] Joining Simulation Worker Threads\n";
    for (unsigned int i = 0; i < SimulationThreads_.size(); i++) {
        SimulationThreads_[i].join();
    }
}


std::string Manager::SimulationCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    std::string SimulationName = RequestJSON["Name"];
    std::cout<<"[Info] Create Simulation Called, New sim has name "<<SimulationName<<std::endl;


    // Build New Simulation Object
    Simulations_.push_back(std::make_unique<Simulation>());
    int SimID = Simulations_.size()-1;
    Simulation* Sim = Simulations_[SimID].get();
    Sim->Name = SimulationName;
    Sim->ID = SimID;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, Renderer_, &StopThreads_));


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["SimulationID"] = SimID;
    return ResponseJSON.dump();
}

std::string Manager::SimulationReset(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Simulation Reset Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    // ThisSimulation->CurrentTask = SIMULATION_RESET; // request a reset be done
    // ThisSimulation->WorkRequested = true;

    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationRunFor(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Simulation RunFor Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    // ThisSimulation->RunTimes_ms = Util::GetFloat(&RequestJSON, "Runtime_ms"); // set requested runtime
    // ThisSimulation->CurrentTask = SIMULATION_RUNFOR; // request work be done
    // ThisSimulation->WorkRequested = true;

    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationRecordAll(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Simulation RecordAll Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    ThisSimulation->MaxRecordTime_ms = Util::GetFloat(&RequestJSON, "MaxRecordTime_ms");

    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationGetRecording(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Simulation GetRecording Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }


    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Recording"] = ThisSimulation->RecordingBlob;
    return ResponseJSON.dump();
}

std::string Manager::SimulationGetStatus(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Simulation GetStatus Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }


    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["IsSimulating"] = (bool)ThisSimulation->IsProcessing; /////////////////////////////////////////////////////////////////HARD CODED STUFF HERE - FIXME!!!!
    ResponseJSON["RealWorldTimeRemaining_ms"] = 0.0;
    ResponseJSON["RealWorldTimeElapsed_ms"] = 0.0;
    ResponseJSON["InSimulationTime_ms"] = 0.0;
    ResponseJSON["InSimulationTimeRemaining_ms"] = 0.0;
    ResponseJSON["PercentComplete"] = 0.0;
    return ResponseJSON.dump();
}

std::string Manager::SimulationBuildMesh(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation BuildMesh Called, On Sim " + std::to_string(SimulationID), 1);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }

    if (!BuildMeshFromSimulation(Logger_, Renderer_, ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 999; // general failure
        return ResponseJSON.dump();
    }
    

    // Return JSON
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}


std::string Manager::SphereCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Create Sphere Called, On Sim " + std::to_string(SimulationID), 1);
    
    float Radius_um  = Util::GetFloat(&RequestJSON, "Radius_um");
    float CenterPosX_um = Util::GetFloat(&RequestJSON, "CenterPosX_um");
    float CenterPosY_um = Util::GetFloat(&RequestJSON, "CenterPosY_um");
    float CenterPosZ_um = Util::GetFloat(&RequestJSON, "CenterPosZ_um");
    std::string Name = Util::GetString(&RequestJSON, "Name");

    // Build New Sphere Object
    Geometries::Sphere S;
    S.Name = Name;
    S.Radius_um = Radius_um;
    S.Center_um.x_um = CenterPosX_um;
    S.Center_um.y_um = CenterPosY_um;
    S.Center_um.z_um = CenterPosZ_um;


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["ShapeID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int SphereID = ThisSimulation->Collection.Geometries.size();
    S.ID = SphereID;
    ThisSimulation->Collection.Geometries.push_back(S);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeID"] = SphereID;
    return ResponseJSON.dump();
}

std::string Manager::CylinderCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create Cylinder Called, On Sim "<<SimulationID<<std::endl;
    float E0Radius_um  = Util::GetFloat(&RequestJSON, "Point1Radius_um");
    float E0X_um = Util::GetFloat(&RequestJSON, "Point1PosX_um");
    float E0Y_um = Util::GetFloat(&RequestJSON, "Point1PosY_um");
    float E0Z_um = Util::GetFloat(&RequestJSON, "Point1PosZ_um");
    float E1Radius_um  = Util::GetFloat(&RequestJSON, "Point2Radius_um");
    float E1X_um = Util::GetFloat(&RequestJSON, "Point2PosX_um");
    float E1Y_um = Util::GetFloat(&RequestJSON, "Point2PosY_um");
    float E1Z_um = Util::GetFloat(&RequestJSON, "Point2PosZ_um");
    std::string Name = Util::GetString(&RequestJSON, "Name");

    //std::cout << "E0:" << E0X_um << ',' << E0Y_um << ',' << E0Z_um << '\n';
    //std::cout << "E1:" << E1X_um << ',' << E1Y_um << ',' << E1Z_um << '\n';
    std::cout << "E0Radius_um: " << E0Radius_um << '\n';
    std::cout << "E1Radius_um: " << E1Radius_um << '\n';

    // Build New Cylinder Object
    Geometries::Cylinder S;
    S.Name = Name;
    S.End0Radius_um = E0Radius_um;
    S.End0Pos_um.x_um = E0X_um;
    S.End0Pos_um.y_um = E0Y_um;
    S.End0Pos_um.z_um = E0Z_um;
    S.End1Radius_um = E1Radius_um;
    S.End1Pos_um.x_um = E1X_um;
    S.End1Pos_um.y_um = E1Y_um;
    S.End1Pos_um.z_um = E1Z_um;


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["ShapeID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ShapeID = ThisSimulation->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Collection.Geometries.push_back(S);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ShapeID"] = ShapeID;
    return ResponseJSON.dump();
}

std::string Manager::BoxCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create Box Called, On Sim "<<SimulationID<<std::endl;


    // Build New Box Object
    Geometries::Box S;
    S.Name = Util::GetString(&RequestJSON, "Name");
    Util::GetVec3(S.Center_um, &RequestJSON, "CenterPos");
    Util::GetVec3(S.Dims_um, &RequestJSON, "Scale");
    Util::GetVec3(S.Rotations_rad, &RequestJSON, "Rotation", "rad");

    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["ShapeID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ShapeID = ThisSimulation->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Collection.Geometries.push_back(S);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ShapeID"] = ShapeID;
    return ResponseJSON.dump();
}

std::string Manager::BSCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create BS Called, On Sim "<<SimulationID<<std::endl;


    // Build New BS Object
    Compartments::BS C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.ShapeID = Util::GetInt(&RequestJSON, "ShapeID");
    C.MembranePotential_mV = Util::GetFloat(&RequestJSON, "MembranePotential_mV");
    C.SpikeThreshold_mV = Util::GetFloat(&RequestJSON, "SpikeThreshold_mV");
    C.DecayTime_ms = Util::GetFloat(&RequestJSON, "DecayTime_ms");



    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["CompartmentID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int CompartmentID = ThisSimulation->BSCompartments.size();
    C.ID = CompartmentID;
    ThisSimulation->BSCompartments.push_back(C);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["CompartmentID"] = CompartmentID;
    return ResponseJSON.dump();
}

std::string Manager::StapleCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create Staple Called, On Sim "<<SimulationID<<std::endl;


    // Build New Staple Object
    Connections::Staple C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.SourceCompartmentID = Util::GetInt(&RequestJSON, "SourceCompartmentID");
    C.DestinationCompartmentID = Util::GetInt(&RequestJSON, "DestinationCompartmentID");


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["StapleID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int StapleID = ThisSimulation->Staples.size();
    C.ID = StapleID;
    ThisSimulation->Staples.push_back(C);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["StapleID"] = StapleID;
    return ResponseJSON.dump();
}

std::string Manager::ReceptorCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create Receptor Called, On Sim "<<SimulationID<<std::endl;


    // Build New Receptor Object
    Connections::Receptor C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.SourceCompartmentID = Util::GetInt(&RequestJSON, "SourceCompartmentID");
    C.DestinationCompartmentID = Util::GetInt(&RequestJSON, "DestinationCompartmentID");
    C.Conductance_nS = Util::GetFloat(&RequestJSON, "Conductance_nS");
    C.TimeConstant_ms  = Util::GetFloat(&RequestJSON, "TimeConstant_ms");
    Util::GetVec3(C.ReceptorPos_um, &RequestJSON, "ReceptorPos");    


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["ReceptorID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ReceptorID = ThisSimulation->Receptors.size();
    C.ID = ReceptorID;
    ThisSimulation->Receptors.push_back(C);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ReceptorID"] = ReceptorID;
    return ResponseJSON.dump();
}

std::string Manager::PatchClampDACCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create PatchClampDAC Called, On Sim "<<SimulationID<<std::endl;


    // Build New DAC Object
    Tools::PatchClampDAC T;
    T.Name = Util::GetString(&RequestJSON, "Name");
    T.DestinationCompartmentID = Util::GetInt(&RequestJSON, "DestinationCompartmentID");
    T.Timestep_ms = 0.0f;
    Util::GetVec3(T.ClampPos_um, &RequestJSON, "ClampPos");    


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["PatchClampDACID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int PatchClampDACID = ThisSimulation->PatchClampDACs.size();
    T.ID = PatchClampDACID;
    ThisSimulation->PatchClampDACs.push_back(T);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["PatchClampDACID"] = PatchClampDACID;
    return ResponseJSON.dump();
}

std::string Manager::PatchClampDACSetOutputList(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] PatchClampDAC SetOutputList Called, On Sim "<<SimulationID<<std::endl;

    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }

    // Get/Check PatchClampdDACID
    int PatchClampDACID = Util::GetInt(&RequestJSON, "PatchClampDACID");
    if (PatchClampDACID >= ThisSimulation->PatchClampDACs.size() || PatchClampDACID < 0) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // invalid ID
        return ResponseJSON.dump();
    }
    Tools::PatchClampDAC* ThisDAC = &ThisSimulation->PatchClampDACs[PatchClampDACID];
    
    // Set Params
    ThisDAC->Timestep_ms = Util::GetFloat(&RequestJSON, "Timestep_ms");
    Util::GetFloatVector(&ThisDAC->DACVoltages_mV, &RequestJSON, "DDACVoltages_mV");

    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::PatchClampADCCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create PatchClampADC Called, On Sim "<<SimulationID<<std::endl;


    // Build New ADC Object
    Tools::PatchClampADC T;
    T.Name = Util::GetString(&RequestJSON, "Name");
    T.SourceCompartmentID = Util::GetInt(&RequestJSON, "SourceCompartmentID");
    T.Timestep_ms = 0.0f;
    Util::GetVec3(T.ClampPos_um, &RequestJSON, "ClampPos");    


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["PatchClampADCID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int PatchClampADCID = ThisSimulation->PatchClampADCs.size();
    T.ID = PatchClampADCID;
    ThisSimulation->PatchClampADCs.push_back(T);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["PatchClampADCID"] = PatchClampADCID;
    return ResponseJSON.dump();
}

std::string Manager::PatchClampADCSetSampleRate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] PatchClampADC SetSampleRate Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = Util::GetInt(&RequestJSON, "PatchClampADCID");
    if (PatchClampADCID >= ThisSimulation->PatchClampADCs.size() || PatchClampADCID < 0) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // invalid ID
        return ResponseJSON.dump();
    }
    Tools::PatchClampADC* ThisADC = &ThisSimulation->PatchClampADCs[PatchClampADCID];
    
    // Set Params
    ThisADC->Timestep_ms = Util::GetFloat(&RequestJSON, "Timestep_ms");
    ThisADC->RecordedData_mV.clear(); // clear recorded data as it is now invalid (the timestep is not the same anymore)

    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::PatchClampADCGetRecordedData(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] PatchClampADC GetRecordedData Called, On Sim "<<SimulationID<<std::endl;


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 4; // simulation is currently processing
        return ResponseJSON.dump();
    }

    // Get/Check PatchClampdADDCID
    int PatchClampADCID = Util::GetInt(&RequestJSON, "PatchClampADCID");
    if (PatchClampADCID >= ThisSimulation->PatchClampADCs.size() || PatchClampADCID < 0) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // invalid ID
        return ResponseJSON.dump();
    }
    Tools::PatchClampADC* ThisADC = &ThisSimulation->PatchClampADCs[PatchClampADCID];
    

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["Timestep_ms"] = ThisADC->Timestep_ms;
    ResponseJSON["RecordedData_mV"] = ThisADC->RecordedData_mV;
    return ResponseJSON.dump();
}


std::string Manager::Debug(std::string _JSONRequest) {

    // Parse Request
    std::cout<<_JSONRequest<<std::endl;
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

    // if (Util::GetString(&RequestJSON, "Action") == "Render") {

    // }


    std::cout<<"This will break if you don't have at least one simulation. Trying to get sim with id >0\n";

    Simulation* ThisSimulation = Simulations_[Simulations_.size()-1].get();


    BoundingBox BB = BoundingBox();
    BB.bb_point1[0] = -1.;
    BB.bb_point1[1] = -1.;
    BB.bb_point1[2] = -1.;

    BB.bb_point2[0] = 9.;
    BB.bb_point2[1] = 6.;
    BB.bb_point2[2] = 6.;

    
    float VoxelSize = atof(Util::GetString(&RequestJSON, "Query").c_str());
    VoxelArray Arr(BB, VoxelSize);


    MicroscopeParameters MParams;
    MParams.VoxelResolution_um = VoxelSize;


    CreateVoxelArrayFromSimulation(Logger_, ThisSimulation, &MParams, &Arr);

    // for (unsigned int i = 0; i < Arr.GetZ(); i++) {
    //     Arr.SetVoxel(i, i, i, FILLED);
    // }

    Renderer_->ResetScene();


    // RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, 10);
    for (unsigned int i = 0; i < Arr.GetZ(); i++) {
        RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay so our mutex is easier to get by another thread if needed
    }

    // Renderer_->ResetViewer();



    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}



bool Manager::IsSimulationBusy(Simulation* _Sim) {
    return _Sim->IsProcessing || _Sim->WorkRequested;
}

std::vector<std::unique_ptr<Simulation>>* Manager::GetSimulationVectorPtr() {
    return &Simulations_;
}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
