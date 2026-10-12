//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the Simulation/Geometry/{Sphere,Cylinder,Box}/Create route
                 handlers (GeometryRPCInterface.cpp), called directly with JSON strings:
                 valid requests and the resulting simulation state, sequential ShapeIDs,
                 every required field missing or wrongly typed, unknown / busy simulation.
                 (dev_frame NT-11)
*/

#include <functional>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <RPC/APIStatusCode.h>
#include <TestUtil/RouteHarness.h>

using nlohmann::json;
using BG::NES::TestUtil::RouteHarness;
namespace Geo = BG::NES::Simulator::Geometries;

namespace {

struct GeometryRoute {
    std::string Name;
    std::function<std::string(const std::string&)> Call;
    json Valid;
    std::vector<std::string> Required;  // every key except SimulationID
};

class GeometryRouteTest : public ::testing::Test {
protected:
    RouteHarness& H() { return RouteHarness::Get(); }

    std::vector<GeometryRoute> Routes() {
        auto& G = H().Geometry();
        return {
            {"Sphere", [&G](const std::string& R) { return G.SphereCreate(R); },
             {{"Radius_um", 2.5}, {"CenterPosX_um", 1.0}, {"CenterPosY_um", 2.0}, {"CenterPosZ_um", 3.0}, {"Name", "s"}},
             {"Radius_um", "CenterPosX_um", "CenterPosY_um", "CenterPosZ_um", "Name"}},
            {"Cylinder", [&G](const std::string& R) { return G.CylinderCreate(R); },
             {{"Point1Radius_um", 1.0}, {"Point1PosX_um", 0.0}, {"Point1PosY_um", 0.0}, {"Point1PosZ_um", 0.0},
              {"Point2Radius_um", 2.0}, {"Point2PosX_um", 0.0}, {"Point2PosY_um", 0.0}, {"Point2PosZ_um", 10.0}, {"Name", "c"}},
             {"Point1Radius_um", "Point1PosX_um", "Point1PosY_um", "Point1PosZ_um",
              "Point2Radius_um", "Point2PosX_um", "Point2PosY_um", "Point2PosZ_um", "Name"}},
            {"Box", [&G](const std::string& R) { return G.BoxCreate(R); },
             {{"CenterPosX_um", 1.0}, {"CenterPosY_um", 2.0}, {"CenterPosZ_um", 3.0},
              {"ScaleX_um", 4.0}, {"ScaleY_um", 5.0}, {"ScaleZ_um", 6.0},
              {"RotationX_rad", 0.1}, {"RotationY_rad", 0.2}, {"RotationZ_rad", 0.3}, {"Name", "b"}},
             {"CenterPosX_um", "CenterPosY_um", "CenterPosZ_um", "ScaleX_um", "ScaleY_um", "ScaleZ_um",
              "RotationX_rad", "RotationY_rad", "RotationZ_rad", "Name"}},
        };
    }

    static json With(json Valid, int SimID) {
        Valid["SimulationID"] = SimID;
        return Valid;
    }
    static json Parse(const std::string& S) { return json::parse(S); }
};

TEST_F(GeometryRouteTest, SphereCreateStoresTheSphereAndReturnsItsId) {
    int Sim = H().NewSim();
    auto R = Routes()[0];
    json Resp = Parse(R.Call(With(R.Valid, Sim).dump()));
    EXPECT_EQ(Resp, json({{"StatusCode", 0}, {"ShapeID", 0}}));

    auto* S = H().Sim(Sim);
    ASSERT_EQ(S->Collection.Size(), 1u);
    ASSERT_EQ(S->Collection.GetShapeType(0), Geo::GeometrySphere);
    const Geo::Sphere& Sp = S->Collection.GetSphere(0);
    EXPECT_NEAR(Sp.Radius_um, 2.5f, 1e-6);
    EXPECT_NEAR(Sp.Center_um.x, 1.0f, 1e-6);
    EXPECT_NEAR(Sp.Center_um.y, 2.0f, 1e-6);
    EXPECT_NEAR(Sp.Center_um.z, 3.0f, 1e-6);
    EXPECT_EQ(Sp.Name, "s");
    EXPECT_EQ(Sp.ID, 0);
}

TEST_F(GeometryRouteTest, CylinderCreateStoresTheCylinderAndReturnsItsId) {
    int Sim = H().NewSim();
    auto R = Routes()[1];
    json Resp = Parse(R.Call(With(R.Valid, Sim).dump()));
    EXPECT_EQ(Resp, json({{"StatusCode", 0}, {"ShapeID", 0}}));

    auto* S = H().Sim(Sim);
    ASSERT_EQ(S->Collection.Size(), 1u);
    ASSERT_EQ(S->Collection.GetShapeType(0), Geo::GeometryCylinder);
    const Geo::Cylinder& C = S->Collection.GetCylinder(0);
    EXPECT_NEAR(C.End0Radius_um, 1.0f, 1e-6);
    EXPECT_NEAR(C.End1Radius_um, 2.0f, 1e-6);
    EXPECT_NEAR(C.End0Pos_um.z, 0.0f, 1e-6);
    EXPECT_NEAR(C.End1Pos_um.z, 10.0f, 1e-6);
    EXPECT_EQ(C.Name, "c");
}

TEST_F(GeometryRouteTest, BoxCreateStoresTheBoxAndReturnsItsId) {
    int Sim = H().NewSim();
    auto R = Routes()[2];
    json Resp = Parse(R.Call(With(R.Valid, Sim).dump()));
    EXPECT_EQ(Resp, json({{"StatusCode", 0}, {"ShapeID", 0}}));

    auto* S = H().Sim(Sim);
    ASSERT_EQ(S->Collection.Size(), 1u);
    ASSERT_EQ(S->Collection.GetShapeType(0), Geo::GeometryBox);
    const Geo::Box& B = S->Collection.GetBox(0);
    EXPECT_NEAR(B.Center_um.x, 1.0f, 1e-6);
    EXPECT_NEAR(B.Dims_um.x, 4.0f, 1e-6);
    EXPECT_NEAR(B.Dims_um.y, 5.0f, 1e-6);
    EXPECT_NEAR(B.Dims_um.z, 6.0f, 1e-6);
    EXPECT_NEAR(B.Rotations_rad.x, 0.1f, 1e-6);
    EXPECT_NEAR(B.Rotations_rad.y, 0.2f, 1e-6);
    EXPECT_NEAR(B.Rotations_rad.z, 0.3f, 1e-6);
    EXPECT_EQ(B.Name, "b");
}

TEST_F(GeometryRouteTest, ShapeIdsAreSequentialAcrossShapeTypesAndPerSimulation) {
    int SimA = H().NewSim();
    int SimB = H().NewSim();
    auto Rs = Routes();
    // sphere, box, cylinder, sphere in A -> 0,1,2,3; one sphere in B starts again at 0
    int Order[] = {0, 2, 1, 0};
    int Expect = 0;
    for (int Idx : Order) {
        json Resp = Parse(Rs[Idx].Call(With(Rs[Idx].Valid, SimA).dump()));
        EXPECT_EQ(Resp["ShapeID"], Expect++) << Rs[Idx].Name;
    }
    EXPECT_EQ(Parse(Rs[0].Call(With(Rs[0].Valid, SimB).dump()))["ShapeID"], 0);
    EXPECT_EQ(H().Sim(SimA)->Collection.Size(), 4u);
    EXPECT_EQ(H().Sim(SimB)->Collection.Size(), 1u);
    EXPECT_EQ(H().Sim(SimA)->Collection.GetShapeType(1), Geo::GeometryBox);
    EXPECT_EQ(H().Sim(SimA)->Collection.GetShapeType(2), Geo::GeometryCylinder);
}

TEST_F(GeometryRouteTest, IntegerValuesAreAcceptedForFloatFields) {
    int Sim = H().NewSim();
    json Req = {{"SimulationID", Sim}, {"Radius_um", 3}, {"CenterPosX_um", 1}, {"CenterPosY_um", 2}, {"CenterPosZ_um", 3}, {"Name", "i"}};
    EXPECT_EQ(Parse(H().Geometry().SphereCreate(Req.dump()))["StatusCode"], 0);
    EXPECT_NEAR(H().Sim(Sim)->Collection.GetSphere(0).Radius_um, 3.0f, 1e-6);
}

TEST_F(GeometryRouteTest, EachMissingRequiredFieldIsStatus2AndAddsNoShape) {
    for (auto& R : Routes()) {
        int Sim = H().NewSim();
        for (const std::string& Key : R.Required) {
            json Req = With(R.Valid, Sim);
            Req.erase(Key);
            EXPECT_EQ(Parse(R.Call(Req.dump())), json({{"StatusCode", 2}})) << R.Name << " missing " << Key;
        }
        EXPECT_EQ(H().Sim(Sim)->Collection.Size(), 0u) << R.Name;
    }
}

TEST_F(GeometryRouteTest, EachWronglyTypedRequiredFieldIsStatus2AndAddsNoShape) {
    for (auto& R : Routes()) {
        int Sim = H().NewSim();
        for (const std::string& Key : R.Required) {
            // Name is a string, everything else numeric: swap the type of each in turn.
            json Wrong = (Key == "Name") ? json(5) : json("text");
            json Req = With(R.Valid, Sim);
            Req[Key] = Wrong;
            EXPECT_EQ(Parse(R.Call(Req.dump())), json({{"StatusCode", 2}})) << R.Name << " wrong type " << Key;
            Req[Key] = nullptr;
            EXPECT_EQ(Parse(R.Call(Req.dump())), json({{"StatusCode", 2}})) << R.Name << " null " << Key;
        }
        EXPECT_EQ(H().Sim(Sim)->Collection.Size(), 0u) << R.Name;
    }
}

TEST_F(GeometryRouteTest, MissingOrWronglyTypedSimulationIdIsStatus2) {
    for (auto& R : Routes()) {
        json NoId = R.Valid;
        EXPECT_EQ(Parse(R.Call(NoId.dump())), json({{"StatusCode", 2}})) << R.Name;
        json BadId = R.Valid;
        BadId["SimulationID"] = "0";
        EXPECT_EQ(Parse(R.Call(BadId.dump())), json({{"StatusCode", 2}})) << R.Name;
    }
}

TEST_F(GeometryRouteTest, UnknownSimulationIdIsStatus2) {
    int Sim = H().NewSim();
    for (auto& R : Routes()) {
        for (int Bad : {-1, Sim + 1000}) {
            EXPECT_EQ(Parse(R.Call(With(R.Valid, Bad).dump())), json({{"StatusCode", 2}})) << R.Name << " id " << Bad;
        }
    }
}

TEST_F(GeometryRouteTest, BusySimulationIsStatus5AndAddsNoShape) {
    for (auto& R : Routes()) {
        int Sim = H().NewSim();
        H().Sim(Sim)->IsProcessing = true;
        EXPECT_EQ(Parse(R.Call(With(R.Valid, Sim).dump())), json({{"StatusCode", 5}})) << R.Name;
        H().Sim(Sim)->IsProcessing = false;
        H().Sim(Sim)->WorkRequested = true;
        EXPECT_EQ(Parse(R.Call(With(R.Valid, Sim).dump())), json({{"StatusCode", 5}})) << R.Name << " (WorkRequested)";
        EXPECT_EQ(H().Sim(Sim)->Collection.Size(), 0u) << R.Name;
        H().Sim(Sim)->WorkRequested = false;
        // And it works again once the simulation is idle.
        EXPECT_EQ(Parse(R.Call(With(R.Valid, Sim).dump()))["StatusCode"], 0) << R.Name;
    }
}

TEST_F(GeometryRouteTest, RoutesAreRegisteredOnTheManager) {
    int Sim = H().NewSim();
    auto R = Routes()[0];
    json Item = With(R.Valid, Sim);
    json Batch = json::array({json({{"ReqID", 7}, {"Simulation/Geometry/Sphere/Create", Item}})});
    json Resp = json::parse(H().Manager().NESRequest(Batch.dump()));
    ASSERT_TRUE(Resp.is_array());
    ASSERT_EQ(Resp.size(), 1u);
    EXPECT_EQ(Resp[0]["StatusCode"], 0);
    EXPECT_EQ(Resp[0]["ShapeID"], 0);
    EXPECT_EQ(H().Sim(Sim)->Collection.Size(), 1u);
}

// ---- Known bug (dev_frame D8): asserts the CORRECT behaviour and fails today -----------

class GeometryRouteKnownBug : public GeometryRouteTest {};

// Bug: dev_frame #137
TEST_F(GeometryRouteKnownBug, RejectedRequestsAreNotJournaled) {
    int Sim = H().NewSim();
    auto R = Routes()[0];
    json Bad = With(R.Valid, Sim);
    Bad.erase("Radius_um");
    EXPECT_EQ(Parse(R.Call(Bad.dump())), json({{"StatusCode", 2}}));
    H().Sim(Sim)->IsProcessing = true;
    EXPECT_EQ(Parse(R.Call(With(R.Valid, Sim).dump())), json({{"StatusCode", 5}}));
    H().Sim(Sim)->IsProcessing = false;
    EXPECT_EQ(H().Sim(Sim)->NumStoredRequests(), 0u);
}

}  // namespace
