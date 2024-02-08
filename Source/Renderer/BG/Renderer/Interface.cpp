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


// Hack to make windowed rendering work again lol
void Interface::WindowedThread() {

    while (!ThreadExit_) {
        RendererManager_->DrawFrame();

        // delay so the scene mutex is easier to get by another thread if needed
        std::this_thread::sleep_for(std::chrono::microseconds(750)); 
    }

}


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

    if (IsWindowed_) {
        ThreadExit_ = true;
        WindowedUpdaterThread_.join();
    }

}


bool Interface::Initialize(bool _Windowed) {
    assert(Logger_ != nullptr);

    IsWindowed_ = _Windowed;
    ThreadExit_ = !_Windowed;

    if (!RendererManager_->SetupScene()) {
        return false;
    }

    if (!RendererManager_->Initialize(_Windowed)) {
        return false;
    }

    if (IsWindowed_) {
        WindowedUpdaterThread_ = std::thread(&Interface::WindowedThread, this);
    }

    RendererManager_->SetupViewer();

    return RendererManager_->CompileScene();

}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}

bool Interface::RenderImage(Image* _Image) {
    return RendererManager_->RenderImage(_Image);
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

bool Interface::SetResolution(int _Width, int _Height) {
    return RendererManager_->SetResolution(_Width, _Height);
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

void Interface::ResetScene() {
    RendererManager_->ClearScene();

    // LockScene();
    // WaitUntilGPUDone();

    // hacky
    // Shaders::Phong CubeShader;
    // CubeShader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    // CubeShader.SpecularColor_ = vsg::vec4(0.f, 0.0f, 0.0f, 1.0f);
    // CubeShader.Type_ = Shaders::SHADER_PHONG;
    // Primitive::Cube CreateInfo;
    // CreateInfo.Position_ = vsg::vec3(0.0f, -1.1f, 0.0f);
    // CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    // CreateInfo.Scale_    = vsg::vec3(4.1f, 4.1f, 4.1f);
    // CreateInfo.Shader_ = &CubeShader;
    // AddBox(&CreateInfo);

    // UnlockScene();
}



bool Interface::UpdateCameraPosition(vsg::dvec3 _Position, vsg::dvec3 _LookAtPosition) {

    return RendererManager_->UpdateCameraPosition(_Position, _LookAtPosition);

}

bool Interface::SetCameraFOV(double _FOV) {

    return RendererManager_->SetCameraFOV(_FOV);

}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG