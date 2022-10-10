//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_DrawMeshNoTextures.h>


void ERS_FUNCTION_DrawMeshNoTextures(ERS_STRUCT_Mesh* Mesh, ERS_STRUCT_Shader* Shader) {

    Shader->SetMat4("model", Mesh->ModelMatrix);

    // unsigned int AmbientOcclusionHandle = 1;
    // unsigned int DiffuseHandle = 1;
    // unsigned int DisplacementHandle = 1;
    // unsigned int EmissiveHandle = 1;
    // unsigned int HeightHandle = 1;
    // unsigned int MetalnessHandle = 1;
    // unsigned int NormalsHandle = 1;
    // unsigned int ShininessHandle = 1;



    // // Reset All Textures To Defaults
    // unsigned int ShaderProgram = Shader->ShaderProgram;
    // unsigned int ResetTexID = OpenGLDefaults->DefaultTexture_;
    // ERS_FUNCTION_ResetMeshTexture("texture_ambient_occlusion1", 1, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_diffuse1", 2, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_displacement1", 3, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_emissive1", 4, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_height1", 5, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_metalness1", 6, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_normals1", 7, ShaderProgram, ResetTexID);
    // ERS_FUNCTION_ResetMeshTexture("texture_shininess1", 8, ShaderProgram, ResetTexID);



    // bool HasAmbientOcclusion = false;
    // bool HasDiffuse = false;
    // bool HasDisplacement = false;
    // bool HasEmissive = false;
    // bool HasHeight = false;
    // bool HasMetalness = false;
    // bool HasNormals = false;
    // bool HasShininess = false;


    // // Iterate Through Textures
    // for (unsigned int i = 0; i < Mesh->TextureIDs.size(); i++) {

    //     // Set To Proper Texture
    //     glActiveTexture(GL_TEXTURE0 + i);

    //     // Get Texture Number
    //     std::string Number;
    //     std::string Name = Mesh->TextureNames[i];
    //     int Type = 0;

    //     // Detect Type
    //     if(Name == "texture_ambient_occlusion") {
    //         Number = std::to_string(AmbientOcclusionHandle++);
    //         Type = 1;
    //         HasAmbientOcclusion = true;
    //     } else if(Name == "texture_diffuse") {
    //         Number = std::to_string(DiffuseHandle++);
    //         Type = 2;
    //         HasDiffuse = true;
    //     } else if(Name == "texture_displacement") {
    //         Number = std::to_string(DisplacementHandle++);
    //         Type = 3;
    //         HasDisplacement = true;
    //     } else if(Name == "texture_emissive") {
    //         Number = std::to_string(EmissiveHandle++);
    //         Type = 4;
    //         HasEmissive = true;
    //     } else if(Name == "texture_height") {
    //         Number = std::to_string(HeightHandle++);
    //         Type = 5;
    //         HasHeight = true;
    //     } else if(Name == "texture_metalness") {
    //         Number = std::to_string(MetalnessHandle++);
    //         Type = 6;
    //         HasMetalness = true;
    //     } else if(Name == "texture_normals") {
    //         Number = std::to_string(NormalsHandle++);
    //         Type = 7;
    //         HasNormals = true;
    //     } else if(Name == "texture_shininess") {
    //         Number = std::to_string(ShininessHandle++);
    //         Type = 8;
    //         HasShininess = true;
    //     }

    //     glUniform1i(glGetUniformLocation(Shader->ShaderProgram, (Name + Number).c_str()), Type);
        
    //     // Bind Texture
    //     glActiveTexture(GL_TEXTURE0 + Type);
    //     glBindTexture(GL_TEXTURE_2D, Mesh->TextureIDs[i]);

    // }

    // // Set Uniforms
    // Shader->SetBool("HasAmbientOcclusion", HasAmbientOcclusion);
    // Shader->SetBool("HasDiffuse", HasDiffuse);
    // Shader->SetBool("HasDisplacement", HasDisplacement);
    // Shader->SetBool("HasEmissive", HasEmissive);
    // Shader->SetBool("HasHeight", HasHeight);
    // Shader->SetBool("HasMetalness", HasMetalness);
    // Shader->SetBool("HasNormals", HasNormals);
    // Shader->SetBool("HasShininess", HasShininess);
    


    // Draw Mesh
    glBindVertexArray(Mesh->VAO);
    glDrawElements(GL_TRIANGLES, Mesh->NumberIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}