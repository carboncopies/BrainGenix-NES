#include <Simulator/Manager.h>



namespace BG {
namespace NES {
namespace Simulator {


Manager::Manager(Config::Config* _Config, API::Manager* _RPCManager) {
    Config_ = _Config;

    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Simulation/Reset", [this](std::string RequestJSON){ return SimulationReset(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RunFor", [this](std::string RequestJSON){ return SimulationRunFor(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RecordAll", [this](std::string RequestJSON){ return SimulationRecordAll(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetRecording", [this](std::string RequestJSON){ return SimulationGetRecording(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetStatus", [this](std::string RequestJSON){ return SimulationGetStatus(RequestJSON);});
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


// void Manager::SimulationEngineThread(Simulation* _Sim) {

//     // Log Init message
//     std::cout<<"[Info] Starting Simulation Updater Thread\n";

//     // Enter into loop until thread should stop
//     while (!StopThreads_) {

//         if (_Sim->WorkRequested) {
//             _Sim->IsProcessing = true;

//             if (_Sim->CurrentTask == SIMULATION_RESET) {
//                 std::cout<<"[Info] Worker Performing Simulation Reset For Simulation "<<_Sim->ID<<std::endl;
//                 _Sim->CurrentTask = SIMULATION_NONE;
//                 _Sim->WorkRequested = false;

//             } else if (_Sim->CurrentTask == SIMULATION_RUNFOR) {
//                 std::cout<<"[Info] Worker Performing Simulation RunFor For Simulation "<<_Sim->ID<<std::endl;
//                 _Sim->CurrentTask = SIMULATION_NONE;
//                 _Sim->WorkRequested = false;
//             }

//             _Sim->IsProcessing = false;
//         } else {
//             std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10ms
//         }

//     }

// }


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
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Sim, &StopThreads_));


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
    ThisSimulation->CurrentTask = SIMULATION_RESET; // request a reset be done
    ThisSimulation->WorkRequested = true;

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
    ThisSimulation->RunTimes_ms = Util::GetFloat(&RequestJSON, "Runtime_ms"); // set requested runtime
    ThisSimulation->CurrentTask = SIMULATION_RUNFOR; // request work be done
    ThisSimulation->WorkRequested = true;

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

std::string Manager::SphereCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    std::cout<<"[Info] Create Sphere Called, On Sim "<<SimulationID<<std::endl;
    float Radius_nm  = Util::GetFloat(&RequestJSON, "Radius_nm");
    float CenterPosX_nm = Util::GetFloat(&RequestJSON, "CenterPosX_nm");
    float CenterPosY_nm = Util::GetFloat(&RequestJSON, "CenterPosY_nm");
    float CenterPosZ_nm = Util::GetFloat(&RequestJSON, "CenterPosZ_nm");
    std::string Name = Util::GetString(&RequestJSON, "Name");



    // Build New Sphere Object
    Shapes::Sphere S;
    S.Name = Name;
    S.Radius_nm = Radius_nm;
    S.Center_nm[0] = CenterPosX_nm;
    S.Center_nm[1] = CenterPosY_nm;
    S.Center_nm[2] = CenterPosZ_nm;


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
    int SphereID = ThisSimulation->Shapes.Shapes.size();
    S.ID = SphereID;
    ThisSimulation->Shapes.Shapes.push_back(S);


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
    float P1Radius_nm  = Util::GetFloat(&RequestJSON, "Point1Radius_nm");
    float P1X_nm = Util::GetFloat(&RequestJSON, "Point1PosX_nm");
    float P1Y_nm = Util::GetFloat(&RequestJSON, "Point1PosY_nm");
    float P1Z_nm = Util::GetFloat(&RequestJSON, "Point1PosZ_nm");
    float P2Radius_nm  = Util::GetFloat(&RequestJSON, "Point2Radius_nm");
    float P2X_nm = Util::GetFloat(&RequestJSON, "Point2PosX_nm");
    float P2Y_nm = Util::GetFloat(&RequestJSON, "Point2PosY_nm");
    float P2Z_nm = Util::GetFloat(&RequestJSON, "Point2PosZ_nm");
    std::string Name = Util::GetString(&RequestJSON, "Name");


    // Build New Cylinder Object
    Shapes::Cylinder S;
    S.Name = Name;
    S.Point1Radius_nm = P1Radius_nm;
    S.Point1Pos_nm[0] = P1X_nm;
    S.Point1Pos_nm[1] = P1Y_nm;
    S.Point1Pos_nm[2] = P1Z_nm;
    S.Point2Radius_nm = P2Radius_nm;
    S.Point2Pos_nm[0] = P2X_nm;
    S.Point2Pos_nm[1] = P2Y_nm;
    S.Point2Pos_nm[2] = P2Z_nm;


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
    int ShapeID = ThisSimulation->Shapes.Shapes.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Shapes.Shapes.push_back(S);


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
    Shapes::Box S;
    S.Name = Util::GetString(&RequestJSON, "Name");;
    Util::GetVec3(S.CenterPos_nm, &RequestJSON, "CenterPos");
    Util::GetVec3(S.Scale_nm, &RequestJSON, "Scale");
    Util::GetVec3(S.Rotation_rad, &RequestJSON, "Rotation", "rad");



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
    int ShapeID = ThisSimulation->Shapes.Shapes.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Shapes.Shapes.push_back(S);


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
    Util::GetVec3(C.ReceptorPos_nm, &RequestJSON, "ReceptorPos");    


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
    Util::GetVec3(T.ClampPos_nm, &RequestJSON, "ClampPos");    


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
    Util::GetVec3(T.ClampPos_nm, &RequestJSON, "ClampPos");    


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


bool Manager::IsSimulationBusy(Simulation* _Sim) {
    return _Sim->IsProcessing || _Sim->WorkRequested;
}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG