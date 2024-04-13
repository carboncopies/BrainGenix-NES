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

bool GeometryBuilder::ShaderHandler(Shaders::Shader* _Shader) {
    assert(_Shader != nullptr);

    // Setup Shader (maybe make this it's own function?)
    vsg::ref_ptr<vsg::Options> Options = vsg::Options::create();

    if (_Shader->Type_ == Shaders::SHADER_PHONG) {
        Builder_->shaderSet = vsg::createPhongShaderSet(Options);
        vsg::UniformBinding& MaterialBinding = Builder_->shaderSet->getUniformBinding("material");
        vsg::ref_ptr<vsg::PhongMaterialValue> Material = vsg::PhongMaterialValue::create();

        Shaders::Phong* ShaderInfo = (Shaders::Phong*)_Shader; // If this fails and explodes, someone set the Shader.Type_ enum wrong!

        Material->value().diffuse = ShaderInfo->DiffuseColor_;
        Material->value().specular = ShaderInfo->SpecularColor_;
        Material->value().emissive = ShaderInfo->EmissiveColor_;

        MaterialBinding.data = Material;
        return true;
    } else {
        // No shader was set, log error and return false
        Logger_->Log("Shader Was Not Specified. Please Specify A Shader Type Prior To Calling Create", 8);
        return false;
    }

}

bool GeometryBuilder::CreateCube(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Cube* _CubeCreateInfo) {
    assert(_CubeCreateInfo != nullptr && "_CubeCreateInfo is a nullptr");
    assert(_CubeCreateInfo->Shader_ != nullptr && "_CubeCreateInfo->Shader_ is null");


    Logger_->Log("Creating Cube Mesh", 0);


    // Setup Geom Info
    vsg::GeometryInfo Info;
    Info.transform = Math::BuildTransformMatrix(_CubeCreateInfo->Position_, _CubeCreateInfo->Rotation_, _CubeCreateInfo->Scale_);


    // Setup State Information
    vsg::StateInfo StateInfo;
    StateInfo.lighting = true;
    if (!ShaderHandler(_CubeCreateInfo->Shader_)) {
        return false;
    }
    
    // Add Primitive To Scene Provided
    _Scene->addChild(Builder_->createBox(Info, StateInfo));

    return true;

}


bool GeometryBuilder::CreateSphere(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Sphere* _SphereCreateInfo) {
    assert(_SphereCreateInfo != nullptr && "_SphereCreateInfo is a nullptr");
    assert(_SphereCreateInfo->Shader_ != nullptr && "_SphereCreateInfo->Shader_ is null");


    Logger_->Log("Creating Sphere Mesh", 0);


    // Setup Geom Info
    vsg::GeometryInfo Info;
    Info.transform = Math::BuildTransformMatrix(_SphereCreateInfo->Position_, vsg::vec3(0.f, 0.f, 0.f), vsg::vec3(_SphereCreateInfo->Radius_*2, _SphereCreateInfo->Radius_*2, _SphereCreateInfo->Radius_*2));


    // Setup State Information
    vsg::StateInfo StateInfo;
    StateInfo.lighting = true;
    if (!ShaderHandler(_SphereCreateInfo->Shader_)) {
        return false;
    }

    
    // Add Primitive To Scene Provided
    _Scene->addChild(Builder_->createSphere(Info, StateInfo));

    return true;
}


bool GeometryBuilder::CreateCylinder(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Cylinder* _CylinderCreateInfo) {
    assert(_CylinderCreateInfo != nullptr && "_CylinderCreateInfo is a nullptr");
    assert(_CylinderCreateInfo->Shader_ != nullptr && "_CylinderCreateInfo->Shader_ is null");


    Logger_->Log("Creating Cylinder Mesh", 0);


    // Setup Geom Info
    vsg::GeometryInfo Info;
    Info.transform = Math::BuildTransformMatrix(_CylinderCreateInfo->Position_, _CylinderCreateInfo->Rotation_, vsg::vec3(_CylinderCreateInfo->Radius_, _CylinderCreateInfo->Radius_, _CylinderCreateInfo->Height_));


    // Setup State Information
    vsg::StateInfo StateInfo;
    StateInfo.lighting = true;
    if (!ShaderHandler(_CylinderCreateInfo->Shader_)) {
        return false;
    }

    
    // Add Primitive To Scene Provided
    _Scene->addChild(Builder_->createCylinder(Info, StateInfo));

    return true;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG