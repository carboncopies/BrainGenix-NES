//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_MeshRenderer.h>



ERS_CLASS_MeshRenderer::ERS_CLASS_MeshRenderer(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing MeshRenderer Class Instance", 5);

}

ERS_CLASS_MeshRenderer::~ERS_CLASS_MeshRenderer() {

    SystemUtils_->Logger_->Log("MeshRenderer Destructor Called", 6);

}


void ERS_CLASS_MeshRenderer::RenderScene(ERS_STRUCT_Scene* Scene, ERS_STRUCT_OpenGLDefaults* OpenGLDefaults, std::vector<ERS_STRUCT_Shader*> Shaders, int DefaultShaderIndex, ERS_STRUCT_ShaderUniformData ShaderUniformInfo) {

    ERS_FUNCTION_UpdateMeshTransparency(Scene);

    std::vector<ERS_STRUCT_Mesh*> OpaqueMeshes;
    std::vector<ERS_STRUCT_Mesh*> TransparentMeshes;
    ERS_FUNCTION_MeshTransparencySort(&OpaqueMeshes, &TransparentMeshes, Scene);

    // Setup Variables To Handle Shader Switching
    int CurrentShaderIndex = DefaultShaderIndex;
    ERS_STRUCT_Shader* Shader = Shaders[CurrentShaderIndex];
    Shader->MakeActive();
    ERS_FUNCTION_SetShaderUniformData(Shader, ShaderUniformInfo);

    // Draw All Opaque Meshes
    for (unsigned long i = 0; i < OpaqueMeshes.size(); i++) {

        // Handle Shader Changes If Needed
        int MeshShaderOverrideIndex = OpaqueMeshes[i]->ShaderOverrideIndex_;
        if (MeshShaderOverrideIndex == -1) {
            MeshShaderOverrideIndex = DefaultShaderIndex;
        }
        if (MeshShaderOverrideIndex != CurrentShaderIndex) {
            CurrentShaderIndex = MeshShaderOverrideIndex;
            Shader = Shaders[CurrentShaderIndex];
            Shader->MakeActive();
            ERS_FUNCTION_SetShaderUniformData(Shader, ShaderUniformInfo);
        }

        ERS_FUNCTION_DrawMesh(OpaqueMeshes[i], OpenGLDefaults, Shader);
    }

    // Render Transparent Meshes In Right Order
    for (unsigned long i = 0; i < TransparentMeshes.size(); i++) {

        // Handle Shader Changes If Needed
        int MeshShaderOverrideIndex = TransparentMeshes[i]->ShaderOverrideIndex_;
        if (MeshShaderOverrideIndex == -1) {
            MeshShaderOverrideIndex = DefaultShaderIndex;
        }
        if (MeshShaderOverrideIndex != CurrentShaderIndex) {
            CurrentShaderIndex = MeshShaderOverrideIndex;
            Shader = Shaders[CurrentShaderIndex];
            Shader->MakeActive();
            ERS_FUNCTION_SetShaderUniformData(Shader, ShaderUniformInfo);
        }


        ERS_FUNCTION_DrawMesh(TransparentMeshes[i], OpenGLDefaults, Shader);
    }


    // TODO: Update rendering process
    // should be based around the idea that the models are used to get the meshes to be rendered
    // the meshes are then compiled into two lists to be rendered
    // firstly, there are opaque meshes which are rendered normally (With the depth testing enabled)
    // next, there's the transparent meshes which are sorted by distance on another thread while the opaque meshes are rendering
    // then, these are rendered via depth peeling.

    // A later method would be to implement support for OIT or something fancy like that but this will work for now.  


    // todo: finish moving mesh draw stuff to this directory..


    // Also remove model draw call, as it's no longer used
    // then remove the mesh draw function and put it into here so that it's more consoldiated and self-explanitory
    // then maybe move the renderer code into it's own namespace?

}


void ERS_CLASS_MeshRenderer::RenderSceneNoTextures(ERS_STRUCT_Scene* Scene, ERS_STRUCT_Shader* Shader) {

    ERS_FUNCTION_UpdateMeshTransparency(Scene);

    // Sort Into Pesh Categories
    std::vector<ERS_STRUCT_Mesh*> OpaqueMeshes;
    std::vector<ERS_STRUCT_Mesh*> TransparentMeshes;
    ERS_FUNCTION_MeshTransparencySortNoTextures(&OpaqueMeshes, &TransparentMeshes, Scene);

    // Draw All Opaque Meshes
    for (unsigned long i = 0; i < OpaqueMeshes.size(); i++) {
        ERS_FUNCTION_DrawMeshNoTextures(OpaqueMeshes[i], Shader);
    }
    for (unsigned long i = 0; i < TransparentMeshes.size(); i++) {
        ERS_FUNCTION_DrawMeshNoTextures(TransparentMeshes[i], Shader);
    }

    // ToDO: Make It So That The Transparency Of The Mesh Is Taken Into Account, Rather Than Being completely Bypassed Like It Is Now.

}