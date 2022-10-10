//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_IconRenderer.h>



ERS_CLASS_IconRenderer::ERS_CLASS_IconRenderer(ERS_STRUCT_OpenGLDefaults* Defaults, ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Shader* IconRendererShader) {


    SystemUtils_ = SystemUtils;
    IconRendererShader_ = IconRendererShader;
    OpenGLDefaults_ = Defaults;


    SystemUtils_->Logger_->Log("Setting Up VisualRenderer IconRenderer System", 5);



    const float IconRendererVertices[] = {
        -1.0,  1.0, 0.0, 0.0, 1.0,  // Top Left
        -1.0, -1.0, 0.0, 1.0, 1.0,  // Bottom Left
        1.0, -1.0,  0.0, 1.0, 0.0,  // Bottom Right
        1.0,  1.0,  0.0, 0.0, 0.0   // Top Right
    };

    glGenBuffers(1, &IconRendererVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, IconRendererVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(IconRendererVertices), IconRendererVertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &IconRendererVAO_);
    glBindVertexArray(IconRendererVAO_);

    int PositionIndex = 0;
    glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(PositionIndex);

    int TexCoordsIndex = 2;
    glEnableVertexAttribArray(TexCoordsIndex);
    glVertexAttribPointer(TexCoordsIndex, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));


    // Initialize Model Array
    IconRendererModelArray_ = glm::mat4(1.0f);

}

ERS_CLASS_IconRenderer::~ERS_CLASS_IconRenderer() {

    SystemUtils_->Logger_->Log("IconRenderer Destructor Called", 6);

}

void ERS_CLASS_IconRenderer::Draw(ERS_STRUCT_Camera* Camera, ERS_CLASS_SceneManager* SceneManager) {

    glDisable(GL_DEPTH_TEST);
    IconRendererShader_->MakeActive();
    glm::mat4 Projection;
    glm::mat4 View;
    Camera->GetMatrices(Projection, View);

    glm::vec3 CameraPosition = Camera->GetPosition();

    ERS_STRUCT_Scene* Scene = SceneManager->Scenes_[SceneManager->ActiveScene_].get();

    // Draw All Point Lights
    for (int i = 0; (long)i < (long)Scene->PointLights.size(); i++) {

        glm::vec3 LightPosition = Scene->PointLights[i]->Pos;
        glm::mat4 NewModelMatrix = glm::translate(IconRendererModelArray_, LightPosition);


        // FIXME: Make Lights a "Billboard" So they Rotate Towards The Camera

        // glm::vec3 ModelRotation = glm::normalize(CameraPosition - LightPosition);
        // NewModelMatrix = glm::rotate(NewModelMatrix, ModelRotation.x, glm::vec3(1, 0, 0));
        // NewModelMatrix = glm::rotate(NewModelMatrix, ModelRotation.y, glm::vec3(0, 1, 0));
        // NewModelMatrix = glm::rotate(NewModelMatrix, ModelRotation.z, glm::vec3(0, 0, 1));
        NewModelMatrix = glm::scale(NewModelMatrix, glm::vec3(IconRendererScale_));

        IconRendererShader_->SetMat4("model", NewModelMatrix);
        IconRendererShader_->SetMat4("view", View);
        IconRendererShader_->SetMat4("projection", Projection);

        IconRendererShader_->SetVec3("CameraPosition", CameraPosition);

        IconRendererShader_->SetFloat("BillboardSize", IconRendererScale_);
        IconRendererShader_->SetVec3("BillboardPosition", LightPosition);
        
        //IconRendererShader_->SetVec3("BillboardRotation", Scene->PointLights[i]->Pos);
        
        glUniform1i(glGetUniformLocation(IconRendererShader_->ShaderProgram_, "IconTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLDefaults_->PointLightTexture_);

        glBindVertexArray(IconRendererVAO_);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }


    // Draw All Directional Lights
    for (int i = 0; (long)i < (long)Scene->DirectionalLights.size(); i++) {

        glm::vec3 LightPosition = Scene->DirectionalLights[i]->Pos;
        glm::mat4 NewModelMatrix = glm::translate(IconRendererModelArray_, LightPosition);


        // FIXME: Make Lights a "Billboard" So they Rotate Towards The Camera
        // glm::vec3 ModelRotation = glm::normalize(CameraPosition - LightPosition);

        glm::vec3 LampRotation = Scene->DirectionalLights[i]->Rot;
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.x, glm::vec3(1, 0, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.y, glm::vec3(0, 1, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.z, glm::vec3(0, 0, 1));
        NewModelMatrix = glm::scale(NewModelMatrix, glm::vec3(IconRendererScale_));

        IconRendererShader_->SetMat4("model", NewModelMatrix);
        IconRendererShader_->SetMat4("view", View);
        IconRendererShader_->SetMat4("projection", Projection);

        IconRendererShader_->SetVec3("CameraPosition", CameraPosition);

        IconRendererShader_->SetFloat("BillboardSize", IconRendererScale_);
        IconRendererShader_->SetVec3("BillboardPosition", LightPosition);
        IconRendererShader_->SetVec3("BillboardRotation", LampRotation);
        
        glUniform1i(glGetUniformLocation(IconRendererShader_->ShaderProgram_, "IconTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLDefaults_->DirectionalLightTexture_);

        glBindVertexArray(IconRendererVAO_);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }


    // Draw All Spot Lights
    for (int i = 0; (long)i < (long)Scene->SpotLights.size(); i++) {

        glm::vec3 LightPosition = Scene->SpotLights[i]->Pos;
        glm::mat4 NewModelMatrix = glm::translate(IconRendererModelArray_, LightPosition);


        // FIXME: Make Lights a "Billboard" So they Rotate Towards The Camera
        // glm::vec3 ModelRotation = glm::normalize(CameraPosition - LightPosition);

        glm::vec3 LampRotation = Scene->SpotLights[i]->Rot;
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.x, glm::vec3(1, 0, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.y, glm::vec3(0, 1, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.z, glm::vec3(0, 0, 1));
        NewModelMatrix = glm::scale(NewModelMatrix, glm::vec3(IconRendererScale_));

        IconRendererShader_->SetMat4("model", NewModelMatrix);
        IconRendererShader_->SetMat4("view", View);
        IconRendererShader_->SetMat4("projection", Projection);

        IconRendererShader_->SetVec3("CameraPosition", CameraPosition);

        IconRendererShader_->SetFloat("BillboardSize", IconRendererScale_);
        IconRendererShader_->SetVec3("BillboardPosition", LightPosition);
        IconRendererShader_->SetVec3("BillboardRotation", LampRotation);
        
        glUniform1i(glGetUniformLocation(IconRendererShader_->ShaderProgram_, "IconTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLDefaults_->SpotLightTexture_);

        glBindVertexArray(IconRendererVAO_);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }


    // Draw All Cameras
    for (int i = 0; (long)i < (long)Scene->SceneCameras.size(); i++) {

        glm::vec3 CameraPosition = Scene->SceneCameras[i]->Pos_;
        glm::mat4 NewModelMatrix = glm::translate(IconRendererModelArray_, CameraPosition);


        // FIXME: Make Lights a "Billboard" So they Rotate Towards The Camera
        // glm::vec3 ModelRotation = glm::normalize(CameraPosition - LightPosition);

        glm::vec3 LampRotation = Scene->SceneCameras[i]->Rot_;
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.x, glm::vec3(1, 0, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.y, glm::vec3(0, 1, 0));
        NewModelMatrix = glm::rotate(NewModelMatrix, LampRotation.z, glm::vec3(0, 0, 1));
        NewModelMatrix = glm::scale(NewModelMatrix, glm::vec3(IconRendererScale_));

        IconRendererShader_->SetMat4("model", NewModelMatrix);
        IconRendererShader_->SetMat4("view", View);
        IconRendererShader_->SetMat4("projection", Projection);

        IconRendererShader_->SetVec3("CameraPosition", CameraPosition);

        IconRendererShader_->SetFloat("BillboardSize", IconRendererScale_);
        IconRendererShader_->SetVec3("BillboardPosition", CameraPosition);
        IconRendererShader_->SetVec3("BillboardRotation", LampRotation);
        
        glUniform1i(glGetUniformLocation(IconRendererShader_->ShaderProgram_, "IconTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLDefaults_->SceneCameraTexture_);

        glBindVertexArray(IconRendererVAO_);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }


    glEnable(GL_DEPTH_TEST);


}

