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
 * @brief Struct containing a python script to be run in the editor
 * 
 */
struct ERS_STRUCT_Script {

    std::string Name_; /**<Display name of the script*/
    std::string Code_; /**<Python Code Content Of The Script*/
    long AssetID; /**<AssetID Allocated To This Script*/

};