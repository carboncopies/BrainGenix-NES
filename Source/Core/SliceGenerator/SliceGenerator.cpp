#include <SliceGenerator/SliceGenerator.h>





namespace BG {
namespace NES {
namespace Simulator {



bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);


    // Build Bounding Boxes For All Compartments
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];

        Geometry * ThisShape = put something here;
        bb = ThisShape.GetBoundingBox();

        auto ThisShape = _Sim->Shapes.Shapes[ThisCompartment->ShapeID];

        std::variant<Shapes::Sphere, Shapes::Cylinder, Shapes::Box> ThisShape = _Sim->Shapes.Shapes[ThisCompartment->ShapeID];

        if (std::holds_alternative<Shapes::Sphere>(ThisShape)) {
            // Shapes::Sphere ThisSphere = ThisShape;
            

        }

    }

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
