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

    // Extract Values From Struct
    glm::vec3 Position = glm::vec3(_CubeCreateInfo->Position_.x, _CubeCreateInfo->Position_.y, _CubeCreateInfo->Position_.z);
    glm::vec3 Rotation = glm::vec3(_CubeCreateInfo->Rotation_.x, _CubeCreateInfo->Rotation_.y, _CubeCreateInfo->Rotation_.z);
    glm::vec3 Scale    = glm::vec3(_CubeCreateInfo->Scale_.x, _CubeCreateInfo->Scale_.y, _CubeCreateInfo->Scale_.z);
    
    // Build Transformation Matrix
    glm::mat4 LRSMat = glm::translate(glm::mat4(1.0f), Position);
    LRSMat = glm::rotate(LRSMat, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
    LRSMat = glm::rotate(LRSMat, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
    LRSMat = glm::rotate(LRSMat, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
    LRSMat = glm::scale(LRSMat, Scale);

    // Convert GLM Mat4 Transformation Matrix To VSG Mat4
    Info.transform = Math::Mat4_GLMToVSG(LRSMat);


    std::string PositionString = std::to_string(Position.x) + "X " + std::to_string(Position.y) + "Y " + std::to_string(Position.z) + "Z";
    Logger_->Log(std::string("Creating Cube At ") + PositionString, 0);



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