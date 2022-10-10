//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_SetShaderUniformData.h>


void ERS_FUNCTION_SetShaderUniformData(ERS_STRUCT_Shader* Shader, ERS_STRUCT_ShaderUniformData Data) {

    // Set Metadata Params
    Shader->SetVec3("CameraPosition",                            Data.CameraPosition_);
    Shader->SetVec2("ViewportRes",                               Data.ViewportRes_);
    Shader->SetFloat("Time",                                     Data.Time_);
    Shader->SetFloat("FrameTime",                                Data.FrameTime_);
    Shader->SetFloat("ShininessOffset",                          Data.ShininessOffset_);
    Shader->SetInt("FrameNumber",                                Data.FrameNumber_);


    // Set Camera Data
    Shader->SetMat4  ("projection",                              Data.Projection_);
    Shader->SetMat4  ("view",                                    Data.View_);
    Shader->SetFloat ("Exposure_",                               Data.Exposure_);
    Shader->SetFloat ("Gamma_",                                  Data.Gamma_);
    Shader->SetBool  ("GammaCorrectionEnabled_",                 Data.GammaCorrectionEnabled_);
    Shader->SetBool  ("HDREnabled_",                             Data.HDREnabled_);


    // Set Shadow Info
    Shader->SetInt("ShadowFilterType_",                          Data.ShadowFilterType_);
    Shader->SetInt("ShadowFilterKernelSize_",                    Data.ShadowFilterKernelSize_);

    // Bind To Shadow Maps
    glUniform1i(glGetUniformLocation(Shader->ShaderProgram_, "DepthMapArray"), 8);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D_ARRAY, Data.DepthMapArray_);

    glUniform1i(glGetUniformLocation(Shader->ShaderProgram_, "DepthCubemapArray"), 9);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Data.DepthCubemapArray_);




    // Directional Lights
    Shader->SetInt("NumberDirectionalLights", Data.NumberDirectionalLights_);
    for (int i = 0; i < Data.NumberDirectionalLights_; i++) {
    
        std::string UniformName = std::string("DirectionalLights[") + std::to_string(i) + std::string("]");

        Shader->SetMat4  ((UniformName + std::string(".LightSpaceMatrix")).c_str(),   Data.DirectionalLights_[i].LightSpaceMatrix_);
        Shader->SetVec3  ((UniformName + std::string(".Direction")).c_str(),          Data.DirectionalLights_[i].Direction_);
        Shader->SetVec3  ((UniformName + std::string(".Color")).c_str(),              Data.DirectionalLights_[i].Color_);
        Shader->SetFloat ((UniformName + std::string(".Intensity")).c_str(),          Data.DirectionalLights_[i].Intensity_);
        Shader->SetFloat ((UniformName + std::string(".MaxDistance")).c_str(),        Data.DirectionalLights_[i].MaxDistance_);
        Shader->SetInt   ((UniformName + std::string(".DepthMapIndex")).c_str(),      Data.DirectionalLights_[i].DepthMapIndex_);
        Shader->SetBool  ((UniformName + std::string(".CastsShadows")).c_str(),       Data.DirectionalLights_[i].CastsShadows_);
    
    }

    // Point Lights
    Shader->SetInt("NumberPointLights", Data.NumberPointLights_);
    for (int i = 0; i < Data.NumberPointLights_; i++) {
    
        std::string UniformName = std::string("PointLights[") + std::to_string(i) + std::string("]");

        Shader->SetVec3  ((UniformName + std::string(".Position")).c_str(),           Data.PointLights_[i].Position_);
        Shader->SetVec3  ((UniformName + std::string(".Color")).c_str(),              Data.PointLights_[i].Color_);
        Shader->SetFloat ((UniformName + std::string(".MaxDistance")).c_str(),        Data.PointLights_[i].MaxDistance_);
        Shader->SetFloat ((UniformName + std::string(".Intensity")).c_str(),          Data.PointLights_[i].Intensity_);
        Shader->SetInt   ((UniformName + std::string(".DepthCubemapIndex")).c_str(),  Data.PointLights_[i].DepthCubemapIndex_);
        Shader->SetBool  ((UniformName + std::string(".CastsShadows")).c_str(),       Data.PointLights_[i].CastsShadows_);

    }


    // Spot Lights
    Shader->SetInt("NumberSpotLights", Data.NumberSpotLights_);
    for (int i = 0; i < Data.NumberSpotLights_; i++) {
    
        std::string UniformName = std::string("SpotLights[") + std::to_string(i) + std::string("]");

        Shader->SetMat4  ((UniformName + std::string(".LightSpaceMatrix")).c_str(),   Data.SpotLights_[i].LightSpaceMatrix_);
        Shader->SetVec3  ((UniformName + std::string(".Position")).c_str(),           Data.SpotLights_[i].Position_);
        Shader->SetVec3  ((UniformName + std::string(".Direction")).c_str(),          Data.SpotLights_[i].Direction_);
        Shader->SetVec3  ((UniformName + std::string(".Color")).c_str(),              Data.SpotLights_[i].Color_);
        Shader->SetFloat ((UniformName + std::string(".Intensity")).c_str(),          Data.SpotLights_[i].Intensity_);
        Shader->SetFloat ((UniformName + std::string(".CutOff")).c_str(),             Data.SpotLights_[i].CutOff_);
        Shader->SetFloat ((UniformName + std::string(".RollOff")).c_str(),            Data.SpotLights_[i].RollOff_);
        Shader->SetFloat ((UniformName + std::string(".MaxDistance")).c_str(),        Data.SpotLights_[i].MaxDistance_);
        Shader->SetInt   ((UniformName + std::string(".DepthMapIndex")).c_str(),      Data.SpotLights_[i].DepthMapIndex_);
        Shader->SetBool  ((UniformName + std::string(".CastsShadows")).c_str(),       Data.SpotLights_[i].CastsShadows_);
    }
}