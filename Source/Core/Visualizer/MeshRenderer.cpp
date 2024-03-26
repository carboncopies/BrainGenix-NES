//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Visualizer/MeshRenderer.h>

#include <Visualizer/ImageProcessorPool/ProcessingTask.h>




namespace BG {
namespace NES {
namespace Simulator {





bool RenderVisualization(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VisualizerParameters* _Params, std::string _Filepath, Visualizer::ImageProcessorPool* _ImageProcessorPool) {
    assert(_Logger != nullptr && "You've passed an invalid pointer to _Logger");
    assert(_Renderer != nullptr && "You've passed an invalid pointer to _Renderer");
    assert(_Params != nullptr && "You've passed an invalid pointer to _Params");


    _Logger->Log("Rendering Simulation Visualization", 3);


    _Params->FileHandles.clear();


    // Render Once To Fix Image Resolution Issue
    BG::NES::Renderer::Image TmpImg;
    TmpImg.TargetFileName_ = _Filepath;
    _Renderer->RenderImage(&TmpImg);




    // Iterate through all items of the vector and render them
    std::vector<std::unique_ptr<Visualizer::ProcessingTask>> Tasks;
    for (size_t i = 0; i < _Params->CameraPositionList_um.size(); i++) {

        // Get Parameters from _Params, ensure renderer is properly setup for them.
        _Renderer->SetCameraFOV(_Params->FOVList_deg[i]);
        _Renderer->SetResolution(_Params->ImageWidth_px, _Params->ImageHeight_px);
        _Renderer->UpdateCameraPosition(_Params->CameraPositionList_um[i], _Params->CameraLookAtPositionList_um[i]);


        // Now Render The Thing
        Tasks.push_back(std::make_unique<Visualizer::ProcessingTask>());
        Visualizer::ProcessingTask* Task = Tasks[Tasks.size()-1].get();
        Task->Image_.TargetFileName_ = _Filepath + std::to_string(_Params->LastImageNumber++) + ".png";
        Task->TargetFileName_ = Task->Image_.TargetFileName_;
        _Renderer->RenderImage(&Task->Image_);
        _Params->FileHandles.push_back(Task->Image_.TargetFileName_);

        _ImageProcessorPool->QueueEncodeOperation(Task);
        _Logger->Log("Enqueued Visualization Image To File '" + Task->Image_.TargetFileName_ + "'", 4);


    }

    // Now Ensure That All Images Are Flushed To Disk
    for (size_t i = 0; i < Tasks.size(); i++) {
        while (Tasks[i]->IsDone_ != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }


    return true;

}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
