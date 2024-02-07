#include <VSDA/DebugHelpers/MeshBuilder.h>



namespace BG {
namespace NES {
namespace Simulator {

// bool BuildMeshFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation) {
//     assert(_Logger != nullptr && "You have passed a nullptr to the logger parameter, bad!");
//     assert(_Renderer != nullptr && "You have passed a nullptr to the renderer parameter, bad!");
//     assert(_Simulation != nullptr && "You have passed a nullptr to the simulation parameter, bad!");

//     _Logger->Log("Building 3D Mesh For Simulation " + _Simulation->Name, 5);
//     _Renderer->LockScene();
//     _Renderer->WaitUntilGPUDone();

//     // Enumerate Simulation Primitives
//     for (unsigned int i = 0; i < _Simulation->BSCompartments.size(); i++) {
//         int ShapeID = _Simulation->BSCompartments[i].ShapeID;

//         if (_Simulation->Collection.IsBox(ShapeID)) { // (std::holds_alternative<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID])) {
//             const Geometries::Box & Box = _Simulation->Collection.GetBox(ShapeID); // Geometries::Box Box = std::get<Geometries::Box>(_Simulation->Collection.Geometries[ShapeID]);
            
//             BG::NES::Renderer::Shaders::Phong Shader;
//             Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 0.1f);
//             Shader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
//             Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

//             BG::NES::Renderer::Primitive::Cube CreateInfo;
//             CreateInfo.Position_ = ConvertVector(Box.Center_um);
//             CreateInfo.Rotation_ = RadsToDeg(ConvertVector(Box.Rotations_rad));
//             CreateInfo.Scale_    = ConvertVector(Box.Dims_um);
//             CreateInfo.Shader_ = &Shader;

//             _Renderer->AddBox(&CreateInfo);

//         } else if (_Simulation->Collection.IsSphere(ShapeID)) { // (std::holds_alternative<Geometries::Sphere>(_Simulation->Collection.Geometries[ShapeID])) {
//             const Geometries::Sphere & Sphere = _Simulation->Collection.GetSphere(ShapeID); // Geometries::Sphere Sphere = std::get<Geometries::Sphere>(_Simulation->Collection.Geometries[ShapeID]);
            
//             BG::NES::Renderer::Shaders::Phong Shader;
//             Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 0.1f);
//             Shader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
//             Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

//             BG::NES::Renderer::Primitive::Sphere CreateInfo;
//             CreateInfo.Position_ = ConvertVector(Sphere.Center_um);
//             CreateInfo.Radius_   = Sphere.Radius_um;
//             CreateInfo.Shader_ = &Shader;

//             _Renderer->AddSphere(&CreateInfo); 
//         } else if (_Simulation->Collection.IsCylinder(ShapeID)) { // (std::holds_alternative<Geometries::Cylinder>(_Simulation->Collection.Geometries[ShapeID])) {
//             const Geometries::Cylinder & Cylinder = _Simulation->Collection.GetCylinder(ShapeID); // Geometries::Cylinder Cylinder = std::get<Geometries::Cylinder>(_Simulation->Collection.Geometries[ShapeID]);
            
//             BG::NES::Renderer::Shaders::Phong Shader;
//             Shader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 0.1f);
//             Shader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
//             Shader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;

//             BG::NES::Renderer::Primitive::Cylinder CreateInfo;
//             CreateInfo.Position_ = ConvertVector(Cylinder.Center_um);
//             _Logger->Log("Warning: Renderer does not yet support Variable Width Cylinders!", 8); // FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! FIXME! 
//             CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
//             CreateInfo.Height_ = 1.0f;
//             CreateInfo.Radius_   = Cylinder.End0Radius_um;
//             CreateInfo.Shader_ = &Shader;

//             _Renderer->AddCylinder(&CreateInfo); 
//         }

//     }


//     _Renderer->UpdateScene();
//     _Renderer->UnlockScene();


//     return true;

// }

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG