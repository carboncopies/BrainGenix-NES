//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_ResetMeshTexture.h>


void ERS_FUNCTION_ResetMeshTexture(const char* Name, int Offset, unsigned int ShaderProgram, unsigned int TextureID) {

    glUniform1i(glGetUniformLocation(ShaderProgram, Name), Offset);
    glActiveTexture(GL_TEXTURE0 + Offset);
    glBindTexture(GL_TEXTURE_2D, TextureID);

}