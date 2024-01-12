#include <vector>

#include <VSDA/SliceGenerator/SliceGenerator.h>





namespace BG {
namespace NES {
namespace Simulator {


// FrustumHeight = 2. * Distance * math.tan(FOV * 0.5 * (math.pi/180))
// FrustumWidth = FrustumHeight * Aspect


double CalculateFrustumHeight_um(double _Distance, double _FOV) {
    return 2. * _Distance * tan(_FOV * 0.5 * (M_PI / 180.));
}

double CalculateFrustumWidth_um(double _Distance, double _FOV, double _AspectRatio) {
    return CalculateFrustumHeight_um(_Distance, _FOV) * _AspectRatio;
}

double CalculateCameraMovementStep_um(double _FrustumValue_um, double _Overlap_Percent) {
    return _FrustumValue_um * (1. - (_Overlap_Percent / 100.));
}



bool VSDA_EM_Initialize(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->VSDAData_.State_ != VSDA_NOT_INITIALIZED) { // Check if the VSDA has already been initialized, if so - we don't touch it.
        _Logger->Log("VSDA EM Initialize Error, Cannot Reinit A Region Which Has Already Been Initialized", 6);
        return false; 
    }

    // Initialize it
    _Sim->VSDAData_.State_ = VSDA_INIT_BEGIN;


    // That's all we have to do apparently...
    return true;

}

bool VSDA_EM_SetupMicroscope(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters _Params) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM SetupMicroscope Error, Cannot Setup Microscope On Syatem With Unknown State", 6);
        return false; 
    }

    // Check that Resolution is valid
    if (_Params.VoxelResolution_um == 0) {
        return false;
    }

    // Copy over the parameters
    _Sim->VSDAData_.Params_ = _Params;

    return true;

}

bool VSDA_EM_DefineScanRegion(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, ScanRegion _ScanRegion, int* _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM DefineScanRegion Error, Cannot Define Microscope Scan Region On Syatem With Unknown State", 6);
        return false; 
    }

    // Copy over the parameters, update region ID
    _Logger->Log(std::string("Adding Scan Region With Dimensions ") + _ScanRegion.Dimensions(), 2);
    _Sim->VSDAData_.Regions_.push_back(_ScanRegion);
    (*_RegionID) = _Sim->VSDAData_.Regions_.size()-1;

    // Add New Vector To Store The Rendered Image Paths As We Create Them Later On
    _Sim->VSDAData_.RenderedImagePaths_.push_back(std::vector<std::string>());

    return true;

}

bool VSDA_EM_QueueRenderOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, int _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM QueueRenderOperation Error, Cannot Queue Render Operation On Syatem With Unknown State", 6);
        return false; 
    }
    if (_RegionID < 0 || _RegionID >= _Sim->VSDAData_.Regions_.size()) { // the region ID is out of range
        _Logger->Log("VSDA EM QueueRenderOperation Error, Region ID Is Out Of Range!", 6);
        return false;    
    }

    if (_Sim->WorkRequested || _Sim->CurrentTask != SIMULATION_NONE) { // Cannot start an operation, simulation is busy
        _Logger->Log("VSDA EM QueueRenderOperation Error, Simulation Is Currently Running!", 6);
        return false;    
    }

    // Setup Enums, Indicate that work is requested
    _Sim->VSDAData_.ActiveRegionID_ = _RegionID;
    _Sim->VSDAData_.State_ = VSDA_RENDER_REQUESTED;
    _Sim->CurrentTask = SIMULATION_VSDA;
    _Sim->WorkRequested = true;

    return true;

}



bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

    // // X Alligned Border
    // for (int X = 0; X < _Array->GetX(); X++) {
    //     _Array->SetVoxel(X, 0, 0, BORDER);
    //     _Array->SetVoxel(X, _Array->GetY()-1, 0, BORDER);
    //     _Array->SetVoxel(X, 0, _Array->GetZ()-1, BORDER);
    //     _Array->SetVoxel(X, _Array->GetY()-1, _Array->GetZ()-1, BORDER);
    // }

    // // Y Alligned Border
    // for (int Y = 0; Y < _Array->GetY(); Y++) {
    //     _Array->SetVoxel(0, Y, 0, BORDER);
    //     _Array->SetVoxel(_Array->GetX()-1, Y, 0, BORDER);
    //     _Array->SetVoxel(0, Y, _Array->GetZ()-1, BORDER);
    //     _Array->SetVoxel(_Array->GetX()-1, Y, _Array->GetZ()-1, BORDER);
    // }

    // Z Alligned Border
    for (int Z = 0; Z < _Array->GetZ(); Z++) {
        _Array->SetVoxel(0, 0, Z, BORDER);
        _Array->SetVoxel(_Array->GetX()-1, 0, Z, BORDER);
        _Array->SetVoxel(0, _Array->GetY()-1, Z, BORDER);
        _Array->SetVoxel(_Array->GetX()-1, _Array->GetY()-1, Z, BORDER);
    }

    return true;

}

bool FillBoundingBox(VoxelArray* _Array, BoundingBox* _BB, float _VoxelScale) {

    for (float X = _BB->bb_point1[0]; X < _BB->bb_point2[0]; X+= _VoxelScale) {
        for (float Y = _BB->bb_point1[1]; Y < _BB->bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = _BB->bb_point1[2]; Z < _BB->bb_point2[2]; Z+= _VoxelScale) {
                _Array->SetVoxelAtPosition(X, Y, Z, FILLED);
            }
        }
    }

    return true;

}

bool FillShape(VoxelArray* _Array, Geometries::Geometry* _Shape, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop
    BoundingBox BB = _Shape->GetBoundingBox();

    for (float X = BB.bb_point1[0]; X < BB.bb_point2[0]; X+= _VoxelScale) {
        for (float Y = BB.bb_point1[1]; Y < BB.bb_point2[1]; Y+= _VoxelScale) {
            for (float Z = BB.bb_point1[2]; Z < BB.bb_point2[2]; Z+= _VoxelScale) {
                if (_Shape->IsPointInShape(Geometries::Vec3D(X, Y, Z))) {
                    _Array->SetVoxelAtPosition(X, Y, Z, FILLED);
                }
            }
        }
    }

    return true;

}

// NOTE: This needs a different FillShape function, because we will not be going through the bounding box and testing if in shape.
//       Instead, we will be placing rotated points that are in the shape.
bool FillCylinder(VoxelArray* _Array, Geometries::Cylinder* _Cylinder, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // 1. Get rotated point cloud.
    std::vector<Geometries::Vec3D> point_cloud = _Cylinder->GetPointCloud(_VoxelScale);

    // 2. Set corresponding voxels.
    for (const Geometries::Vec3D & p : point_cloud) {
        _Array->SetVoxelAtPosition(p.x, p.y, p.z, FILLED);
    }

    return true;
}

bool FillBox(VoxelArray* _Array, Geometries::Box* _Box, float _VoxelScale) {

    assert(_VoxelScale != 0); // Will get stuck in infinite loop

    // 1. Get rotated point cloud.
    std::vector<Geometries::Vec3D> point_cloud = _Box->GetPointCloud(_VoxelScale);

    // 2. Set corresponding voxels.
    for (const Geometries::Vec3D & p : point_cloud) {
        //std::cout << "Point: " << p.str() << '\n';
        _Array->SetVoxelAtPosition(p.x, p.y, p.z, FILLED);
    }

    return true;
}

bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array) {
    assert(_Array != nullptr);
    assert(_Params != nullptr);
    assert(_Sim != nullptr);
    assert(_Logger != nullptr);

    _Logger->Log(std::string("Building Voxel Array For Simulation '") + _Sim->Name + "'", 2);

    // Build Bounding Boxes For All Compartments
    for (unsigned int i = 0; i < _Sim->BSCompartments.size(); i++) {

        Compartments::BS* ThisCompartment = &_Sim->BSCompartments[i];
        std::variant<Geometries::Sphere, Geometries::Cylinder, Geometries::Box> ThisShape = _Sim->Collection.Geometries[ThisCompartment->ShapeID];

        if (std::holds_alternative<Geometries::Sphere>(ThisShape)) {
            Geometries::Sphere ThisSphere = std::get<Geometries::Sphere>(ThisShape);
            _Logger->Log("Adding Sphere To Voxel Array", 0);
            FillShape(_Array, &ThisSphere, _Params->VoxelResolution_um);
        }
        else if (std::holds_alternative<Geometries::Box>(ThisShape)) {
            Geometries::Box ThisBox = std::get<Geometries::Box>(ThisShape);
            _Logger->Log("Adding Box To Voxel Array", 0);
            FillBox(_Array, &ThisBox, _Params->VoxelResolution_um);
        }
        else if (std::holds_alternative<Geometries::Cylinder>(ThisShape)) {
            Geometries::Cylinder ThisCylinder = std::get<Geometries::Cylinder>(ThisShape);
            _Logger->Log("Adding Cylinder To Voxel Array", 0);
            FillCylinder(_Array, &ThisCylinder, _Params->VoxelResolution_um);
        }


    }

    CreateVoxelArrayBorderFrame(_Array);

    return true;

}

