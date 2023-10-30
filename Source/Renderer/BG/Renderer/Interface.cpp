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

    // Instantiate Subclass RendererManager
    RendererManager_ = std::make_unique<Manager>(_Logger);


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
    AddBox();
    return RendererManager_->SetupViewer();
}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}


bool Interface::AddBox() {

    // Get Scene From RendererMangaer
    vsg::ref_ptr<vsg::Group> Scene = RendererManager_->GetScene();


    // Setup Builder
    vsg::ref_ptr<vsg::Builder> Builder = vsg::Builder::create();


    // Setup Geom Info
    vsg::GeometryInfo Info;
    Info.dx.set(1.0f, 0.0f, 0.0f);
    Info.dy.set(0.0f, 1.0f, 0.0f);
    Info.dz.set(0.0f, 0.0f, 1.0f);
    Info.position = vsg::vec3(0.0f, 0.0f, 0.0f);

    // Setup State Information
    vsg::StateInfo StateInfo;
    StateInfo.lighting = true;


    // Setup Shader
    vsg::ref_ptr<vsg::Options> Options = vsg::Options::create();
    Builder->shaderSet = vsg::createPhongShaderSet(Options);

    vsg::UniformBinding& MaterialBinding = Builder->shaderSet->getUniformBinding("material");

    vsg::ref_ptr<vsg::PhongMaterialValue> Material = vsg::PhongMaterialValue::create();
    Material->value().specular = vsg::vec4(0.9f, 0.3f, 0.3f, 1.0f);
    Material->value().diffuse = vsg::vec4(0.3f, 0.8f, 0.3f, 1.0f);
    MaterialBinding.data = Material;


    Scene->addChild(Builder->createBox(Info, StateInfo));


    return true;

}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG