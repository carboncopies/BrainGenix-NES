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


    Shaders::Phong CubeShader;
    CubeShader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    CubeShader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
    CubeShader.Type_ = Shaders::SHADER_PHONG;
    Primitive::Cube CreateInfo;
    CreateInfo.Position_ = vsg::vec3(0.0f, -1.1f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    CreateInfo.Scale_    = vsg::vec3(1.0f, 1.0f, 1.0f);
    CreateInfo.Shader_ = &CubeShader;
    AddBox(&CreateInfo);



    CreateInfo.Position_ = vsg::vec3(-1.1f, 0.0f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(45.0f, 0.0f, 0.0f);
    CubeShader.DiffuseColor_  = vsg::vec4(0.8f, 0.2f, 0.2f, 1.0f);
    AddBox(&CreateInfo);

    CreateInfo.Position_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(0.0f, 45.0f, 0.0f);
    CubeShader.DiffuseColor_  = vsg::vec4(0.2f, 0.8f, 0.2f, 1.0f);
    AddBox(&CreateInfo);

    CreateInfo.Position_ = vsg::vec3(1.1f, 0.0f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 45.0f);
    CubeShader.DiffuseColor_  = vsg::vec4(0.2f, 0.2f, 0.8f, 1.0f);
    AddBox(&CreateInfo);





    for (int i = -10; i < 10; i++) {
        for (int y = -10; y < 10; y++) {
            CreateInfo.Position_ = vsg::vec3((float)i + 0.1f*i, float(y) + 0.1f*y, 0.0f);
            CreateInfo.Rotation_ = vsg::vec3(10.0f*i, 10.0f*y, -1.1f);
            AddBox(&CreateInfo);
        }
    }

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



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG