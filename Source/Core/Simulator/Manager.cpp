#include <Simulator/Manager.h>
#include <Simulator/BallAndStick/BSNeuron.h>

#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {


Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, API::Manager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_Config != nullptr);
    assert(_RPCManager != nullptr);
    assert(_RenderPool != nullptr);


    Config_ = _Config;
    Logger_ = _Logger;
    RenderPool_ = _RenderPool;



    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", Logger_, [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Simulation/Reset", Logger_, [this](std::string RequestJSON){ return SimulationReset(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RunFor", Logger_, [this](std::string RequestJSON){ return SimulationRunFor(RequestJSON);});
    _RPCManager->AddRoute("Simulation/RecordAll", Logger_, [this](std::string RequestJSON){ return SimulationRecordAll(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetRecording", Logger_, [this](std::string RequestJSON){ return SimulationGetRecording(RequestJSON);});
    _RPCManager->AddRoute("Simulation/GetStatus", Logger_, [this](std::string RequestJSON){ return SimulationGetStatus(RequestJSON);});
    _RPCManager->AddRoute("Simulation/BuildMesh", Logger_, [this](std::string RequestJSON){ return SimulationBuildMesh(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/Create", Logger_, [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkSphereCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/Create", Logger_, [this](std::string RequestJSON){ return CylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Cylinder/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkCylinderCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/Create", Logger_, [this](std::string RequestJSON){ return BoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Box/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkBoxCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/Create", Logger_, [this](std::string RequestJSON){ return BSCreate(RequestJSON);});
    _RPCManager->AddRoute("Compartment/BS/BulkCreate", Logger_, [this](std::string RequestJSON){ return BulkBSCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Staple/Create", Logger_, [this](std::string RequestJSON){ return StapleCreate(RequestJSON);});
    _RPCManager->AddRoute("Connection/Receptor/Create", Logger_, [this](std::string RequestJSON){ return ReceptorCreate(RequestJSON);});
    _RPCManager->AddRoute("Neuron/BSNeuron/Create", Logger_, [this](std::string RequestJSON){ return BSNeuronCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/Create", Logger_, [this](std::string RequestJSON){ return PatchClampDACCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampDAC/SetOutputList", Logger_, [this](std::string RequestJSON){ return PatchClampDACSetOutputList(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/Create", Logger_, [this](std::string RequestJSON){ return PatchClampADCCreate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/SetSampleRate", Logger_, [this](std::string RequestJSON){ return PatchClampADCSetSampleRate(RequestJSON);});
    _RPCManager->AddRoute("Tool/PatchClampADC/GetRecordedData", Logger_, [this](std::string RequestJSON){ return PatchClampADCGetRecordedData(RequestJSON);});
    _RPCManager->AddRoute("NES", Logger_, [this](std::string RequestJSON){ return NESRequest(RequestJSON);});


    _RPCManager->AddRoute("Debug", Logger_, [this](std::string RequestJSON){ return Debug(RequestJSON);});

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
    Logger_->Log("Create Simulation Called, New Sim Has Name " + SimulationName, 3);


    // Build New Simulation Object
    Simulations_.push_back(std::make_unique<Simulation>(Logger_));
    int SimID = Simulations_.size()-1;
    Simulation* Sim = Simulations_[SimID].get();
    Sim->Name = SimulationName;
    Sim->ID = SimID;
    Sim->CurrentTask = SIMULATION_NONE;

    // Start Thread
    SimulationThreads_.push_back(std::thread(&SimulationEngineThread, Logger_, Sim, RenderPool_, &StopThreads_));


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["SimulationID"] = SimID;
    return ResponseJSON.dump();
}

std::string Manager::SimulationReset(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Reset Called, On Sim " + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    // ThisSimulation->CurrentTask = SIMULATION_RESET; // request a reset be done
    // ThisSimulation->WorkRequested = true;

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationRunFor(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation RunFor Called, On Sim " + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }

    ThisSimulation->RunTimes_ms = Util::GetFloat(&RequestJSON, "Runtime_ms"); // set requested runtime
    ThisSimulation->CurrentTask = SIMULATION_RUNFOR; // request work be done
    ThisSimulation->WorkRequested = true;

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationRecordAll(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation RecordAll Called, On Sim " + std::to_string(SimulationID), 3);



    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    ThisSimulation->MaxRecordTime_ms = Util::GetFloat(&RequestJSON, "MaxRecordTime_ms");

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::SimulationGetRecording(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation GetRecording Called, On Sim" + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
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

    Logger_->Log("Simulation GetStatus Called, On Sim " + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid sim id
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }

    // if (!BuildMeshFromSimulation(Logger_, Renderer_, ThisSimulation)) {
    //     nlohmann::json ResponseJSON;
    //     ResponseJSON["StatusCode"] = 999; // general failure
    //     return ResponseJSON.dump();
    // }
    
    std::cout<<"WARNING THE SIMULATION BUILD MESH FUNCTION DOESNT DO AYNTHING!!!!\n";


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
    S.Center_um.x = CenterPosX_um;
    S.Center_um.y = CenterPosY_um;
    S.Center_um.z = CenterPosZ_um;


    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["ShapeID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int SphereID = ThisSimulation->Collection.Geometries.size();
    S.ID = SphereID;
    ThisSimulation->Collection.Geometries.push_back(S);
    //ThisSimulation->Collection.append(S);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeID"] = SphereID;
    return ResponseJSON.dump();
}

std::string Manager::BulkSphereCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Sphere Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> RadiusList_um;
    std::vector<float> CenterXList_um;
    std::vector<float> CenterYList_um;
    std::vector<float> CenterZList_um;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&RadiusList_um, &RequestJSON, "RadiusList_um");
    Util::GetFloatVector(&CenterXList_um, &RequestJSON, "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &RequestJSON, "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &RequestJSON, "CenterZList_um");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = RadiusList_um.size();
    LengthsEqual &= (CenterXList_um.size() == Length);
    LengthsEqual &= (CenterYList_um.size() == Length);
    LengthsEqual &= (CenterZList_um.size() == Length);
    LengthsEqual &= (NameList.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Spheres", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Sphere Object
        Geometries::Sphere S;
        S.Name = NameList[i];
        S.Radius_um = RadiusList_um[i];
        S.Center_um.x = CenterXList_um[i];
        S.Center_um.y = CenterYList_um[i];
        S.Center_um.z = CenterZList_um[i];


        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int SphereID = ThisSimulation->Collection.Geometries.size();
        S.ID = SphereID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}


std::string Manager::CylinderCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create Cylinder Called, On Sim " + std::to_string(SimulationID), 3);


    float E0Radius_um  = Util::GetFloat(&RequestJSON, "Point1Radius_um");
    float E0X_um = Util::GetFloat(&RequestJSON, "Point1PosX_um");
    float E0Y_um = Util::GetFloat(&RequestJSON, "Point1PosY_um");
    float E0Z_um = Util::GetFloat(&RequestJSON, "Point1PosZ_um");
    float E1Radius_um  = Util::GetFloat(&RequestJSON, "Point2Radius_um");
    float E1X_um = Util::GetFloat(&RequestJSON, "Point2PosX_um");
    float E1Y_um = Util::GetFloat(&RequestJSON, "Point2PosY_um");
    float E1Z_um = Util::GetFloat(&RequestJSON, "Point2PosZ_um");
    std::string Name = Util::GetString(&RequestJSON, "Name");


    // Build New Cylinder Object
    Geometries::Cylinder S;
    S.Name = Name;
    S.End0Radius_um = E0Radius_um;
    S.End0Pos_um.x = E0X_um;
    S.End0Pos_um.y = E0Y_um;
    S.End0Pos_um.z = E0Z_um;
    S.End1Radius_um = E1Radius_um;
    S.End1Pos_um.x = E1X_um;
    S.End1Pos_um.y = E1Y_um;
    S.End1Pos_um.z = E1Z_um;


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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ShapeID = ThisSimulation->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Collection.Geometries.push_back(S);
    //ThisSimulation->Collection.append(S);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ShapeID"] = ShapeID;
    return ResponseJSON.dump();
}

std::string Manager::BulkCylinderCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Cylinder Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> Point1RadiusList_um;
    std::vector<float> Point2RadiusList_um;
    std::vector<float> Point1XList_um;
    std::vector<float> Point1YList_um;
    std::vector<float> Point1ZList_um;
    std::vector<float> Point2XList_um;
    std::vector<float> Point2YList_um;
    std::vector<float> Point2ZList_um;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&Point1RadiusList_um, &RequestJSON, "Point1RadiusList_um");
    Util::GetFloatVector(&Point2RadiusList_um, &RequestJSON, "Point2RadiusList_um");
    Util::GetFloatVector(&Point1XList_um, &RequestJSON, "Point1XList_um");
    Util::GetFloatVector(&Point1YList_um, &RequestJSON, "Point1YList_um");
    Util::GetFloatVector(&Point1ZList_um, &RequestJSON, "Point1ZList_um");
    Util::GetFloatVector(&Point2XList_um, &RequestJSON, "Point2XList_um");
    Util::GetFloatVector(&Point2YList_um, &RequestJSON, "Point2YList_um");
    Util::GetFloatVector(&Point2ZList_um, &RequestJSON, "Point2ZList_um");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = Point1RadiusList_um.size();
    LengthsEqual &= (Point2RadiusList_um.size() == Length);
    LengthsEqual &= (Point1XList_um.size() == Length);
    LengthsEqual &= (Point1YList_um.size() == Length);
    LengthsEqual &= (Point1ZList_um.size() == Length);
    LengthsEqual &= (Point2XList_um.size() == Length);
    LengthsEqual &= (Point2YList_um.size() == Length);
    LengthsEqual &= (Point2ZList_um.size() == Length);
    LengthsEqual &= (NameList.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Cylinders", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Cylinder Object
        Geometries::Cylinder S;
        S.Name = NameList[i];
        S.End0Radius_um = Point1RadiusList_um[i];
        S.End1Radius_um = Point2RadiusList_um[i];
        S.End0Pos_um.x = Point1XList_um[i];
        S.End0Pos_um.y = Point1YList_um[i];
        S.End0Pos_um.z = Point1ZList_um[i];
        S.End1Pos_um.x = Point2XList_um[i];
        S.End1Pos_um.y = Point2YList_um[i];
        S.End1Pos_um.z = Point2ZList_um[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int CylinderID = ThisSimulation->Collection.Geometries.size();
        S.ID = CylinderID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}


std::string Manager::BoxCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create Box Called, On Sim " + std::to_string(SimulationID), 3);


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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ShapeID = ThisSimulation->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    S.ID = ShapeID;
    ThisSimulation->Collection.Geometries.push_back(S);
    //ThisSimulation->Collection.append(S);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ShapeID"] = ShapeID;
    return ResponseJSON.dump();
}

std::string Manager::BulkBoxCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create Box Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> CenterXList_um;
    std::vector<float> CenterYList_um;
    std::vector<float> CenterZList_um;
    std::vector<float> DimensionsXList_um;
    std::vector<float> DimensionsYList_um;
    std::vector<float> DimensionsZList_um;
    std::vector<float> RotationXList_rad;
    std::vector<float> RotationYList_rad;
    std::vector<float> RotationZList_rad;
    std::vector<std::string> NameList;
    Util::GetFloatVector(&CenterXList_um, &RequestJSON, "CenterXList_um");
    Util::GetFloatVector(&CenterYList_um, &RequestJSON, "CenterYList_um");
    Util::GetFloatVector(&CenterZList_um, &RequestJSON, "CenterZList_um");
    Util::GetFloatVector(&DimensionsXList_um, &RequestJSON, "DimensionsXList_um");
    Util::GetFloatVector(&DimensionsYList_um, &RequestJSON, "DimensionsYList_um");
    Util::GetFloatVector(&DimensionsZList_um, &RequestJSON, "DimensionsZList_um");
    Util::GetFloatVector(&RotationXList_rad, &RequestJSON, "RotationXList_rad");
    Util::GetFloatVector(&RotationYList_rad, &RequestJSON, "RotationYList_rad");
    Util::GetFloatVector(&RotationZList_rad, &RequestJSON, "RotationZList_rad");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = NameList.size();
    LengthsEqual &= (CenterXList_um.size() == Length);
    LengthsEqual &= (CenterYList_um.size() == Length);
    LengthsEqual &= (CenterZList_um.size() == Length);
    LengthsEqual &= (DimensionsXList_um.size() == Length);
    LengthsEqual &= (DimensionsYList_um.size() == Length);
    LengthsEqual &= (DimensionsZList_um.size() == Length);
    LengthsEqual &= (RotationXList_rad.size() == Length);
    LengthsEqual &= (RotationYList_rad.size() == Length);
    LengthsEqual &= (RotationZList_rad.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> ShapeIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " Boxes", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Box Object
        Geometries::Box S;
        S.Name = NameList[i];
        S.Center_um.x = CenterXList_um[i];
        S.Center_um.y = CenterYList_um[i];
        S.Center_um.z = CenterZList_um[i];
        S.Dims_um.x = DimensionsXList_um[i];
        S.Dims_um.y = DimensionsYList_um[i];
        S.Dims_um.z = DimensionsZList_um[i];
        S.Rotations_rad.x = RotationXList_rad[i];
        S.Rotations_rad.y = RotationYList_rad[i];
        S.Rotations_rad.z = RotationZList_rad[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int BoxID = ThisSimulation->Collection.Geometries.size();
        S.ID = BoxID;
        ThisSimulation->Collection.Geometries.push_back(S);

        ShapeIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeIDs"] = ShapeIDs;
    return ResponseJSON.dump();
}


std::string Manager::BSCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create BS Called, On Sim " + std::to_string(SimulationID), 3);



    // Build New BS Object
    Compartments::BS C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.ShapeID = Util::GetInt(&RequestJSON, "ShapeID");
    C.MembranePotential_mV = Util::GetFloat(&RequestJSON, "MembranePotential_mV");
    C.SpikeThreshold_mV = Util::GetFloat(&RequestJSON, "SpikeThreshold_mV");
    C.DecayTime_ms = Util::GetFloat(&RequestJSON, "DecayTime_ms");
    C.RestingPotential_mV = Util::GetFloat(&RequestJSON, "RestingPotential_mV");
    C.AfterHyperpolarizationAmplitude_mV = Util::GetFloat(&RequestJSON, "AfterHyperpolarizationAmplitude_mV");



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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int CompartmentID = ThisSimulation->BSCompartments.size();
    C.ID = CompartmentID;
    ThisSimulation->BSCompartments.push_back(C);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["CompartmentID"] = CompartmentID;
    return ResponseJSON.dump();
}

std::string Manager::BulkBSCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Simulation Bulk Create BS Called, On Sim " + std::to_string(SimulationID), 1);
    
    // Get Parameters
    std::vector<float> MembranePotentialList_mV;
    std::vector<float> SpikeThresholdList_mV;
    std::vector<float> DecayTimeList_ms;
    std::vector<float> RestingPotentialList_mV;
    std::vector<float> AfterHyperpolarizationAmplitudeList_mV;
    std::vector<std::string> NameList;
    std::vector<int> ShapeIDList;
    Util::GetFloatVector(&MembranePotentialList_mV, &RequestJSON, "MembranePotentialList_mV");
    Util::GetFloatVector(&SpikeThresholdList_mV, &RequestJSON, "SpikeThresholdList_mV");
    Util::GetFloatVector(&DecayTimeList_ms, &RequestJSON, "DecayTimeList_ms");
    Util::GetFloatVector(&RestingPotentialList_mV, &RequestJSON, "RestingPotentialList_mV");
    Util::GetFloatVector(&AfterHyperpolarizationAmplitudeList_mV, &RequestJSON, "AfterHyperpolarizationAmplitudeList_mV");
    Util::GetStringVector(&NameList, &RequestJSON, "NameList");
    Util::GetIntVector(&ShapeIDList, &RequestJSON, "ShapeIDList");

    // Check List Lengths Are Equal, Exit If Not
    bool LengthsEqual = true;
    size_t Length = NameList.size();
    LengthsEqual &= (ShapeIDList.size() == Length);
    LengthsEqual &= (MembranePotentialList_mV.size() == Length);
    LengthsEqual &= (SpikeThresholdList_mV.size() == Length);
    LengthsEqual &= (DecayTimeList_ms.size() == Length);
    LengthsEqual &= (RestingPotentialList_mV.size() == Length);
    LengthsEqual &= (AfterHyperpolarizationAmplitudeList_mV.size() == Length);
    if (!LengthsEqual) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2;
        ResponseJSON["ShapeIDs"] = "[]";
        return ResponseJSON.dump();
    }
    


    // Setup Output ID List
    std::vector<size_t> CompartmentIDs;

    // Now enumerate and build them
    Logger_->Log("Adding " + std::to_string(Length) + " BS Compartments", 2);
    for (size_t i = 0; i < Length; i++) {

        // Build New Box Object
        Compartments::BS S;
        S.Name = NameList[i];
        S.AfterHyperpolarizationAmplitude_mV = AfterHyperpolarizationAmplitudeList_mV[i];
        S.DecayTime_ms = DecayTimeList_ms[i];
        S.ShapeID = ShapeIDList[i];
        S.MembranePotential_mV = MembranePotentialList_mV[i];
        S.RestingPotential_mV = RestingPotentialList_mV[i];
        S.SpikeThreshold_mV = SpikeThresholdList_mV[i];

        // Add to Sim, Set ID
        if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 2;
            ResponseJSON["ShapeIDs"] = "[]";
            return ResponseJSON.dump();
        }
        Simulation* ThisSimulation = Simulations_[SimulationID].get();
        if (IsSimulationBusy(ThisSimulation)) {
            nlohmann::json ResponseJSON;
            ResponseJSON["StatusCode"] = 5; // simulation is currently processing
            return ResponseJSON.dump();
        }
        int CompartmentID = ThisSimulation->BSCompartments.size();
        S.ID = CompartmentID;
        ThisSimulation->BSCompartments.push_back(S);


        CompartmentIDs.push_back(S.ID);

    }

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["CompartmentIDs"] = CompartmentIDs;
    return ResponseJSON.dump();
}

std::string Manager::StapleCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create Staple Called, On Sim " + std::to_string(SimulationID), 3);


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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int StapleID = ThisSimulation->Staples.size();
    C.ID = StapleID;
    ThisSimulation->Staples.push_back(C);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["StapleID"] = StapleID;
    return ResponseJSON.dump();
}

std::string Manager::ReceptorCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create Receptor Called, On Sim " + std::to_string(SimulationID), 3);



    // Build New Receptor Object
    Connections::Receptor C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.SourceCompartmentID = Util::GetInt(&RequestJSON, "SourceCompartmentID");
    C.DestinationCompartmentID = Util::GetInt(&RequestJSON, "DestinationCompartmentID");
    C.Conductance_nS = Util::GetFloat(&RequestJSON, "Conductance_nS");
    C.TimeConstantRise_ms  = Util::GetFloat(&RequestJSON, "TimeConstantRise_ms");
    C.TimeConstantDecay_ms  = Util::GetFloat(&RequestJSON, "TimeConstantDecay_ms");
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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int ReceptorID = ThisSimulation->Receptors.size();
    C.ID = ReceptorID;
    ThisSimulation->Receptors.push_back(C);
    // *** TODO: Are these receptor structs that are kept in the list here actually
    //           used anywhere? Do they create a functional receptor object? Are
    //           they checked during Neuron updates?
    //           This might be where a functional object has to be created.


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["ReceptorID"] = ReceptorID;
    return ResponseJSON.dump();
}

/*
As of 2024-01-12 the method to add a neuron that will be run in a
simulation is:

1. Create the soma and axon shapes for the neuron (remember their IDs).
2. Create a BSNeuron (which creates compartments using the shapes provided).
3. Add the BSNeuron to a NeuralCircuit.
OR
1. Create a NeuralCircuit.
2. Tell the NeuralCircuit to create a neuron.
*/

std::string Manager::BSNeuronCreate(std::string _JSONRequest) {
    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create BSNeuron Called, On Sim " + std::to_string(SimulationID), 3);

    // Build New BSNeuron Object
    CoreStructs::BSNeuronStruct C;
    C.Name = Util::GetString(&RequestJSON, "Name");
    C.SomaShapeID = Util::GetInt(&RequestJSON, "SomaID");
    C.AxonShapeID = Util::GetInt(&RequestJSON, "AxonID");
    C.MembranePotential_mV = Util::GetFloat(&RequestJSON, "MembranePotential_mV");
    C.RestingPotential_mV = Util::GetFloat(&RequestJSON, "RestingPotential_mV");
    C.SpikeThreshold_mV = Util::GetFloat(&RequestJSON, "SpikeThreshold_mV");
    C.DecayTime_ms = Util::GetFloat(&RequestJSON, "DecayTime_ms");
    C.AfterHyperpolarizationAmplitude_mV = Util::GetFloat(&RequestJSON, "AfterHyperpolarizationAmplitude_mV");
    C.PostsynapticPotentialRiseTime_ms = Util::GetFloat(&RequestJSON, "PostsynapticPotentialRiseTime_ms");
    C.PostsynapticPotentialDecayTime_ms = Util::GetFloat(&RequestJSON, "PostsynapticPotentialDecayTime_ms");
    C.PostsynapticPotentialAmplitude_mV = Util::GetFloat(&RequestJSON, "PostsynapticPotentialAmplitude_mV");

    // Add to Sim, Set ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        ResponseJSON["NeuronID"] = -1;
        return ResponseJSON.dump();
    }
    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int NeuronID = ThisSimulation->Neurons.size();
    C.ID = NeuronID;
    // NOTE: At this time, we're keeping both a list of these data structs (in the Neurons list)
    //       and a list of functional neuron objects. For more info, see Log 202401121525.
    Geometries::Geometry * soma_ptr = ThisSimulation->Collection.GetGeometry(C.SomaShapeID);
    Geometries::Geometry * axon_ptr = ThisSimulation->Collection.GetGeometry(C.AxonShapeID);
    if ((!axon_ptr) || (!soma_ptr)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // invalid shape id
        ResponseJSON["NeuronID"] = -1;
        return ResponseJSON.dump();
    }
    ThisSimulation->Neurons.push_back(std::make_shared<BallAndStick::BSNeuron>(C, soma_ptr, axon_ptr));

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["NeuronID"] = NeuronID;
    return ResponseJSON.dump();
}

std::string Manager::PatchClampDACCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create PatchClampDAC Called, On Sim " + std::to_string(SimulationID), 3);


    // Build New DAC Object
    Tools::PatchClampDAC T;
    T.Name = Util::GetString(&RequestJSON, "Name");
    T.DestinationCompartmentID = Util::GetInt(&RequestJSON, "DestinationCompartmentID");
    //T.Timestep_ms = 0.0f;
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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int PatchClampDACID = ThisSimulation->PatchClampDACs.size();
    T.ID = PatchClampDACID;
    ThisSimulation->PatchClampDACs.push_back(T);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["PatchClampDACID"] = PatchClampDACID;
    return ResponseJSON.dump();
}

// *** TODO: We should use an enum for response codes so that we don't
//           have to remember which number means what.
std::string ErrResponse(int ErrStatusCode) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = ErrStatusCode;
    return ResponseJSON.dump();
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "PatchClampDACID": <DAC-ID>,
 *   "ControlData": [
 *      [ <t_ms>, <v_mV> ],
 *      (more pairs)
 *   ]
 * }
 */
std::string Manager::PatchClampDACSetOutputList(std::string _JSONRequest) {

    // *** TODO: This needs to be modified so that it pulls out a
    //           list of pairs, where each pair holds a time and a voltage.

    // ----- Begin standard part (we should make a shared helper function for this) -----
    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("PatchClampDAC SetOutputList Called, On Sim " + std::to_string(SimulationID), 3);

    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        return ErrResponse(1); // invalid simulation id
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        return ErrResponse(5); // simulation is currently processing
    }
    // ----------------------------------------------------------------------------------

    // Get/Check PatchClampdDACID
    int PatchClampDACID = Util::GetInt(&RequestJSON, "PatchClampDACID");
    if (PatchClampDACID >= ThisSimulation->PatchClampDACs.size() || PatchClampDACID < 0) {
        return ErrResponse(2); // invalid ID
    }
    Tools::PatchClampDAC* ThisDAC = &ThisSimulation->PatchClampDACs[PatchClampDACID];
    
    // Set Params
    ThisDAC->ControlData.clear();
    const auto & ControlDataJSON = RequestJSON.find("ControlData");
    if (ControlDataJSON == RequestJSON.end()) {
        return ErrResponse(2); // invalid data *** TODO: use the right code here
    }
    for (const auto & value_pair: ControlDataJSON.value()) {
        if (value_pair.size() < 2) {
            return ErrResponse(2); // invalid data *** TODO: use the right code here
        }
        float t_ms = value_pair[0].template get<float>();
        float v_mV = value_pair[1].template get<float>();
        ThisDAC->ControlData.emplace_back(t_ms, v_mV);
    }

    // Return Result ID
    return ErrResponse(0); // ok
}

std::string Manager::PatchClampADCCreate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("Create PatchClampADC Called, On Sim " + std::to_string(SimulationID), 3);


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
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
        return ResponseJSON.dump();
    }
    int PatchClampADCID = ThisSimulation->PatchClampADCs.size();
    T.ID = PatchClampADCID;
    ThisSimulation->PatchClampADCs.push_back(T);


    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    ResponseJSON["PatchClampADCID"] = PatchClampADCID;
    return ResponseJSON.dump();
}

std::string Manager::PatchClampADCSetSampleRate(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("PatchClampADC SetSampleRate Called, On Sim " + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
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

    // Return Result ID
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0; // ok
    return ResponseJSON.dump();
}

std::string Manager::PatchClampADCGetRecordedData(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");

    Logger_->Log("PatchClampADC GetRecordedData Called, On Sim " + std::to_string(SimulationID), 3);


    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    if (IsSimulationBusy(ThisSimulation)) {
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 5; // simulation is currently processing
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

// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.

//typedef bool NESRequest_func_t(Simulation*, const nlohmann::json&);
typedef std::string NESRequest_func_t(Manager&, const nlohmann::json&);
typedef std::map<std::string, NESRequest_func_t*> NESRequest_map_t;

std::string SphereCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.SphereCreate(ReqParams.dump());
}
std::string CylinderCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.CylinderCreate(ReqParams.dump());
}
std::string BoxCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.BoxCreate(ReqParams.dump());
}
std::string StapleCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.StapleCreate(ReqParams.dump());
}
std::string ReceptorCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.ReceptorCreate(ReqParams.dump());
}
std::string BSNeuronCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.BSNeuronCreate(ReqParams.dump());
}
std::string PatchClampDACCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampDACCreate(ReqParams.dump());
}
std::string PatchClampADCCreateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCCreate(ReqParams.dump());
}
std::string PatchClampDACSetOutputListHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampDACSetOutputList(ReqParams.dump());
}
std::string PatchClampADCSetSampleRateHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCSetSampleRate(ReqParams.dump());
}
std::string PatchClampADCGetRecordedDataHandler(Manager& Man, const nlohmann::json& ReqParams) {
    return Man.PatchClampADCGetRecordedData(ReqParams.dump());
}

const NESRequest_map_t NES_Request_handlers = {
    {"SphereCreate", SphereCreateHandler},
    {"CylinderCreate", CylinderCreateHandler},
    {"BoxCreate", BoxCreateHandler},
    {"StapleCreate", StapleCreateHandler},
    {"ReceptorCreate", ReceptorCreateHandler},
    {"BSNeuronCreate", BSNeuronCreateHandler},
    {"PatchClampDACCreate", PatchClampDACCreateHandler},
    {"PatchClampADCCreate", PatchClampADCCreateHandler},
    {"PatchClampDACSetOutputList", PatchClampDACSetOutputListHandler},
    {"PatchClampADCSetSampleRate", PatchClampADCSetSampleRateHandler},
    {"PatchClampADCGetRecordedData", PatchClampADCGetRecordedDataHandler},
};

bool Manager::BadReqID(int ReqID) {
    // *** TODO: Add some rules here for ReqIDs that should be refused.
    //           For example, keep track of the largest ReqID received
    //           and reject if ReqID is smaller than that. This is
    //           useful if ReqIDs are made using clock time or something.
    return false;
}

/**
 * This expects requests of the following format:
 * [
 *   {
 *     "ReqID": <request-id>,
 *     "AddBSNeuron": {
 *       "SimulationID": <SimID>, // Logically, this could belong outside AddBSNeuron, but we wish to reuse backward compatible functions. 
 *       "Name": <name>,
 *       "SomaID": <soma-id>,
 *       "AxonID": <axon-id>,
 *       <etc... all parameters>
 *     }
 *   },
 *   <more requests>
 * ]
 */