bool RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber) {
    assert(_VSDAData != nullptr);
    assert(_Logger != nullptr);


    // Get Params and Array From VSDAData
    MicroscopeParameters* Params = &_VSDAData->Params_;
    VoxelArray* Array = _VSDAData->Array_.get();


    _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);

    _Renderer->ResetScene();

    // Ensure Scene Is Now Threadsafe
    _Renderer->LockScene();
    _Renderer->WaitUntilGPUDone();

    // Setup Voxel Info
    float VoxelSize = Array->GetResolution();
    BoundingBox VoxelBB = Array->GetBoundingBox();
    
    _Renderer->UpdateCameraPosition(vsg::dvec3(4., 4., 5));


    // Enumerate Slice, Build Cubes Where Needed
    for (unsigned int X = 0; X < Array->GetX(); X++) {
        for (unsigned int Y = 0; Y < Array->GetY(); Y++) {

            // Get Voxel At The Specified Position
            VoxelType ThisVoxel = Array->GetVoxel(X, Y, SliceNumber);
            if (ThisVoxel != EMPTY) {


                // Build Shader with different color so we can tell each voxel apart
                Renderer::Shaders::Phong BoxShader;

                if (ThisVoxel == FILLED) {
                    BoxShader.DiffuseColor_  = vsg::vec4(0.8f, 0.8f, 0.8f, 1.0f);//vsg::vec4(X/(float)Array->GetX(), Y/(float)Array->GetY(), 1.0, 1.0f);
                    BoxShader.SpecularColor_ = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                } else if (ThisVoxel == BORDER) {
                    BoxShader.DiffuseColor_  = vsg::vec4(1.0, 0.5, 0.3, 1.0f);
                    BoxShader.SpecularColor_ = vsg::vec4(0.f, 0.f, 0.f, 0.0f);
                }
                BoxShader.Type_ = Renderer::Shaders::SHADER_PHONG;


                // If It's Not Empty, Create A Cube There
                Renderer::Primitive::Cube CubeCreateInfo;
                CubeCreateInfo.Position_ = vsg::vec3(X*VoxelSize + VoxelBB.bb_point1[0], Y*VoxelSize + VoxelBB.bb_point1[1], 0);//SliceNumber*VoxelSize + VoxelBB.bb_point1[2]);
                // CubeCreateInfo.Position_ /= 10.f;
                CubeCreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
                CubeCreateInfo.Scale_ = vsg::vec3(VoxelSize / 1.5, VoxelSize / 1.5, VoxelSize / 1.5);
                // CubeCreateInfo.Scale_ /= 10.f;
                CubeCreateInfo.Shader_ = &BoxShader;

                _Renderer->AddBox(&CubeCreateInfo);
                // std::cout<<"Adding Cube for voxel "<< X <<"x, "<< Y<<"y, " << SliceNumber<<"z  at position (um) " << CubeCreateInfo.Position_<<std::endl;

            }

        }
    }

    // Now, Update and Unlock Scene When Done
    _Renderer->UpdateScene();
    _Renderer->UnlockScene();




    // Check that the renderers dir exists
    if (!std::filesystem::is_directory("Renders") || !std::filesystem::exists("Renders")) { // Check if Renders folder exists
        std::filesystem::create_directory("Renders"); // create Renders folder
    }



    // Now we start a for loop which renders each image for each step within the slice
    // This is based on the FOV of the camera, height of the camera, and size of the voxels
    double FOV_deg = Params->MicroscopeFOV_deg;
    int PixelsPerVoxel = Params->NumPixelsPerVoxel_px;

    // Calculate the camera distance based on the given microscope params
    // res_in_voxels = image_width_px / pixels_per_voxel
    int ResolutionInVoxels = Params->ImageWidth_px / 16; 
    // height_um = res_in_voxels * voxel_size_um / tan(FOV_deg / 2)
    double CameraDistance = ResolutionInVoxels * Params->VoxelResolution_um / tan(FOV_deg * 0.5 * (M_PI / 180.));



    // To do this, we need to identify the total number of x,y steps
    double CameraFrustumHeight_um = CalculateFrustumHeight_um(CameraDistance, FOV_deg); 
    double CameraFrustumWidth_um = CalculateFrustumWidth_um(CameraDistance, FOV_deg, (double)Params->ImageWidth_px / (double)Params->ImageHeight_px);
    double CameraYStep_um = CalculateCameraMovementStep_um(CameraFrustumHeight_um, Params->ScanRegionOverlap_percent);
    double CameraXStep_um = CalculateCameraMovementStep_um(CameraFrustumWidth_um, Params->ScanRegionOverlap_percent);

    double TotalSliceWidth = abs((double)Array->GetBoundingBox().bb_point1[0] - (double)Array->GetBoundingBox().bb_point2[0]);
    double TotalSliceHeight = abs((double)Array->GetBoundingBox().bb_point1[1] - (double)Array->GetBoundingBox().bb_point2[1]);
    int TotalXSteps = ceil(TotalSliceWidth / CameraXStep_um);
    int TotalYSteps = ceil(TotalSliceHeight / CameraYStep_um);

    // Update Stats With Total Number Of Images
    _VSDAData->TotalSliceImages_ = TotalXSteps * TotalYSteps;

    // Now, we enumerate through all the steps needed, one at a time until we reach the end
    for (int XStep = 0; XStep < TotalXSteps; XStep++) {
        for (int YStep = 0; YStep < TotalYSteps; YStep++) {

            // Then, calculate the camera's position at this step
            // We do it here over doing it in the for loop (with a double) due to floating-point error accumulation
            double CameraX = CameraXStep_um * XStep;
            double CameraY = CameraYStep_um * YStep;

            // Now, reposition the camera, and take the picture
            vsg::dvec3 NewPosition(CameraX, CameraY, CameraDistance);
            _Renderer->UpdateCameraPosition(NewPosition);
            _Renderer->SetCameraFOV(FOV_deg);

            // Finally, actually render the frame, and save it to disk
            std::string FilePath = "Renders/" + _FilePrefix + "_Slice" + std::to_string(SliceNumber);
            FilePath += "_X" + std::to_string(CameraX);
            FilePath += "_Y" + std::to_string(CameraY);
            FilePath += ".bmp";
            _Renderer->DrawFrame(FilePath);
            _FileNameArray->push_back(FilePath);

            // Update The API Status Info With The Current Slice Number
            _VSDAData->CurrentSliceImage_ = (XStep * TotalYSteps) + YStep + 1;

        }
    }




    return true;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
