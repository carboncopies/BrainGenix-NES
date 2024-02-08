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





namespace BG {
namespace NES {
namespace Simulator {





bool RenderVisualization(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VisualizerParameters* _Params, std::string _Filepath) {
    assert(_Logger != nullptr && "You've passed an invalid pointer to _Logger");
    assert(_Renderer != nullptr && "You've passed an invalid pointer to _Renderer");
    assert(_Params != nullptr && "You've passed an invalid pointer to _Params");


    _Logger->Log("Rendering Simulation Visualization", 3);


    _Params->FileHandles.clear();


    // Render Once To Fix Image Resolution Issue
    BG::NES::Renderer::Image RenderedImage;
    RenderedImage.TargetFileName_ = _Filepath;
    _Renderer->RenderImage(&RenderedImage);




    // Iterate through all items of the vector and render them
    for (size_t i = 0; i < _Params->CameraPositionList_um.size(); i++) {

        // Get Parameters from _Params, ensure renderer is properly setup for them.
        _Renderer->SetCameraFOV(_Params->FOVList_deg[i]);
        _Renderer->SetResolution(_Params->ImageWidth_px, _Params->ImageHeight_px);
        _Renderer->UpdateCameraPosition(_Params->CameraPositionList_um[i], _Params->CameraLookAtPositionList_um[i]);


        // Now Render The Thing
        BG::NES::Renderer::Image RenderedImage;
        RenderedImage.TargetFileName_ = _Filepath + std::to_string(_Params->LastImageNumber++) + ".png";
        _Renderer->RenderImage(&RenderedImage);


        // Get Image Properties, Write
        int SourceX = RenderedImage.Width_px;
        int SourceY = RenderedImage.Height_px;
        int Channels = RenderedImage.NumChannels_;
        unsigned char* SourcePixels = RenderedImage.Data_.get();
        stbi_write_png(RenderedImage.TargetFileName_.c_str(), SourceX, SourceY, Channels, SourcePixels, SourceX * Channels);
        _Logger->Log("Wrote Visualization Image To File '" + RenderedImage.TargetFileName_ + "'", 4);

        _Params->FileHandles.push_back(RenderedImage.TargetFileName_);

    }

    return true;

}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
