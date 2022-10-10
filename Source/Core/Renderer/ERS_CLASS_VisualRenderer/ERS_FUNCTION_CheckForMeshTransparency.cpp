//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_CheckForMeshTransparency.h>


void ERS_FUNCTION_UpdateMeshTransparency(ERS_STRUCT_Scene* Scene) {

    for (unsigned long i = 0; i < Scene->Models.size(); i++) {
        Scene->Models[i]->UpdateMeshTransparency();
    }


}