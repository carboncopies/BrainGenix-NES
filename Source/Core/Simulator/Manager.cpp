#include <Simulator/Manager.h>



namespace BG {
namespace NES {
namespace Simulator {


Manager::Manager(Config::Config* _Config, API::Manager* _RPCManager) {
    Config_ = _Config;

    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/Create", [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/Create", [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/Create", [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/Create", [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Staple/Create", [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Receptor/Create", [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/Create", [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/SetOutputList", [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});


}

Manager::~Manager() {

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


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["SimulationID"] = SimID;
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


    // Build New Staple Object
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


    // Build New Staple Object
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
        ResponseJSON["PatchClampDACID"] = -1;
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();

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




}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG