#include <Simulator/RPC/GeometryRPCInterface.h>
#include <RPC/APIStatusCode.h>

// Third-Party Libraries (BG convention: use <> instead of "")

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>



namespace BG {
namespace NES {
namespace Simulator {


GeometryRPCInterface::GeometryRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<Simulation>>* _Simulations, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;
    Simulations_ = _Simulations;

    // Register Callbacks
    _RPCManager->AddRoute("Simulation/Geometry/Sphere/Create",   std::bind(&GeometryRPCInterface::SphereCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Geometry/Cylinder/Create", std::bind(&GeometryRPCInterface::CylinderCreate, this, std::placeholders::_1));
    _RPCManager->AddRoute("Simulation/Geometry/Box/Create",      std::bind(&GeometryRPCInterface::BoxCreate, this, std::placeholders::_1));

}

GeometryRPCInterface::~GeometryRPCInterface() {

}

std::string GeometryRPCInterface::SphereCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Geometry/Sphere/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    
    // Build New Sphere Object
    Geometries::Sphere S;
    if ((!Handle.GetParFloat("Radius_um", S.Radius_um))
        || (!Handle.GetParVec3("CenterPos", S.Center_um))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size();
    Handle.Sim()->Collection.Geometries.push_back(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}

std::string GeometryRPCInterface::CylinderCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Geometry/Cylinder/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Cylinder Object
    Geometries::Cylinder S;
    if ((!Handle.GetParFloat("Point1Radius_um", S.End0Radius_um))
        || (!Handle.GetParVec3("Point1Pos", S.End0Pos_um))
        || (!Handle.GetParFloat("Point2Radius_um", S.End1Radius_um))
        || (!Handle.GetParVec3("Point2Pos", S.End1Pos_um))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    Handle.Sim()->Collection.Geometries.push_back(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}

std::string GeometryRPCInterface::BoxCreate(std::string _JSONRequest) {
 
    API::HandlerData Handle(_JSONRequest, Logger_, "Simulation/Geometry/Box/Create", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build New Box Object
    Geometries::Box S;
    if ((!Handle.GetParVec3("CenterPos", S.Center_um))
        || (!Handle.GetParVec3("Scale", S.Dims_um))
        || (!Handle.GetParVec3("Rotation", S.Rotations_rad, "rad"))
        || (!Handle.GetParString("Name", S.Name))) {
        return Handle.ErrResponse();
    }

    S.ID = Handle.Sim()->Collection.Geometries.size(); // we can do this since the shapes vector is a variant
    Handle.Sim()->Collection.Geometries.push_back(S);

    // Return Result ID
    return Handle.ResponseWithID("ShapeID", S.ID);
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
