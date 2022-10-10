// ToDO: then add to project struct, then update project loader/writer with this info. Then check trello board for other related tasks like live ediitng.


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")



/**
 * @brief Struct containing the info about an object in the scene
 * 
 */
struct ERS_STRUCT_SceneObject {

    std::string Type_; /**<Name Of The Type Of The Object*/
    std::string Label_; /**<Human Readable Name Of The Object*/
    unsigned long Index_; /**<Index in respective list (e.g. If Type Is Model, This Would Be The Index In The Model List)*/

};