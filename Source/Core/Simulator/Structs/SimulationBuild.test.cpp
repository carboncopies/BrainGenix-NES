//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Tests of the Simulation build API: Add and Find functions for shapes, compartments,
                 neurons, receptors, circuits and regions, dense 0-based IDs, neuron-class
                 compatibility, compartment-to-neuron lookup and parent-UID registration
                 (dev_frame NT-5). Tests only, no production code changes.
                 Suites ending in KnownBug assert the CORRECT behaviour of confirmed bugs
                 and carry the ctest label known_bug (dev_frame D8).
*/

#include <climits>
#include <cmath>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <BG/Common/Logger/Logger.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/SimulationTestHelpers.h>
#include <TestUtil/TestUtil.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::Simulator::SimTest;

namespace {

struct SimulationBuild : BG::NES::TestUtil::TempOutputRoot {
    std::unique_ptr<Simulation> Sim;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Sim = std::make_unique<Simulation>(Logger_.get());
    }
    void TearDown() override {
        Sim.reset();
        TempOutputRoot::TearDown();
    }

    // Counts that must not move when an Add* call is rejected.
    struct Counts {
        size_t Shapes, BSComps, LIFCComps, Neurons, ByComp, Receptors, LIFCReceptors, ReceptorData, LIFCReceptorData;
        bool operator==(const Counts& o) const {
            return Shapes == o.Shapes && BSComps == o.BSComps && LIFCComps == o.LIFCComps && Neurons == o.Neurons &&
                   ByComp == o.ByComp && Receptors == o.Receptors && LIFCReceptors == o.LIFCReceptors &&
                   ReceptorData == o.ReceptorData && LIFCReceptorData == o.LIFCReceptorData;
        }
    };
    Counts Snapshot() const {
        return {Sim->Collection.Size(), Sim->BSCompartments.size(), Sim->LIFCCompartments.size(), Sim->Neurons.size(),
                Sim->NeuronByCompartment.size(), Sim->Receptors.size(), Sim->LIFCReceptors.size(),
                Sim->ReceptorDataVec.size(), Sim->LIFCReceptorDataVec.size()};
    }
};

struct SimulationBuildKnownBug : SimulationBuild {};

} // namespace

// ---------------------------------------------------------------- shapes

TEST_F(SimulationBuild, ShapesGetDenseZeroBasedIdsAcrossTypes) {
    EXPECT_EQ(AddSphereAt(*Sim, 1, 2, 3, 4), 0);
    EXPECT_EQ(AddBoxAt(*Sim, 10, 0, 0), 1);
    EXPECT_EQ(AddCylinderAt(*Sim, 20, 0, 0), 2);
    EXPECT_EQ(AddSphereAt(*Sim, 30, 0, 0), 3);

    ASSERT_EQ(Sim->Collection.Size(), 4u);
    EXPECT_EQ(Sim->Collection.GetShapeType(0), Geometries::GeometrySphere);
    EXPECT_EQ(Sim->Collection.GetShapeType(1), Geometries::GeometryBox);
    EXPECT_EQ(Sim->Collection.GetShapeType(2), Geometries::GeometryCylinder);
    EXPECT_EQ(Sim->Collection.GetShapeType(3), Geometries::GeometrySphere);

    for (int i = 0; i < 4; i++) {
        Geometries::Geometry* G = Sim->FindShapeByID(i);
        ASSERT_NE(G, nullptr) << "shape " << i;
        EXPECT_EQ(G->ID, i);
    }
    EXPECT_NEAR(Sim->FindShapeByID(0)->Center_um.x, 1.0f, 1e-6);
    EXPECT_NEAR(Sim->FindShapeByID(0)->Center_um.y, 2.0f, 1e-6);
    EXPECT_NEAR(Sim->FindShapeByID(0)->Center_um.z, 3.0f, 1e-6);
    EXPECT_NEAR(Sim->FindShapeByID(1)->Center_um.x, 10.0f, 1e-6);
}

TEST_F(SimulationBuild, FindShapeByIdRejectsInvalidIdsWithoutChangingState) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddBoxAt(*Sim, 5, 0, 0);
    const Counts Before = Snapshot();
    EXPECT_EQ(Sim->FindShapeByID(-1), nullptr);
    EXPECT_EQ(Sim->FindShapeByID(2), nullptr);
    EXPECT_EQ(Sim->FindShapeByID(INT_MAX), nullptr);
    EXPECT_EQ(Before, Snapshot());
}

TEST_F(SimulationBuild, FindShapeByIdOnEmptySimulationIsNull) {
    EXPECT_EQ(Sim->FindShapeByID(0), nullptr);
}

