#include <Main.h>


int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup Logging System
    BG::Common::Logger::LoggingSystem Logger(true); // NOTE THAT THIS IS TEMPORARY; IT NEEDS TO BE EVENTUALLY FED
    // CONFIGURATION INFO FROM THE CONFIG SUBSYS - CURRENTLY IT IS OPERATING IN UNIT-TEST MODE!!!!

    // Setup API Server
    BG::NES::API::Manager APIManager(&SystemConfiguration);

    // Setup Simulator
    BG::NES::Simulator::Manager SimulationManager(&SystemConfiguration, &APIManager);

    // Setup Renderer
    BG::NES::Renderer::Interface RenderingInterface(&Logger);
    if (!RenderingInterface.Initialize(NumArguments,  ArgumentValues)) { 
        Logger.Log("Error During Renderer Initialization, Aborting", 10);
        return -1;
    }


    BG::NES::Renderer::Shaders::Phong CubeShader;
    CubeShader.DiffuseColor_  = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    CubeShader.SpecularColor_ = vsg::vec4(1.f, 0.1f, 0.1f, 1.0f);
    CubeShader.Type_ = BG::NES::Renderer::Shaders::SHADER_PHONG;
    BG::NES::Renderer::Primitive::Cube CreateInfo;
    CreateInfo.Position_ = vsg::vec3(0.0f, -1.1f, 0.0f);
    CreateInfo.Rotation_ = vsg::vec3(0.0f, 0.0f, 0.0f);
    CreateInfo.Scale_    = vsg::vec3(1.0f, 1.0f, 1.0f);
    CreateInfo.Shader_ = &CubeShader;
    RenderingInterface.AddBox(&CreateInfo);
    RenderingInterface.UpdateScene();



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


    
    // block forever while servers are running
    // while (true) {}
    
    // draw a thousand frames
    for (unsigned int i = 0; i < 1000; i++) {
        RenderingInterface.DrawFrame();
    }

    return 0;


}


