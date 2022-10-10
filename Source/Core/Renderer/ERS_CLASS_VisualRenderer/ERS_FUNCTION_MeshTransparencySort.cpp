//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_MeshTransparencySort.h>


void ERS_FUNCTION_MeshTransparencySort(std::vector<ERS_STRUCT_Mesh*>* OpaqueMeshes, std::vector<ERS_STRUCT_Mesh*>* TransparentMeshes, ERS_STRUCT_Scene* Scene) {

    // Iterate Over All Models
    for (unsigned long i = 0; i < Scene->Models.size(); i++) {

        ERS_STRUCT_Model *Model = Scene->Models[i].get();
        glm::mat4 ModelMatrix = Model->GetMat4();

        // If Model's meshes are to be rendered
        if (Model->FullyReady && Model->Enabled) {

            // Iterate over all meshes, and add pointer to mesh array, sort
            for (unsigned int i = 0; i < Model->Meshes.size(); i++) {

                // Get Pointer From Model Mesh Array
                ERS_STRUCT_Mesh* Mesh = &Model->Meshes[i];

                // Copy Data
                Mesh->ModelMatrix = ModelMatrix;
                Mesh->ShaderOverrideIndex_ = Model->ShaderOverrideIndex_;


                if (Mesh->HasTransparency_) {
                    TransparentMeshes->push_back(Mesh);
                } else {
                    OpaqueMeshes->push_back(Mesh);
                }

            }
        }

    }


}