TEST_F(SimulationBuild, AddShapeOverwritesIdFieldOfTheArgument) {
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 1.0f);
    S.ID = 99;
    S.ParentID = 0;
    EXPECT_EQ(Sim->AddSphere(S), 0);
    EXPECT_EQ(S.ID, 0);
    EXPECT_EQ(Sim->FindShapeByID(0)->ID, 0);
}

// ---------------------------------------------------------------- circuits, regions

TEST_F(SimulationBuild, CircuitAndRegionIdsAreDenseAndRegionsOwnACircuit) {
    BG::NES::Simulator::CoreStructs::NeuralCircuit Standalone(&Sim->Collection);
    BG::NES::Simulator::BrainRegions::BrainRegion R0, R1;
    R0.SetName("cortex");
    R1.SetName("thalamus");

    EXPECT_EQ(Sim->AddRegion(R0), 0);       // adds circuit 0
    EXPECT_EQ(Sim->AddCircuit(Standalone), 1);
    EXPECT_EQ(Sim->AddRegion(R1), 1);       // adds circuit 2

    ASSERT_EQ(Sim->Regions.size(), 2u);
    ASSERT_EQ(Sim->NeuralCircuits.size(), 3u);
    for (int i = 0; i < 3; i++) EXPECT_EQ(Sim->NeuralCircuits[i]->ID, i);
    EXPECT_EQ(R0.CircuitID, 0);
    EXPECT_EQ(R1.CircuitID, 2);
    EXPECT_EQ(Sim->NeuralCircuits[0]->RegionID, 0);
    EXPECT_EQ(Sim->NeuralCircuits[2]->RegionID, 1);
    EXPECT_EQ(Sim->Regions[1]->Name(), "thalamus");
}

// ---------------------------------------------------------------- compartments

TEST_F(SimulationBuild, BSCompartmentsGetDenseIdsAndCacheTheirShape) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddCylinderAt(*Sim, 10, 0, 0);
    AddBoxAt(*Sim, 20, 0, 0);
    for (int shape = 0; shape < 3; shape++) {
        Compartments::BS C = MakeBSCompartment(shape);
        EXPECT_EQ(Sim->AddSCCompartment(C, BSNEURONS), shape);
    }
    ASSERT_EQ(Sim->BSCompartments.size(), 3u);
    EXPECT_EQ(Sim->GetNumCompartments(), 3u);
    for (int i = 0; i < 3; i++) {
        Compartments::BS* C = Sim->FindBSCompartmentByID(i);
        ASSERT_NE(C, nullptr);
        EXPECT_EQ(C->ID, i);
        EXPECT_EQ(C->ShapeID, i);
        EXPECT_EQ(C->ShapePtr, Sim->FindShapeByID(i));
        EXPECT_EQ(Sim->GetCompartmentByIdx(i), static_cast<Compartments::Compartment*>(C));
    }
    EXPECT_EQ(Sim->GetCompartmentByIdx(3), nullptr);
}

TEST_F(SimulationBuild, FindBSCompartmentByIdRejectsInvalidIds) {
    AddSphereAt(*Sim, 0, 0, 0);
    Compartments::BS C = MakeBSCompartment(0);
    ASSERT_EQ(Sim->AddSCCompartment(C, BSNEURONS), 0);
    const Counts Before = Snapshot();
    EXPECT_EQ(Sim->FindBSCompartmentByID(-1), nullptr);
    EXPECT_EQ(Sim->FindBSCompartmentByID(1), nullptr);
    EXPECT_EQ(Before, Snapshot());
}

TEST_F(SimulationBuild, BSCompartmentWithUnknownShapeIsRejectedWithoutChangingState) {
    AddSphereAt(*Sim, 0, 0, 0);
    const Counts Before = Snapshot();
    Compartments::BS Missing = MakeBSCompartment(7);
    Compartments::BS Negative = MakeBSCompartment(-1);
    EXPECT_EQ(Sim->AddSCCompartment(Missing, BSNEURONS), -1);
    EXPECT_EQ(Sim->AddSCCompartment(Negative, BSNEURONS), -1);
    EXPECT_EQ(Before, Snapshot());

    // A later valid compartment still gets ID 0 (no ID consumed by the rejected ones).
    Compartments::BS Ok = MakeBSCompartment(0);
    EXPECT_EQ(Sim->AddSCCompartment(Ok, BSNEURONS), 0);
}

TEST_F(SimulationBuild, LIFCCompartmentsGetDenseIdsAndRejectUnknownShapes) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddSphereAt(*Sim, 10, 0, 0);
    Compartments::LIFC C0 = MakeLIFCCompartment(1);
    Compartments::LIFC C1 = MakeLIFCCompartment(0);
    EXPECT_EQ(Sim->AddLIFCCompartment(C0), 0);
    EXPECT_EQ(Sim->AddLIFCCompartment(C1), 1);
    EXPECT_EQ(Sim->LIFCCompartments[0].ShapePtr, Sim->FindShapeByID(1));
    EXPECT_EQ(Sim->LIFCCompartments[1].ShapePtr, Sim->FindShapeByID(0));
    EXPECT_EQ(Sim->GetNumCompartments(), 2u);
    ASSERT_NE(Sim->GetCompartmentByIdx(1), nullptr);
    EXPECT_EQ(Sim->GetCompartmentByIdx(2), nullptr);

    const Counts Before = Snapshot();
    Compartments::LIFC Bad = MakeLIFCCompartment(5);
    EXPECT_EQ(Sim->AddLIFCCompartment(Bad), -1);
    EXPECT_EQ(Before, Snapshot());
}

// ---------------------------------------------------------------- neuron class compatibility

TEST_F(SimulationBuild, FirstAddFixesTheNeuronClass) {
    EXPECT_EQ(Sim->SimNeuronClass, UNDETERMINED);
    EXPECT_TRUE(Sim->CheckCompatibility(SCNEURONS));
    EXPECT_EQ(Sim->SimNeuronClass, SCNEURONS);
    EXPECT_TRUE(Sim->CheckCompatibility(SCNEURONS));
    EXPECT_FALSE(Sim->CheckCompatibility(BSNEURONS));
    EXPECT_FALSE(Sim->CheckCompatibility(LIFCNEURONS));
    EXPECT_EQ(Sim->SimNeuronClass, SCNEURONS) << "a rejected category must not change the class";
}

TEST_F(SimulationBuild, MixingCompartmentClassesIsRejected) {
    AddSphereAt(*Sim, 0, 0, 0);
    Compartments::BS B = MakeBSCompartment(0);
    ASSERT_EQ(Sim->AddSCCompartment(B, BSNEURONS), 0);

    Compartments::LIFC L = MakeLIFCCompartment(0);
    EXPECT_EQ(Sim->AddLIFCCompartment(L), -1);
    Compartments::BS S = MakeBSCompartment(0);
    EXPECT_EQ(Sim->AddSCCompartment(S, SCNEURONS), -1);
    EXPECT_EQ(Sim->BSCompartments.size(), 1u);
    EXPECT_TRUE(Sim->LIFCCompartments.empty());
    EXPECT_EQ(Sim->SimNeuronClass, BSNEURONS);
}

TEST_F(SimulationBuild, MixingNeuronClassesIsRejectedWithoutChangingState) {
    BuildBSNet(*Sim, 2);
    ASSERT_EQ(Sim->Neurons.size(), 2u);
    const Counts Before = Snapshot();

    CoreStructs::SCNeuronStruct SC = MakeSCNeuronStruct({0});
    CoreStructs::LIFCNeuronStruct LF = MakeLIFCNeuronStruct({0});
    EXPECT_EQ(Sim->AddSCNeuron(SC), -1);
    EXPECT_EQ(Sim->AddLIFCNeuron(LF), -1);
    EXPECT_EQ(Before, Snapshot());
    EXPECT_EQ(Sim->SimNeuronClass, BSNEURONS);
}

TEST_F(SimulationBuild, SCSimulationRejectsBSNeurons) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddCylinderAt(*Sim, 10, 0, 0);
    Compartments::BS C0 = MakeBSCompartment(0), C1 = MakeBSCompartment(1);
    ASSERT_EQ(Sim->AddSCCompartment(C0, SCNEURONS), 0);
    ASSERT_EQ(Sim->AddSCCompartment(C1, SCNEURONS), 1);
    CoreStructs::BSNeuronStruct N = MakeBSNeuronStruct(*Sim, 0, 1);
    EXPECT_EQ(Sim->AddBSNeuron(N), -1);
    EXPECT_TRUE(Sim->Neurons.empty());
    EXPECT_TRUE(Sim->NeuronByCompartment.empty());
}

TEST_F(SimulationBuild, EditNeuronRejectsWrongClassAndBadIds) {
    // BS simulation: SC/LIFC edits are refused.
    BuildBSNet(*Sim, 1);
    CoreStructs::SCNeuronStruct SCS = MakeSCNeuronStruct({0});
    CoreStructs::SCEdit SCE;
    SCE.RestingPotential_mV = true;
    EXPECT_FALSE(Sim->EditSCNeuron(0, SCS, SCE));
    CoreStructs::LIFCNeuronStruct LFS = MakeLIFCNeuronStruct({0});
    CoreStructs::LIFCEdit LFE;
    EXPECT_FALSE(Sim->EditLIFCNeuron(0, LFS, LFE));
}

// ---------------------------------------------------------------- BS neurons

TEST_F(SimulationBuild, BSNeuronsGetDenseIdsAndAreIndexedByTheirCompartments) {
    BuildBSNet(*Sim, 3);
    ASSERT_EQ(Sim->Neurons.size(), 3u);
    EXPECT_EQ(Sim->GetTotalNumberOfNeurons(), 3u);
    EXPECT_EQ(Sim->GetAllNeuronIDs(), (std::vector<size_t>{0, 1, 2}));
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(Sim->Neurons[i]->ID, i);
        EXPECT_EQ(Sim->Neurons[i]->Class_, CoreStructs::_BSNeuron);
        EXPECT_EQ(Sim->FindNeuronByCompartment(SomaComp(i)), Sim->Neurons[i].get());
        EXPECT_EQ(Sim->FindNeuronByCompartment(AxonComp(i)), Sim->Neurons[i].get());
        EXPECT_EQ(Sim->GetNeuronIndexByCompartment(SomaComp(i)), i);
        EXPECT_EQ(Sim->GetNeuronIndexByCompartment(AxonComp(i)), i);
    }
    EXPECT_EQ(Sim->NeuronByCompartment.size(), 6u);
    EXPECT_EQ(Sim->NeuronByCompartment.at(4), 2);
}

TEST_F(SimulationBuild, NeuronByCompartmentLookupsRejectUnknownCompartments) {
    BuildBSNet(*Sim, 2);
    const Counts Before = Snapshot();
    for (int Comp : {-1, 4, 5, 1000, INT_MAX, INT_MIN}) {
        EXPECT_EQ(Sim->FindNeuronByCompartment(Comp), nullptr) << Comp;
        EXPECT_EQ(Sim->GetNeuronIndexByCompartment(Comp), -1) << Comp;
    }
    EXPECT_EQ(Before, Snapshot());
}

TEST_F(SimulationBuild, GetNeuronsByIdsReturnsRequestedNeuronsInOrderAndSkipsUnknown) {
    BuildBSNet(*Sim, 3);
    auto Found = Sim->GetNeuronsByIDs({2, 0, 7, 1});
    ASSERT_EQ(Found.size(), 3u);
    EXPECT_EQ(Found[0].get(), Sim->Neurons[2].get());
    EXPECT_EQ(Found[1].get(), Sim->Neurons[0].get());
    EXPECT_EQ(Found[2].get(), Sim->Neurons[1].get());
    EXPECT_TRUE(Sim->GetNeuronsByIDs({}).empty());
    EXPECT_EQ(Sim->GetAllNeurons().size(), 3u);
}

TEST_F(SimulationBuild, BSNeuronParametersAreCopiedFromTheStruct) {
    BuildBSNet(*Sim, 1);
    auto* N = static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[0].get());
    EXPECT_NEAR(N->VRest_mV, -60.0f, 1e-6);
    EXPECT_NEAR(N->VAct_mV, -50.0f, 1e-6);
    EXPECT_NEAR(N->TauAHP_ms, 30.0f, 1e-6);
    EXPECT_EQ(N->Morphology.at("soma"), Sim->FindShapeByID(0));
    EXPECT_EQ(N->Morphology.at("axon"), Sim->FindShapeByID(1));
}

// ---------------------------------------------------------------- SC neurons

TEST_F(SimulationBuild, SCNeuronMapsAllItsCompartmentsAndClassIsSC) {
    for (int i = 0; i < 5; i++) AddSphereAt(*Sim, 10.0f * i, 0, 0);
    for (int i = 0; i < 5; i++) {
        Compartments::BS C = MakeBSCompartment(i);
        ASSERT_EQ(Sim->AddSCCompartment(C, SCNEURONS), i);
    }
    CoreStructs::SCNeuronStruct N0 = MakeSCNeuronStruct({0, 1}, {2}, {3});
    CoreStructs::SCNeuronStruct N1 = MakeSCNeuronStruct({4});
    EXPECT_EQ(Sim->AddSCNeuron(N0), 0);
    EXPECT_EQ(Sim->AddSCNeuron(N1), 1);

    EXPECT_EQ(Sim->Neurons[0]->Class_, CoreStructs::_SCNeuron);
    EXPECT_EQ(Sim->NeuronByCompartment.size(), 5u);
    for (int c : {0, 1, 2, 3}) EXPECT_EQ(Sim->GetNeuronIndexByCompartment(c), 0) << c;
    EXPECT_EQ(Sim->GetNeuronIndexByCompartment(4), 1);

    // Multi-compartment soma: cell centre is the mean of the soma shape centres (0 and 10).
    EXPECT_NEAR(Sim->Neurons[0]->GetCellCenter().x, 5.0f, 1e-5);
}

TEST_F(SimulationBuild, SCNeuronWithoutSomaIsRejectedWithoutChangingState) {
    AddSphereAt(*Sim, 0, 0, 0);
    Compartments::BS C = MakeBSCompartment(0);
    ASSERT_EQ(Sim->AddSCCompartment(C, SCNEURONS), 0);
    const Counts Before = Snapshot();
    CoreStructs::SCNeuronStruct N = MakeSCNeuronStruct({}, {0}, {});
    EXPECT_EQ(Sim->AddSCNeuron(N), -1);
    EXPECT_EQ(Before, Snapshot());
}

TEST_F(SimulationBuild, EditSCNeuronChangesOnlyTheFlaggedParametersAndChecksTheId) {
    AddSphereAt(*Sim, 0, 0, 0);
    Compartments::BS C = MakeBSCompartment(0);
    ASSERT_EQ(Sim->AddSCCompartment(C, SCNEURONS), 0);
    CoreStructs::SCNeuronStruct N = MakeSCNeuronStruct({0});
    ASSERT_EQ(Sim->AddSCNeuron(N), 0);

    CoreStructs::SCNeuronStruct New = MakeSCNeuronStruct({0});
    New.RestingPotential_mV = -65.0f;
    New.SpikeThreshold_mV = -40.0f;
    CoreStructs::SCEdit E;
    E.RestingPotential_mV = true; // threshold deliberately not flagged
    EXPECT_TRUE(Sim->EditSCNeuron(0, New, E));
    auto* SN = static_cast<SCNeuron*>(Sim->Neurons[0].get());
    EXPECT_NEAR(SN->VRest_mV, -65.0f, 1e-6);
    EXPECT_NEAR(SN->VAct_mV, -50.0f, 1e-6);

    EXPECT_FALSE(Sim->EditSCNeuron(1, New, E));
    EXPECT_FALSE(Sim->EditSCNeuron(-1, New, E));
}

// ---------------------------------------------------------------- LIFC neurons

TEST_F(SimulationBuild, LIFCNeuronsGetDenseIdsMapCompartmentsAndRejectEmptySoma) {
    BuildLIFCNet(*Sim, 3);
    ASSERT_EQ(Sim->Neurons.size(), 3u);
    EXPECT_EQ(Sim->SimNeuronClass, LIFCNEURONS);
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(Sim->Neurons[i]->ID, i);
        EXPECT_EQ(Sim->Neurons[i]->Class_, CoreStructs::_LIFCNeuron);
        EXPECT_EQ(Sim->GetNeuronIndexByCompartment(i), i);
    }
    EXPECT_NEAR(Sim->Neurons[2]->GetCellCenter().x, 200.0f, 1e-4);

    const Counts Before = Snapshot();
    CoreStructs::LIFCNeuronStruct Bad = MakeLIFCNeuronStruct({});
    EXPECT_EQ(Sim->AddLIFCNeuron(Bad), -1);
    EXPECT_EQ(Before, Snapshot());
}

TEST_F(SimulationBuild, EditLIFCNeuronChangesFlaggedParametersAndChecksTheId) {
    BuildLIFCNet(*Sim, 1);
    CoreStructs::LIFCNeuronStruct New = MakeLIFCNeuronStruct({0});
    New.RestingPotential_mV = -75.0f;
    CoreStructs::LIFCEdit E;
    E.RestingPotential_mV = true;
    EXPECT_TRUE(Sim->EditLIFCNeuron(0, New, E));
    EXPECT_NEAR(static_cast<LIFCNeuron*>(Sim->Neurons[0].get())->VRest_mV, -75.0f, 1e-6);
    EXPECT_FALSE(Sim->EditLIFCNeuron(1, New, E));
    EXPECT_FALSE(Sim->EditLIFCNeuron(-1, New, E));
}

// ---------------------------------------------------------------- BS receptors

TEST_F(SimulationBuild, BSReceptorsGetDenseIdsAndWireNeurons) {
    BuildBSNet(*Sim, 3);
    EXPECT_EQ(ConnectBS(*Sim, 0, 1), 0);
    EXPECT_EQ(ConnectBS(*Sim, 1, 2), 1);
    EXPECT_EQ(ConnectBS(*Sim, 0, 2, "GABA"), 2);

    EXPECT_EQ(Sim->GetNumReceptors(), 3u);
    ASSERT_EQ(Sim->Receptors.size(), 3u);
    ASSERT_EQ(Sim->ReceptorDataVec.size(), 3u);
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(Sim->Receptors[i]->ID, i);
        EXPECT_EQ(Sim->ReceptorDataVec[i]->ReceptorID, i);
        EXPECT_EQ(Sim->GetReceptorByIdx(i), static_cast<Connections::ReceptorCommonBase*>(Sim->Receptors[i].get()));
    }
    EXPECT_EQ(Sim->GetReceptorByIdx(3), nullptr);
    EXPECT_EQ(Sim->ReceptorDataVec[2]->SrcNeuronID, 0);
    EXPECT_EQ(Sim->ReceptorDataVec[2]->DstNeuronID, 2);

    auto* N0 = static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[0].get());
    auto* N2 = static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[2].get());
    EXPECT_EQ(N0->TransmitterDataVec.size(), 2u);
    EXPECT_EQ(N2->ReceptorDataVec.size(), 2u);
}

