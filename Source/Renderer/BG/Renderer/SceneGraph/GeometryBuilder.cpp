#include <BG/Renderer/SceneGraph/GeometryBuilder.h>



namespace BG {
namespace NES {
namespace Renderer {



GeometryBuilder::GeometryBuilder(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

    // Create Builder Instance
    Logger_->Log("Creating VSG Geometry Builder Instance", 1);
    Builder_ = vsg::Builder::create();
}

GeometryBuilder::~GeometryBuilder() {
    assert(Logger_ != nullptr);
}



bool GeometryBuilder::CreateCube(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Cube* _CubeCreateInfo) {
    assert(_CubeCreateInfo != nullptr && "_CubeCreateInfo is a nullptr");
    assert(_CubeCreateInfo->Shader_ != nullptr && "_CubeCreateInfo->Shader_ is null");


    // Setup Geom Info
    vsg::GeometryInfo Info;
    Info.dx.set(1.0f, 0.0f, 0.0f); // maybe this
    Info.dy.set(0.0f, 1.0f, 0.0f); // does something
    Info.dz.set(0.0f, 0.0f, 1.0f); // to rotation?
    Info.position = _CubeCreateInfo->Position_;
    std::string PositionString = std::to_string(Info.position.x) + "X " + std::to_string(Info.position.y) + "Y " + std::to_string(Info.position.y) + "Z";
    Logger_->Log(std::string("Creating Cube At ") + PositionString, 0);



    // TODO: Handle rotation as well as scale
    Logger_->Log("TODO: Implement Rotation/Scale for CubeCreate", 8);



    // Setup State Information
    vsg::StateInfo StateInfo;
    StateInfo.lighting = true;


    // Setup Shader (maybe make this it's own function?)
    vsg::ref_ptr<vsg::Options> Options = vsg::Options::create();

    if (_CubeCreateInfo->Shader_->Type_ == Shaders::SHADER_PHONG) {
        Builder_->shaderSet = vsg::createPhongShaderSet(Options);
        vsg::UniformBinding& MaterialBinding = Builder_->shaderSet->getUniformBinding("material");
        vsg::ref_ptr<vsg::PhongMaterialValue> Material = vsg::PhongMaterialValue::create();

        Shaders::Phong* ShaderInfo = (Shaders::Phong*)_CubeCreateInfo->Shader_; // If this fails and explodes, someone set the Shader.Type_ enum wrong!

        Material->value().diffuse = ShaderInfo->DiffuseColor_;
        Material->value().specular = ShaderInfo->SpecularColor_;

        MaterialBinding.data = Material;

    } else {
        // No shader was set, log error and return false
        Logger_->Log("Shader Was Not Specified. Please Specify A Shader Type Prior To Calling Create", 8);
        return false;
    }

    
    // Add Primitive To Scene Provided
    _Scene->addChild(Builder_->createBox(Info, StateInfo));

    return true;

}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG