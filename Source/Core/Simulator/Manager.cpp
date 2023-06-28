#include <Simulator/Manager.h>



namespace BG {
namespace NES {
namespace Simulator {


Manager::Manager(Config::Config* _Config, API::Manager* _RPCManager) {
    Config_ = _Config;

    // Register Callback For CreateSim
    _RPCManager->AddRoute("Simulation/Create", [this](std::string RequestJSON){ return SimulationCreate(RequestJSON);});
    _RPCManager->AddRoute("Geometry/Shape/Sphere/Create", [this](std::string RequestJSON){ return SphereCreate(RequestJSON);});
    

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
    int SphereID = ThisSimulation->Shapes.Spheres.size();
    S.ID = SphereID;
    ThisSimulation->Shapes.Spheres.push_back(S);


    // Return Status ID
    nlohmann::json ResponseJSON;
    ResponseJSON["ShapeID"] = SphereID;
    return ResponseJSON.dump();
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG