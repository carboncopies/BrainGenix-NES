#include <BG/Renderer/Interface.h>



/**
 * MEMORY LEAK NOTICE:
 * 
 * There are false positives in this code! 
 * It's not actually leaking memory, valgrind just repots memory leaks due to dl_open calls.
 * To *actually* check for memory leaks, please use a null icd driver (see this for more info):
 *     https://github.com/KhronosGroup/Vulkan-Loader/issues/256
 * 
 * THIS IS A KNOWN BUG IN VALGRIND!
*/


// TODO: Update Doxygen Docs!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


namespace BG {
namespace NES {
namespace Renderer {


Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

    // Instantiate Subclasses
    RendererManager_ = std::make_unique<Manager>(_Logger);
    GeometryBuilder_ = std::make_unique<GeometryBuilder>(Logger_);


}

Interface::~Interface() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down NES Rendering Subsystem", 3);


}


bool Interface::Initialize(int _NumArgs, char** _ArgValues) {
    assert(Logger_ != nullptr);

    if (!RendererManager_->Initialize(_NumArgs, _ArgValues)) {
        return false;
    }
    if (!RendererManager_->SetupScene()) {
        return false;
    }


    // hacky
    Shaders::Phong CubeShader;
    CubeShader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 0.0f);
    CubeShader.SpecularColor_ = vsg::vec4(0.f, 0.0f, 0.0f, 0.0f);
    CubeShader.Type_ = Shaders::SHADER_PHONG;
    Primitive::Cube CreateInfo;
    CreateInfo.Position_ = vsg::vec3(0.0f, -1.1f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    CreateInfo.Scale_    = vsg::vec3(4.1f, 4.1f, 4.1f);
    CreateInfo.Shader_ = &CubeShader;
    AddBox(&CreateInfo);



    // CreateInfo.Position_ = vsg::vec3(-1.1f, 0.0f, 0.0f);
    // CreateInfo.Rotation_ = vsg::vec3(45.0f, 0.0f, 0.0f);
    // CreateInfo.Scale_    = vsg::vec3(2.0f, 1.0f, 1.0f);
    // CubeShader.DiffuseColor_  = vsg::vec4(0.8f, 0.2f, 0.2f, 1.0f);
    // AddBox(&CreateInfo);

    // CreateInfo.Position_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    // CreateInfo.Rotation_ = vsg::vec3(0.0f, 45.0f, 0.0f);
    // CreateInfo.Scale_    = vsg::vec3(1.0f, 2.0f, 1.0f);
    // CubeShader.DiffuseColor_  = vsg::vec4(0.2f, 0.8f, 0.2f, 1.0f);
    // AddBox(&CreateInfo);

    // CreateInfo.Position_ = vsg::vec3(1.1f, 0.0f, 0.0f);
    // CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 45.0f);
    // CreateInfo.Scale_    = vsg::vec3(1.0f, 1.0f, 2.0f);
    // CubeShader.DiffuseColor_  = vsg::vec4(0.2f, 0.2f, 0.8f, 1.0f);
    // AddBox(&CreateInfo);


    // Shaders::Phong SphereShader;
    // SphereShader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    // SphereShader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
    // SphereShader.Type_ = Shaders::SHADER_PHONG;
    // Primitive::Sphere SphereCreateInfo;
    // SphereCreateInfo.Position_ = vsg::vec3(0.0f, 0.0f, 2.0f);
    // SphereCreateInfo.Radius_   = 1.0f;
    // SphereCreateInfo.Shader_ = &SphereShader;
    // AddSphere(&SphereCreateInfo);


    // Shaders::Phong CylinderShader;
    // CylinderShader.DiffuseColor_  = vsg::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    // CylinderShader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
    // CylinderShader.Type_ = Shaders::SHADER_PHONG;
    // Primitive::Cylinder CylinderCreateInfo;
    // CylinderCreateInfo.Position_ = vsg::vec3(0.0f, 0.0f, 4.0f);
    // CylinderCreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 2.0f);
    // CylinderCreateInfo.Radius_   = 1.0f;
    // CylinderCreateInfo.Height_   = 1.0f;
    // CylinderCreateInfo.Shader_ = &CylinderShader;
    // AddCylinder(&CylinderCreateInfo);



    // CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);

    // for (int i = -20; i < 20; i++) {
    //     for (int y = -20; y < 20; y++) {
    //         for (int z = -20; z < 20; z++) {
    //             CubeShader.DiffuseColor_  = vsg::vec4(0.05f*abs(i), 0.05f*abs(y), 0.05f*abs(z), 1.0f);
    //             CreateInfo.Position_ = vsg::vec3(float(i) + 0.4f*i, float(y) + 0.4f*y, float(z) + 0.4f*z);
    //             CreateInfo.Rotation_ = vsg::vec3(8.0f*i, 8.0f*y, 8.0f*z);
    //             AddBox(&CreateInfo);
    //         }
    //     }
    // }

    RendererManager_->SetupViewer();


    return RendererManager_->CompileScene();

}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}


bool Interface::AddBox(Primitive::Cube* _CreateInfo) {
    assert(_CreateInfo != nullptr);
    return GeometryBuilder_->CreateCube(RendererManager_->GetScene(), _CreateInfo);
}

bool Interface::AddSphere(Primitive::Sphere* _CreateInfo) {
    assert(_CreateInfo != nullptr);
    return GeometryBuilder_->CreateSphere(RendererManager_->GetScene(), _CreateInfo);
}

bool Interface::AddCylinder(Primitive::Cylinder* _CreateInfo) {
    assert(_CreateInfo != nullptr);
    return GeometryBuilder_->CreateCylinder(RendererManager_->GetScene(), _CreateInfo);
}

bool Interface::UpdateScene() {
    return RendererManager_->CompileScene();
}

void Interface::WaitUntilGPUDone() {
    RendererManager_->WaitUntilGPUDone();
    return;
}
void Interface::LockScene() {
    RendererManager_->LockScene();
}

void Interface::UnlockScene() {
    RendererManager_->UnlockScene();
}


void Interface::ResetViewer() {
    LockScene();
    WaitUntilGPUDone();
    RendererManager_->SetupViewer();
    RendererManager_->CompileScene();
    UnlockScene();
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG