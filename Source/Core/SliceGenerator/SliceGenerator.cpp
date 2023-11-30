#include <SliceGenerator/SliceGenerator.h>





namespace BG {
namespace NES {
namespace Simulator {



bool FillBoundingBox(VoxelArray* _Array, Shapes::BoundingBox* _BB, float _VoxelScale) {


    for (float X = _BB->bb_point1[0]; X < _BB->bb_point2[0]; X+= _VoxelScale) {
        for (float Y = _BB->bb_point1[1]; Y < _BB->bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = _BB->bb_point1[2]; Z < _BB->bb_point2[2]; Z+= _VoxelScale) {
                _Array->SetVoxelAtPosition(X, Y, Z, _VoxelScale);
            }
        }
    }

    return true;

}

bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);


    // Build Bounding Boxes For All Compartments
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];
        std::variant<Shapes::Sphere, Shapes::Cylinder, Shapes::Box> ThisShape = _Sim->Shapes.Shapes[ThisCompartment->ShapeID];


        if (std::holds_alternative<Shapes::Sphere>(ThisShape)) {
            Shapes::Sphere ThisSphere = std::get<Shapes::Sphere>(ThisShape);
            Shapes::BoundingBox BB = ThisSphere.GetBoundingBox();

            FillBoundingBox(_Array, &BB, _Params->VoxelResolution_um);


        }

    }

    return true;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
