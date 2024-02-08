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

    



    // Get Parameters from _Params, ensure renderer is properly setup for them.
    _Renderer->SetCameraFOV(_Params->FOV_deg);
    _Renderer->SetResolution(_Params->ImageWidth_px, _Params->ImageHeight_px);
    _Renderer->UpdateCameraPosition(_Params->CameraPosition_um, _Params->CameraLookAtPosition_um);


    // Now Render The Thing
    BG::NES::Renderer::Image RenderedImage;
    RenderedImage.TargetFileName_ = _Filepath;
    _Renderer->RenderImage(&RenderedImage);
    _Renderer->RenderImage(&RenderedImage);




    // Get Image Properties, Write
    int SourceX = RenderedImage.Width_px;
    int SourceY = RenderedImage.Height_px;
    int Channels = RenderedImage.NumChannels_;
    unsigned char* SourcePixels = RenderedImage.Data_.get();
    stbi_write_png(RenderedImage.TargetFileName_.c_str(), SourceX, SourceY, Channels, SourcePixels, SourceX * Channels);
    _Logger->Log("Wrote Visualization Image To File '" + RenderedImage.TargetFileName_ + "'", 4);


    return true;

}




// double CalculateFrustumHeight_um(double _Distance, double _FOV) {
//     return 2. * _Distance * tan(_FOV * 0.5 * (M_PI / 180.));
// }

// double CalculateFrustumWidth_um(double _Distance, double _FOV, double _AspectRatio) {
//     return CalculateFrustumHeight_um(_Distance, _FOV) * _AspectRatio;
// }

// double CalculateCameraMovementStep_um(double _FrustumValue_um, double _Overlap_Percent) {
//     return _FrustumValue_um * (1. - (_Overlap_Percent / 100.));
// }


// bool VulkanRenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool) {
//     assert(_VSDAData != nullptr);
//     assert(_Logger != nullptr);


//     // Get Params and Array From VSDAData
//     MicroscopeParameters* Params = &_VSDAData->Params_;
//     VoxelArray* Array = _VSDAData->Array_.get();


//     _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);

//     _Renderer->ResetScene();

//     // Ensure Scene Is Now Threadsafe
//     _Renderer->LockScene();
//     _Renderer->WaitUntilGPUDone();

//     // Setup Voxel Info
//     float VoxelSize = Array->GetResolution();
//     BoundingBox VoxelBB = Array->GetBoundingBox();
    
//     _Renderer->UpdateCameraPosition(vsg::dvec3(4., 4., 5));


//     // Enumerate Slice, Build Cubes Where Needed
//     for (unsigned int X = 0; X < Array->GetX(); X++) {
//         for (unsigned int Y = 0; Y < Array->GetY(); Y++) {

//             // Get Voxel At The Specified Position
//             VoxelType ThisVoxel = Array->GetVoxel(X, Y, SliceNumber);
//             if (ThisVoxel != EMPTY) {


//                 // Build Shader with different color so we can tell each voxel apart
//                 Renderer::Shaders::Phong BoxShader;

//                 if (ThisVoxel == FILLED) {
//                     BoxShader.DiffuseColor_  = vsg::vec4(0.8f, 0.8f, 0.8f, 1.0f);//vsg::vec4(X/(float)Array->GetX(), Y/(float)Array->GetY(), 1.0, 1.0f);
//                     BoxShader.SpecularColor_ = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
//                 } else if (ThisVoxel == BORDER) {
//                     BoxShader.DiffuseColor_  = vsg::vec4(1.0, 0.5, 0.3, 1.0f);
//                     BoxShader.SpecularColor_ = vsg::vec4(0.f, 0.f, 0.f, 0.0f);
//                 }
//                 BoxShader.Type_ = Renderer::Shaders::SHADER_PHONG;


//                 // If It's Not Empty, Create A Cube There
//                 Renderer::Primitive::Cube CubeCreateInfo;
//                 CubeCreateInfo.Position_ = vsg::vec3(X*VoxelSize + VoxelBB.bb_point1[0], Y*VoxelSize + VoxelBB.bb_point1[1], 0);//SliceNumber*VoxelSize + VoxelBB.bb_point1[2]);
//                 // CubeCreateInfo.Position_ /= 10.f;
//                 CubeCreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
//                 CubeCreateInfo.Scale_ = vsg::vec3(VoxelSize, VoxelSize, VoxelSize);
//                 // CubeCreateInfo.Scale_ /= 10.f;
//                 CubeCreateInfo.Shader_ = &BoxShader;

//                 _Renderer->AddBox(&CubeCreateInfo);
//                 // std::cout<<"Adding Cube for voxel "<< X <<"x, "<< Y<<"y, " << SliceNumber<<"z  at position (um) " << CubeCreateInfo.Position_<<std::endl;

//             }

//         }
//     }

//     // Now, Update and Unlock Scene When Done
//     _Renderer->UpdateScene();
//     _Renderer->UnlockScene();




//     // Check that the renderers dir exists
//     if (!std::filesystem::is_directory("Renders") || !std::filesystem::exists("Renders")) { // Check if Renders folder exists
//         std::filesystem::create_directory("Renders"); // create Renders folder
//     }



//     // Now we start a for loop which renders each image for each step within the slice
//     // This is based on the FOV of the camera, height of the camera, and size of the voxels
//     double FOV_deg = Params->MicroscopeFOV_deg;
//     int PixelsPerVoxel = Params->NumPixelsPerVoxel_px;

//     // Calculate the camera distance based on the given microscope params
//     // res_in_voxels = image_width_px / pixels_per_voxel
//     int ResolutionInVoxels = Params->ImageWidth_px / 16; 
//     // height_um = res_in_voxels * voxel_size_um / tan(FOV_deg / 2)
//     double CameraDistance = ResolutionInVoxels * Params->VoxelResolution_um / tan(FOV_deg * 0.5 * (M_PI / 180.));



//     // To do this, we need to identify the total number of x,y steps
//     double CameraFrustumHeight_um = CalculateFrustumHeight_um(CameraDistance, FOV_deg); 
//     double CameraFrustumWidth_um = CalculateFrustumWidth_um(CameraDistance, FOV_deg, (double)Params->ImageWidth_px / (double)Params->ImageHeight_px);
//     double CameraYStep_um = CalculateCameraMovementStep_um(CameraFrustumHeight_um, Params->ScanRegionOverlap_percent);
//     double CameraXStep_um = CalculateCameraMovementStep_um(CameraFrustumWidth_um, Params->ScanRegionOverlap_percent);

//     double TotalSliceWidth = abs((double)Array->GetBoundingBox().bb_point1[0] - (double)Array->GetBoundingBox().bb_point2[0]);
//     double TotalSliceHeight = abs((double)Array->GetBoundingBox().bb_point1[1] - (double)Array->GetBoundingBox().bb_point2[1]);
//     int TotalXSteps = ceil(TotalSliceWidth / CameraXStep_um);
//     int TotalYSteps = ceil(TotalSliceHeight / CameraYStep_um);

//     // Update Stats With Total Number Of Images
//     _VSDAData->TotalSliceImages_ = TotalXSteps * TotalYSteps;

//     // Now, we enumerate through all the steps needed, one at a time until we reach the end
//     for (int XStep = 0; XStep < TotalXSteps; XStep++) {
//         for (int YStep = 0; YStep < TotalYSteps; YStep++) {

//             // Then, calculate the camera's position at this step
//             // We do it here over doing it in the for loop (with a double) due to floating-point error accumulation
//             double CameraX = CameraXStep_um * XStep;
//             double CameraY = CameraYStep_um * YStep;

//             // Now, reposition the camera, and take the picture
//             vsg::dvec3 NewPosition(CameraX, CameraY, CameraDistance);
//             _Renderer->UpdateCameraPosition(NewPosition);
//             _Renderer->SetCameraFOV(FOV_deg);

//             // Finally, actually render the frame, and save it to disk
//             // _VSDAData->Images_.push_back(std::make_unique<BG::NES::Renderer::Image>());
//             // BG::NES::Renderer::Image* Image = _VSDAData->Images_[_VSDAData->Images_.size() - 1].get();

//             std::string FilePath = "Renders/" + _FilePrefix + "_Slice" + std::to_string(SliceNumber);
//             FilePath += "_X" + std::to_string(CameraX);
//             FilePath += "_Y" + std::to_string(CameraY);
//             FilePath += ".png";

//             // _FileNameArray->push_back(FilePath);
//             // Image->TargetFileName_ = FilePath;

//             // _Renderer->RenderImage(Image);

//             // _EncoderPool->QueueEncodeOperation(Image);

//             // Update The API Result Info With The Current Slice Number
//             _VSDAData->CurrentSliceImage_ = (XStep * TotalYSteps) + YStep + 1;

//         }
//     }




//     return true;
// }



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
