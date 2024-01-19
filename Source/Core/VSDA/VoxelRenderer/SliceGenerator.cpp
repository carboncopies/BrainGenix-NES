#include <vector>

#include <VSDA/VoxelRenderer/SliceGenerator.h>





namespace BG {
namespace NES {
namespace Simulator {






bool CreateVoxelArrayBorderFrame(VoxelArray* _Array) {

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
        size_t ShapeID = ThisCompartment->ShapeID;
        //std::variant<Geometries::Sphere, Geometries::Cylinder, Geometries::Box> ThisShape = _Sim->Collection.Geometries[ThisCompartment->ShapeID];

        if (_Sim->Collection.IsSphere(ShapeID)) { // (std::holds_alternative<Geometries::Sphere>(ThisShape)) {
            Geometries::Sphere & ThisSphere = _Sim->Collection.GetSphere(ShapeID); // std::get<Geometries::Sphere>(ThisShape);
            _Logger->Log("Adding Sphere To Voxel Array", 0);
            FillShape(_Array, &ThisSphere, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(ThisShape)) {
            Geometries::Box & ThisBox = _Sim->Collection.GetBox(ShapeID); // std::get<Geometries::Box>(ThisShape);
            _Logger->Log("Adding Box To Voxel Array", 0);
            FillBox(_Array, &ThisBox, _Params->VoxelResolution_um);
        }
        else if (_Sim->Collection.IsCylinder(ShapeID)) { // (std::holds_alternative<Geometries::Cylinder>(ThisShape)) {
            Geometries::Cylinder & ThisCylinder = _Sim->Collection.GetCylinder(ShapeID); // std::get<Geometries::Cylinder>(ThisShape);
            _Logger->Log("Adding Cylinder To Voxel Array", 0);
            FillCylinder(_Array, &ThisCylinder, _Params->VoxelResolution_um);
        }


    }

    CreateVoxelArrayBorderFrame(_Array);

    return true;

}

bool RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool) {
    assert(_VSDAData != nullptr);
    assert(_Logger != nullptr);


    // Get Params and Array From VSDAData
    MicroscopeParameters* Params = &_VSDAData->Params_;
    VoxelArray* Array = _VSDAData->Array_.get();


    _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);


    // Setup Voxel Info
    float VoxelSize = Array->GetResolution();
    BoundingBox VoxelBB = Array->GetBoundingBox();
    



    
    // TODO: Add proper total x steps, total y steps

    // Calculate Desired Image Size
    // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
    // then later on, we resample it to be the right size (for the target image)
    int VoxelsPerStepX = ceil(_VSDAData->Params_.ImageWidth_px / _VSDAData->Params_.NumPixelsPerVoxel_px);
    int VoxelsPerStepY = ceil(_VSDAData->Params_.ImageHeight_px / _VSDAData->Params_.NumPixelsPerVoxel_px);
    int NumChannels = 3;
    float CameraStepSizeX_um = VoxelsPerStepX * _VSDAData->Params_.VoxelResolution_um;
    float CameraStepSizeY_um = VoxelsPerStepY * _VSDAData->Params_.VoxelResolution_um;

    double TotalSliceWidth = abs((double)Array->GetBoundingBox().bb_point1[0] - (double)Array->GetBoundingBox().bb_point2[0]);
    double TotalSliceHeight = abs((double)Array->GetBoundingBox().bb_point1[1] - (double)Array->GetBoundingBox().bb_point2[1]);
    int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
    int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);


    // Update Stats With Total Number Of Images
    _VSDAData->TotalSliceImages_ = TotalXSteps * TotalYSteps;
    _Logger->Log("Image Stack Will Have " + std::to_string(TotalXSteps) + " X Steps, " + std::to_string(TotalYSteps) + " Y Steps", 4);


    // Now, we enumerate through all the steps needed, one at a time until we reach the end
    for (int XStep = 0; XStep < TotalXSteps; XStep++) {
        for (int YStep = 0; YStep < TotalYSteps; YStep++) {

            // Calculate the filename of the image to be generated
            std::string FilePath = "Renders/" + _FilePrefix + "_Slice" + std::to_string(SliceNumber);
            FilePath += "_X" + std::to_string(CameraStepSizeX_um * XStep) + "_Y" + std::to_string(CameraStepSizeY_um * YStep) + ".png";

            // Next, setup the image and get it ready to be drawn to
            _VSDAData->Images_.push_back(std::make_unique<BG::NES::Renderer::Image>(VoxelsPerStepX, VoxelsPerStepY, NumChannels));
            BG::NES::Renderer::Image* Image = _VSDAData->Images_[_VSDAData->Images_.size() - 1].get();
            Image->TargetHeight_px = _VSDAData->Params_.ImageWidth_px;
            Image->TargetWidth_px = _VSDAData->Params_.ImageHeight_px;
            Image->TargetFileName_ = FilePath;

            // Now enumerate the voxel array and populate the image with the desired pixels (for the subregion we're on)
            int XStartingPosition = VoxelsPerStepX * XStep;
            int YStartingPosition = VoxelsPerStepY * YStep;
            for (unsigned int XVoxelIndex = XStartingPosition; XVoxelIndex < XStartingPosition + VoxelsPerStepX; XVoxelIndex++) {
                for (unsigned int YVoxelIndex = YStartingPosition; YVoxelIndex < YStartingPosition + VoxelsPerStepY; YVoxelIndex++) {

                    // Get Voxel At Position
                    VoxelType ThisVoxel = Array->GetVoxel(XVoxelIndex, YVoxelIndex, SliceNumber);

                    // Now Set The Pixel
                    int ThisPixelX = XVoxelIndex - XStartingPosition;
                    int ThisPixelY = YVoxelIndex - YStartingPosition;
                    if (ThisVoxel == FILLED) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 220, 220, 220);
                    } else if (ThisVoxel == BORDER) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 255, 128, 50);
                    } else if (ThisVoxel == OUT_OF_RANGE) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 0, 0, 255);
                    } else {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 80, 80, 80);
                    }

                }
            }

            _EncoderPool->QueueEncodeOperation(Image);
            _FileNameArray->push_back(FilePath);

            // Update The API Result Info With The Current Slice Number
            _VSDAData->CurrentSliceImage_ = (XStep * TotalYSteps) + YStep + 1;

        }
    }



    return true;
}





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
