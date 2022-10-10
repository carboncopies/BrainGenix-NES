// ToDO: then add to project struct, then update project loader/writer with this info. Then check trello board for other related tasks like live ediitng.


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

// Internal Libraries (BG convention: use <> instead of "")



/**
 * @brief Struct containing renderer settings.
 * 
 */
struct ERS_OpenGLLogItem {

    std::string Message_; /**<Message To Be Logged*/
    
    GLenum Source_; /**<OpenGL Parameter*/
    GLenum Type_; /**<OpenGL Parameter*/
    GLuint ID_; /**<OpenGL Parameter*/
    GLenum Severity_; /**<OpenGL Parameter*/

};