TEST_F(SimulationBuild, ReceptorNeurotransmitterSetsTheSourceNeuronTypeOnlyWhileUnknown) {
    BuildBSNet(*Sim, 3);
    ASSERT_EQ(Sim->Neurons[0]->Type_, CoreStructs::UnknownNeuron);
    ConnectBS(*Sim, 0, 1, "GABA");
    ConnectBS(*Sim, 1, 2, "AMPA");
    ConnectBS(*Sim, 0, 2, "AMPA"); // 0 is already an interneuron: type stays
    EXPECT_EQ(Sim->Neurons[0]->Type_, CoreStructs::GenericInterneuron);
    EXPECT_EQ(Sim->Neurons[1]->Type_, CoreStructs::GenericPrincipalNeuron);
    EXPECT_EQ(Sim->Neurons[2]->Type_, CoreStructs::UnknownNeuron);
}

TEST_F(SimulationBuild, BSReceptorWithUnknownCompartmentReturnsMinusOne) {
    BuildBSNet(*Sim, 2);
    Connections::Receptor BadSrc = MakeReceptor(99, SomaComp(1));
    Connections::Receptor BadDst = MakeReceptor(AxonComp(0), 99);
    Connections::Receptor Negative = MakeReceptor(-1, -1);
    EXPECT_EQ(Sim->AddReceptor(BadSrc), -1);
    EXPECT_EQ(Sim->AddReceptor(BadDst), -1);
    EXPECT_EQ(Sim->AddReceptor(Negative), -1);
    // Neurons were not wired and no functional record exists.
    EXPECT_TRUE(Sim->ReceptorDataVec.empty());
    EXPECT_TRUE(static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[0].get())->TransmitterDataVec.empty());
    EXPECT_TRUE(static_cast<BallAndStick::BSNeuron*>(Sim->Neurons[1].get())->ReceptorDataVec.empty());
}

// Bug: dev_frame #106
TEST_F(SimulationBuildKnownBug, RejectedBSReceptorLeavesNoOrphanBehind) {
    BuildBSNet(*Sim, 2);
    Connections::Receptor Bad = MakeReceptor(99, SomaComp(1));
    ASSERT_EQ(Sim->AddReceptor(Bad), -1);
    EXPECT_EQ(Sim->Receptors.size(), 0u);
    EXPECT_EQ(Sim->GetNumReceptors(), 0u);
}

// Bug: dev_frame #106
TEST_F(SimulationBuildKnownBug, ReceptorIdAfterARejectedBSReceptorIsStillDense) {
    BuildBSNet(*Sim, 2);
    Connections::Receptor Bad = MakeReceptor(AxonComp(0), 99);
    ASSERT_EQ(Sim->AddReceptor(Bad), -1);
    EXPECT_EQ(ConnectBS(*Sim, 0, 1), 0);
    ASSERT_EQ(Sim->ReceptorDataVec.size(), 1u);
    EXPECT_EQ(Sim->ReceptorDataVec[0]->ReceptorPtr, Sim->Receptors.at(Sim->ReceptorDataVec[0]->ReceptorID).get());
}

// ---------------------------------------------------------------- LIFC receptors

TEST_F(SimulationBuild, LIFCReceptorsGetDenseIdsAndLookUpByIndex) {
    BuildLIFCNet(*Sim, 3);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 1), 0);
    EXPECT_EQ(ConnectLIFC(*Sim, 1, 2, Connections::GABA), 1);
    EXPECT_EQ(Sim->GetNumReceptors(), 2u);
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(Sim->LIFCReceptors[i]->ID, i);
        EXPECT_EQ(Sim->GetReceptorByIdx(i), static_cast<Connections::ReceptorCommonBase*>(Sim->LIFCReceptors[i].get()));
    }
    EXPECT_EQ(Sim->GetReceptorByIdx(2), nullptr);
    EXPECT_EQ(Sim->Neurons[0]->Type_, CoreStructs::GenericPrincipalNeuron);
    EXPECT_EQ(Sim->Neurons[1]->Type_, CoreStructs::GenericInterneuron);
    EXPECT_EQ(Sim->Neurons[2]->Type_, CoreStructs::UnknownNeuron);
}

