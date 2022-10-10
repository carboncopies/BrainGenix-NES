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
 * @brief Stores Location, Rotation, Scale data in struct.
 * 
 */
struct ERS_STRUCT_ShaderProgramAssetIDs {

    // Name Of Shader Program
    std::string Name; /**<Name of the shader program, used to identify different programs*/

    // Shader Asset IDs
    long VertexID; /**<Vertex Shader Asset ID*/
    long FragmentID; /**<Fragment Shader Asset ID*/
    long GeometryID; /**<Shader Asset ID*/
    long ComputeID; /**<Shader Asset ID*/
    long TCID; /**<Shader Asset ID*/
    long TEID; /**<Shader Asset ID*/
};