std::string Manager::NESRequest(std::string _JSONRequest) { // Generic JSON-based NES requests.

    // Parse Request
    Logger_->Log(_JSONRequest, 3);
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

    // Build Response
    nlohmann::json ResponseJSON = nlohmann::json::array(); // Create empty array for the list of responses.

    // For each request in the JSON list:
    for (const auto & req : RequestJSON) {

        int ReqID = -1;
        int SimulationID = -1;
        std::string ReqFunc;
        nlohmann::json ReqParams;
        std::string Response;

        // Get the mandatory components of a request:
        for (const auto & [req_key, req_value]: req.items()) {
            if (req_key == "ReqID") {
                ReqID = req_value.template get<int>();
            //} else if (req_key == "SimID") {
            //    SimulationID = req_value.template get<int>();
            } else {
                ReqFunc = req_key;
                ReqParams = req_value;
            }
        }

        // // Check Sim ID
        // if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        //     // *** TODO: This failure should end up in a response JSON for that element of the response array.
        //     nlohmann::json ReqResponseJSON;
        //     ReqResponseJSON["ReqID"] = ReqID;
        //     ReqResponseJSON["StatusCode"] = 1; // invalid simulation id
        //     Response = ReqResponseJSON.dump();
        // }
        // Simulation* ThisSimulation = Simulations_[SimulationID].get();

        if (BadReqID(ReqID)) { // e.g. < highest request ID already handled
            nlohmann::json ReqResponseJSON;
            ReqResponseJSON["ReqID"] = ReqID;
            ReqResponseJSON["StatusCode"] = 1; // bad request id
            Response = ReqResponseJSON.dump();
        } else {

            // Typically would call a specific handler from here, but let's just keep parsing.
            auto it = NES_Request_handlers.find(ReqFunc);
            if (it == NES_Request_handlers.end()) {
                nlohmann::json ReqResponseJSON;
                ReqResponseJSON["ReqID"] = ReqID;
                ReqResponseJSON["StatusCode"] = 1; // unknown request *** TODO: use the right code
                Response = ReqResponseJSON.dump();
            } else {
                Response = it->second(*this, ReqParams); // Calls the handler.
                // *** TODO: Either:
                //     a) Convert handlers to return nlohmann::json objects so that we
                //        can easily add ReqResponseJSON["ReqID"] = ReqID here, or,
                //     b) Convert Response back to a ReqResponseJSON here in order to
                //        add that... (This is more work, lower performance...)
                //     Right now, we do b) (sadly...)
                nlohmann::json ReqResponseJSON = nlohmann::json::parse(Response);
                ReqResponseJSON["ReqID"] = ReqID;
                Response = ReqResponseJSON.dump();
            }

        }
        ResponseJSON.push_back(Response);

    }

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

    // Renderer_->ResetScene();


    // // RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, 10);
    // for (unsigned int i = 0; i < Arr.GetZ(); i++) {
    //     RenderSliceFromArray(Logger_, Renderer_, &MParams, &Arr, i);
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay so our mutex is easier to get by another thread if needed
    // }

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