TEST_F(SimulationBuild, AbstractedLIFCReceptorsOfOnePairAndTypeAreMerged) {
    BuildLIFCNet(*Sim, 2);
    ASSERT_TRUE(Sim->use_abstracted_LIF_receptors);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.5f), 0);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.25f), 1);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 1, Connections::GABA, 0.125f), 2);

    ASSERT_EQ(Sim->LIFCReceptorDataVec.size(), 2u) << "AMPA pair merged, GABA separate";
    auto& AMPA = *Sim->LIFCReceptorDataVec[0];
    EXPECT_EQ(AMPA.ReceptorIDs, (std::vector<int>{0, 1}));
    EXPECT_NEAR(AMPA.g_peak_sum_nS, 0.75f, 1e-6);
    EXPECT_NEAR(AMPA.weight, 1.0f, 1e-6);
    EXPECT_EQ(Sim->LIFCReceptorDataVec[1]->ReceptorIDs, (std::vector<int>{2}));
    EXPECT_EQ(Sim->LIFCReceptors.size(), 3u);
}

TEST_F(SimulationBuild, NonAbstractedLIFCReceptorsKeepOneFunctionalRecordEach) {
    BuildLIFCNet(*Sim, 2);
    Sim->use_abstracted_LIF_receptors = false;
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.5f);
    ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.25f);
    ASSERT_EQ(Sim->LIFCReceptorDataVec.size(), 2u);
    EXPECT_EQ(Sim->LIFCReceptorDataVec[0]->ReceptorIDs.size(), 1u);
    EXPECT_EQ(Sim->LIFCReceptorDataVec[1]->ReceptorIDs.size(), 1u);
}

TEST_F(SimulationBuild, LIFCReceptorWithUnknownCompartmentReturnsMinusOne) {
    BuildLIFCNet(*Sim, 2);
    EXPECT_EQ(ConnectLIFC(*Sim, 99, 1), -1);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 99), -1);
    EXPECT_TRUE(Sim->LIFCReceptorDataVec.empty());
}

// Bug: dev_frame #106
TEST_F(SimulationBuildKnownBug, RejectedLIFCReceptorLeavesNoOrphanBehind) {
    BuildLIFCNet(*Sim, 2);
    ASSERT_EQ(ConnectLIFC(*Sim, 99, 1), -1);
    EXPECT_EQ(Sim->LIFCReceptors.size(), 0u);
    EXPECT_EQ(ConnectLIFC(*Sim, 0, 1), 0);
}

TEST_F(SimulationBuild, NetmorphLIFCReceptorDerivesPeakConductance) {
    BuildLIFCNet(*Sim, 2);
    Connections::LIFCReceptor R = MakeLIFCReceptor(0, 1);
    Connections::NetmorphLIFCReceptorRaw Raw;
    Raw.ReceptorPeakConductance_nS = 0.1f;
    Raw.ReceptorQuantity = 5;
    Raw.HillocDistance_um = 0.0f;
    Raw.Velocity_mps = 1.0f;
    Raw.SynapticDelay_ms = 0.5f;
    EXPECT_EQ(Sim->AddNetmorphLIFCReceptor(R, Raw), 0);
    EXPECT_NEAR(Sim->LIFCReceptors[0]->PeakConductance_nS, 0.5f, 1e-6);
    EXPECT_NEAR(Sim->LIFCReceptors[0]->OnsetDelay_ms, 0.5f, 1e-6);
}

// Bug: dev_frame #107
TEST_F(SimulationBuildKnownBug, NetmorphOnsetDelayAddsTheAxonalConductionTimeInMilliseconds) {
    BuildLIFCNet(*Sim, 2);
    Connections::LIFCReceptor R = MakeLIFCReceptor(0, 1);
    Connections::NetmorphLIFCReceptorRaw Raw;
    Raw.ReceptorPeakConductance_nS = 0.1f;
    Raw.ReceptorQuantity = 5;
    Raw.HillocDistance_um = 100.0f; // 100 um at 1 m/s is 0.1 ms
    Raw.Velocity_mps = 1.0f;
    Raw.SynapticDelay_ms = 0.5f;
    ASSERT_EQ(Sim->AddNetmorphLIFCReceptor(R, Raw), 0);
    EXPECT_NEAR(Sim->LIFCReceptors[0]->OnsetDelay_ms, 0.6f, 1e-4);
}

// Bug: dev_frame #108
TEST_F(SimulationBuildKnownBug, MergingZeroConductanceLIFCReceptorsKeepsAFiniteWeight) {
    BuildLIFCNet(*Sim, 2);
    ASSERT_EQ(ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.0f), 0);
    ASSERT_EQ(ConnectLIFC(*Sim, 0, 1, Connections::AMPA, 0.0f), 1);
    ASSERT_EQ(Sim->LIFCReceptorDataVec.size(), 1u);
    EXPECT_TRUE(std::isfinite(Sim->LIFCReceptorDataVec[0]->weight));
}

// ---------------------------------------------------------------- parent-UID registration

TEST_F(SimulationBuild, RegisterNeuronUidSetsUidPlusOneOnEveryShapeType) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddCylinderAt(*Sim, 1, 0, 0);
    AddBoxAt(*Sim, 2, 0, 0);
    AddSphereAt(*Sim, 3, 0, 0);
    Sim->RegisterNeuronUIDToCompartments({0, 1, 2}, 7);
    EXPECT_EQ(Sim->Collection.GetSphere(0).ParentID, 8u);
    EXPECT_EQ(Sim->Collection.GetCylinder(1).ParentID, 8u);
    EXPECT_EQ(Sim->Collection.GetBox(2).ParentID, 8u);
    EXPECT_EQ(Sim->Collection.GetSphere(3).ParentID, 0u) << "shape not listed must be untouched";
}

TEST_F(SimulationBuild, RegisterNeuronUidIgnoresOutOfRangeIds) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddSphereAt(*Sim, 1, 0, 0);
    EXPECT_NO_THROW(Sim->RegisterNeuronUIDToCompartments({5, 1000, -1, INT_MIN}, 3));
    EXPECT_EQ(Sim->Collection.GetSphere(0).ParentID, 0u);
    EXPECT_EQ(Sim->Collection.GetSphere(1).ParentID, 0u);
    EXPECT_NO_THROW(Sim->RegisterNeuronUIDToCompartments({}, 3));
}

TEST_F(SimulationBuild, SCNeuronRegistersItsUidOnCompartmentShapesWhenIdsCoincide) {
    for (int i = 0; i < 3; i++) AddSphereAt(*Sim, 10.0f * i, 0, 0);
    for (int i = 0; i < 3; i++) {
        Compartments::BS C = MakeBSCompartment(i);
        ASSERT_EQ(Sim->AddSCCompartment(C, SCNEURONS), i);
    }
    CoreStructs::SCNeuronStruct N0 = MakeSCNeuronStruct({0}, {1});
    CoreStructs::SCNeuronStruct N1 = MakeSCNeuronStruct({2});
    ASSERT_EQ(Sim->AddSCNeuron(N0), 0);
    ASSERT_EQ(Sim->AddSCNeuron(N1), 1);
    EXPECT_EQ(Sim->Collection.GetSphere(0).ParentID, 1u);
    EXPECT_EQ(Sim->Collection.GetSphere(1).ParentID, 1u);
    EXPECT_EQ(Sim->Collection.GetSphere(2).ParentID, 2u);
}

// Bug: dev_frame #109
TEST_F(SimulationBuildKnownBug, BSNeuronRegistersItsUidOnItsSomaAndAxonShapes) {
    BuildBSNet(*Sim, 2); // neuron i: soma shape 2i, axon shape 2i+1
    for (int n = 0; n < 2; n++) {
        EXPECT_EQ(Sim->Collection.GetSphere(2 * n).ParentID, uint64_t(n + 1)) << "soma of neuron " << n;
        EXPECT_EQ(Sim->Collection.GetCylinder(2 * n + 1).ParentID, uint64_t(n + 1)) << "axon of neuron " << n;
    }
}

// Bug: dev_frame #110
TEST_F(SimulationBuildKnownBug, RegisterNeuronUidWithIdEqualToShapeCountIsIgnoredNotThrown) {
    AddSphereAt(*Sim, 0, 0, 0);
    AddSphereAt(*Sim, 1, 0, 0);
    EXPECT_NO_THROW(Sim->RegisterNeuronUIDToCompartments({2}, 3));
}

// Bug: dev_frame #111
TEST_F(SimulationBuildKnownBug, SCNeuronRegistersUidOnTheShapeOfItsCompartmentNotTheCompartmentIdAsShapeId) {
    for (int i = 0; i < 4; i++) AddSphereAt(*Sim, 10.0f * i, 0, 0);
    Compartments::BS C0 = MakeBSCompartment(3); // compartment 0 uses shape 3
    ASSERT_EQ(Sim->AddSCCompartment(C0, SCNEURONS), 0);
    CoreStructs::SCNeuronStruct N = MakeSCNeuronStruct({0});
    ASSERT_EQ(Sim->AddSCNeuron(N), 0);
    EXPECT_EQ(Sim->Collection.GetSphere(3).ParentID, 1u);
    EXPECT_EQ(Sim->Collection.GetSphere(0).ParentID, 0u);
}

// Bug: dev_frame #112
TEST_F(SimulationBuildKnownBug, CompartmentShapePointerSurvivesLaterShapesBeingAdded) {
    AddSphereAt(*Sim, 0, 0, 0);
    Compartments::BS C = MakeBSCompartment(0);
    ASSERT_EQ(Sim->AddSCCompartment(C, BSNEURONS), 0);
    for (int i = 1; i <= 200; i++) AddSphereAt(*Sim, float(i), 0, 0);
    EXPECT_EQ(Sim->BSCompartments[0].ShapePtr, Sim->FindShapeByID(0));
}
