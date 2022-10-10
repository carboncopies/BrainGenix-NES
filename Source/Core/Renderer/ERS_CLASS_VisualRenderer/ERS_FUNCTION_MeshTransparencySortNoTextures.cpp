//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_MeshTransparencySortNoTextures.h>


void ERS_FUNCTION_MeshTransparencySortNoTextures(std::vector<ERS_STRUCT_Mesh*>* OpaqueMeshes, std::vector<ERS_STRUCT_Mesh*>* TransparentMeshes, ERS_STRUCT_Scene* Scene) {


    for (unsigned long i = 0; i < Scene->Models.size(); i++) {

        ERS_STRUCT_Model *Model = Scene->Models[i].get();
        glm::mat4 ModelMatrix = Model->GetMat4();
        if (Model->FullyReady && Model->Enabled && Model->CastDynamicShadows_) {
            for (unsigned int i = 0; i < Model->Meshes.size(); i++) {

                ERS_STRUCT_Mesh* Mesh = &Model->Meshes[i];
                Mesh->ModelMatrix = ModelMatrix;
                if (Mesh->HasTransparency_) {
                    TransparentMeshes->push_back(Mesh);
                } else {
                    OpaqueMeshes->push_back(Mesh);
                }

            }
        }

    }


}