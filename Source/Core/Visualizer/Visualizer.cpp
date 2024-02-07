#include <Visualizer/Visualizer.h>



namespace BG {
namespace NES {
namespace Simulator {


bool VisualizeSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation, std::string* _OutputPath, VisualizerParameters* _Params) {
    assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
    assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
    assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

    _Logger->Log("Rendering Visualization For Simulation " + _Simulation->Name, 5);
   

    // -- Process Description --
    // This is going to have a few steps, firstly, we're just going to build the mesh of the simulation
    // this involves enumerating all the BS compartments in the simulation in order to create them on vulkan
    // Next, we'll then render this all on the GPU with a call to MeshRenderer 



    // -- Stage 1 --
    // Here, we're going to build the mesh using the meshrenderer
    BuildMeshFromSimulation(_Logger, _Renderer, _Simulation);


    // -- Stage 2 --
    // Now, we're just going to render it to a file, and get that path back.



    return true;

}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG