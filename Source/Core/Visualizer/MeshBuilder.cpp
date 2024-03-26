#include <Visualizer/MeshBuilder.h>

#include <Simulator/Structs/RecordingElectrode.h>


#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {


float RandFloatBetween0And1() {
    return (float)(rand()) / (float)(RAND_MAX);
}


void GetRotationInfo(Geometries::Vec3D _Point1_um, Geometries::Vec3D _Point2_um, Geometries::Vec3D* _Rotation_rad, float* _Length_um) {

   // Calculate vector between Point1 and Point2
   Geometries::Vec3D delta = {_Point2_um.x - _Point1_um.x, _Point2_um.y - _Point1_um.y, _Point2_um.z - _Point1_um.z};

   // Normalize the delta vector to get unit direction vector
   float magnitude = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
   if (magnitude > 0.0f) {
       delta.x /= magnitude;
       delta.y /= magnitude;
       delta.z /= magnitude;
   } else {
       // Handle degenerate cases where the rods coincide
       // This could potentially involve setting up default values for rotation and length
   }

   // Compute cross products to find the three axes of the coordinate system defined by the rod
   Geometries::Vec3D axisX = {delta.z, -delta.y, delta.x};
   Geometries::Vec3D axisY = {-delta.z, delta.x, delta.y};
   Geometries::Vec3D axisZ = {delta.y, delta.z, -delta.x};

   // Calculate the angle between the rod and the X-axis
   float angleX = std::atan2(std::sqrt(pow(axisX.x, 2) + pow(axisX.y, 2)), abs(axisX.z));

   // Rotation around Y-axis
   float angleY = std::asin(-axisY.z);

   // Rotation around Z-axis
  float angleZ = std::atan2(std::sqrt(pow(axisZ.y, 2) + pow(axisZ.z, 2)), abs(axisZ.x));

  // Assign the calculated rotation angles to the output pointer
  _Rotation_rad->x = angleX;
  _Rotation_rad->y = angleY;
  _Rotation_rad->z = angleZ;

  // Calculate the length of the rod
  *_Length_um = sqrt((_Point2_um.x - _Point1_um.x)*(_Point2_um.x - _Point1_um.x) + (_Point2_um.y - _Point1_um.y)*(_Point2_um.y - _Point1_um.y) + (_Point2_um.z - _Point1_um.z)*(_Point2_um.z - _Point1_um.z));
}


bool BuildMeshFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation) {
    assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
    assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
    assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

    _Logger->Log("Building 3D Neural Mesh For Simulation " + _Simulation->Name, 5);
    _Renderer->LockScene();
    _Renderer->WaitUntilGPUDone();

    // Enumerate Simulation Primitives
    for (unsigned int i = 0; i < _Simulation->BSCompartments.size(); i++) {
        int ShapeID = _Simulation->BSCompartments[i].ShapeID;

        if (_Simulation->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID])) {
            const Geometries::Box & Box = _Simulation->Collection.GetBox(ShapeID); // Geometries::Box Box = std::get<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID]);
            
            BG::NES::Renderer::Shaders::Phong Shader;
            Shader.DiffuseColor_  = vsg::vec4(RandFloatBetween0And1(), RandFloatBetween0And1(), RandFloatBetween0And1(), 1.0f);
            Shader.SpecularColor_ = vsg::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

            BG::NES::Renderer::Primitive::Cube CreateInfo;
            CreateInfo.Position_ = ConvertVector(Box.Center_um);
            CreateInfo.Rotation_ = RadsToDeg(ConvertVector(Box.Rotations_rad));
            CreateInfo.Scale_    = ConvertVector(Box.Dims_um);
            CreateInfo.Shader_ = &Shader;

            _Renderer->AddBox(&CreateInfo);

        } else if (_Simulation->Collection.IsSphere(ShapeID)) { // (std::holds_alternative<Geometries::Sphere>(_Simulation->Collection.Geometries[ShapeID])) {
            const Geometries::Sphere & Sphere = _Simulation->Collection.GetSphere(ShapeID); // Geometries::Sphere Sphere = std::get<Geometries::Sphere>(_Simulation->Collection.Geometries[ShapeID]);

            BG::NES::Renderer::Shaders::Phong Shader;
            Shader.DiffuseColor_  = vsg::vec4(RandFloatBetween0And1(), RandFloatBetween0And1(), RandFloatBetween0And1(), 1.0f);
            Shader.SpecularColor_ = vsg::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

            BG::NES::Renderer::Primitive::Sphere CreateInfo;
            CreateInfo.Position_ = ConvertVector(Sphere.Center_um);
            CreateInfo.Radius_   = Sphere.Radius_um;
            CreateInfo.Shader_ = &Shader;

            _Renderer->AddSphere(&CreateInfo); 
        } else if (_Simulation->Collection.IsCylinder(ShapeID)) { // (std::holds_alternative<Geometries::Cylinder>(_Simulation->Collection.Geometries[ShapeID])) {
            const Geometries::Cylinder & Cylinder = _Simulation->Collection.GetCylinder(ShapeID); // Geometries::Cylinder Cylinder = std::get<Geometries::Cylinder>(_Simulation->Collection.Geometries[ShapeID]);
            
            BG::NES::Renderer::Shaders::Phong Shader;
            Shader.DiffuseColor_  = vsg::vec4(RandFloatBetween0And1(), RandFloatBetween0And1(), RandFloatBetween0And1(), 1.0f);
            Shader.SpecularColor_ = vsg::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

            BG::NES::Renderer::Primitive::Cylinder CreateInfo;
            Geometries::Vec3D CylinderCenter = (Cylinder.End0Pos_um + Cylinder.End1Pos_um) / 2.;
            CreateInfo.Position_ = ConvertVector(CylinderCenter);
            CreateInfo.Rotation_ = RadsToDeg(ConvertVector(Cylinder.GetRotation_rad()));
            

            Geometries::Vec3D P1 = Cylinder.End0Pos_um;
            Geometries::Vec3D P2 = Cylinder.End1Pos_um;
            double CylinderLength = pow((pow((P2.x - P1.x), 2) + pow((P2.y - P1.y), 2) + pow((P2.z - P1.z), 2)), 0.5);

            CreateInfo.Height_ = CylinderLength;
            CreateInfo.Radius_   = (Cylinder.End0Radius_um + Cylinder.End1Radius_um) / 2.;
            CreateInfo.Shader_ = &Shader;

            _Renderer->AddCylinder(&CreateInfo); 
        }

    }

    _Logger->Log("Done Building 3D Neural Mesh For Simulation " + _Simulation->Name, 3);

    _Renderer->UpdateScene();
    _Renderer->UnlockScene();


    return true;

}



bool BuildMeshFromElectrodes(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation) {
    assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
    assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
    assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

    _Logger->Log("Building 3D Electrode Mesh For Simulation " + _Simulation->Name, 5);
    _Renderer->LockScene();
    _Renderer->WaitUntilGPUDone();


    float MaxElectrodeLength_um = 100;


    // Enumerate Simulation Primitives
    for (unsigned int i = 0; i < _Simulation->RecordingElectrodes.size(); i++) {
        BG::NES::Simulator::Tools::RecordingElectrode* ThisElectrode = _Simulation->RecordingElectrodes[i].get();
        
        // Get Data from Vector
        Geometries::Vec3D End1Pos_um = ThisElectrode->TipPosition_um;
        Geometries::Vec3D End2Pos_um = ThisElectrode->EndPosition_um;

        // Ensure that it's not too long to render
        float Length = End1Pos_um.Distance(End2Pos_um);
        if (Length > MaxElectrodeLength_um) {
            Geometries::Vec3D DirectionVec = End2Pos_um - End1Pos_um;
            Geometries::Vec3D TruncatedVec = DirectionVec * (MaxElectrodeLength_um / Length);
            End2Pos_um = End1Pos_um + TruncatedVec;
        }

        // Setup and Get CenterPoint Info
        float Length_um = -1;
        Geometries::Vec3D Rotation_rad;
        Geometries::Vec3D CenterPoint_um;
        Geometries::Vec3D Dimensions_um;
        CenterPoint_um = (End1Pos_um + End2Pos_um) / 2.;
        GetRotationInfo(End1Pos_um, End2Pos_um, &Rotation_rad, &Length_um);
        //float Rotation_y = Rotation_rad.y;
        //Rotation_rad.y = Rotation_rad.x;
        //Rotation_rad.x = Rotation_y;


        // Now Setup The Dimensions So We Can See It
        Dimensions_um.z = 12;
        Dimensions_um.x = 4;
        Dimensions_um.y = Length_um;

        // Finally, add it to the scene
        BG::NES::Renderer::Shaders::Phong Shader;
        Shader.DiffuseColor_  = vsg::vec4(1., 0., 0., 1.0);
        Shader.SpecularColor_ = vsg::vec4(0.1f, 0.1f, 0.1f, 1.0);
        Shader.EmissiveColor_ = vsg::vec4(1., 0.0, 0.0, 1.0);
        Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

        BG::NES::Renderer::Primitive::Cube CreateInfo;
        CreateInfo.Position_ = ConvertVector(CenterPoint_um);
        CreateInfo.Rotation_ = RadsToDeg(ConvertVector(Rotation_rad));
        CreateInfo.Scale_    = ConvertVector(Dimensions_um);
        CreateInfo.Shader_ = &Shader;

        _Renderer->AddBox(&CreateInfo);


    }

    _Logger->Log("Done Building 3D Electrode Mesh For Simulation " + _Simulation->Name, 3);

    _Renderer->UpdateScene();
    _Renderer->UnlockScene();


    return true;

}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG