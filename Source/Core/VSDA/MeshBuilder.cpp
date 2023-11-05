#include <VSDA/MeshBuilder.h>



namespace BG {
namespace NES {
namespace Simulator {

bool BuildMeshFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation) {
    assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
    assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
    assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

    _Logger->Log("Building 3D Mesh For Simulation " + _Simulation->Name, 5);

    // Enumerate Simulation Primitives
    for (unsigned int i = 0; i < _Simulation->BSCompartments.size(); i++) {
        int ShapeID = _Simulation->BSCompartments[i].ShapeID;

    }

    return false;

}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG