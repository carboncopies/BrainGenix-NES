//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_FindShaderByName.h>


unsigned int ERS_FUNCTION_FindShaderByName(std::string Name, std::vector<std::unique_ptr<ERS_STRUCT_Shader>>* Shaders) {

    long Index = 0;
    for (unsigned long i = 0; i < Shaders->size(); i++) {
        if ((*Shaders)[i]->InternalName == Name) {
            Index = i;
            break;
        }
    }

    return Index;

}