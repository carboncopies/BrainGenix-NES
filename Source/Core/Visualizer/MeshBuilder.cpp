#include <Visualizer/MeshBuilder.h>



namespace BG {
namespace NES {
namespace Simulator {

bool BuildMeshFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation) {
    assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
    assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
    assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

    _Logger->Log("Building 3D Mesh For Simulation " + _Simulation->Name, 5);
    _Renderer->LockScene();
    _Renderer->WaitUntilGPUDone();

    // Enumerate Simulation Primitives
    for (unsigned int i = 0; i < _Simulation->BSCompartments.size(); i++) {
        int ShapeID = _Simulation->BSCompartments[i].ShapeID;

        if (_Simulation->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID])) {
            const Geometries::Box & Box = _Simulation->Collection.GetBox(ShapeID); // Geometries::Box Box = std::get<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID]);
            
            BG::NES::Renderer::Shaders::Phong Shader;
            Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
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
            Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
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
            Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
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

    _Logger->Log("Done Building 3D Mesh For Simulation " + _Simulation->Name, 3);

    _Renderer->UpdateScene();
    _Renderer->UnlockScene();


    return true;

}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG