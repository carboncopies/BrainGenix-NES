#include <Visualizer/Visualizer.h>



namespace BG {
namespace NES {
namespace Simulator {





// Returns:
//   true upon success.
//   false upon failure, and set the std::error_code & err accordingly.
// https://stackoverflow.com/questions/71658440/c17-create-directories-automatically-given-a-file-path
bool CreateDirectoryRecursive(std::string const & dirName, std::error_code & err) {
    err.clear();
    if (!std::filesystem::create_directories(dirName, err))
    {
        if (std::filesystem::exists(dirName))
        {
            // The folder already exists:
            err.clear();
            return true;    
        }
        return false;
    }
    return true;
}




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
    std::string TargetDirectory = "Visualizations/" + std::to_string(_Simulation->ID) + "/"
    std::error_code Code;
    if (!CreateDirectoryRecursive(TargetDirectory, Code)) {
        Logger_ ->Log("Failed To Create Directory, Error '" + Code.message() + "'", 7);
    }
    std::string Filepath = TargetDirectory + std::to_string(rand()) + ".png";
    RenderVisualization(_Logger, _Renderer, _Params, Filepath);


    return true;

}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG