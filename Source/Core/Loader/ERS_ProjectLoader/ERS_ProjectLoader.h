//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Project.h>
#include <ERS_STRUCT_ShaderProgramAssetIDs.h>
#include <ERS_STRUCT_Script.h>

#include <ERS_InputOutputSubsystem.h>




/**
 * @brief Class for loading project files
 * 
 */
class ERS_CLASS_ProjectLoader {


private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointer to system utils*/



public:

    /**
     * @brief Construct a new Project Loader object
     * 
     * @param Logger 
     */
    ERS_CLASS_ProjectLoader(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief 
     * 
     */
    ~ERS_CLASS_ProjectLoader();

    /**
     * @brief Load a project from a file path.
     * 
     * @param FilePath 
     */
    ERS_STRUCT_Project LoadProject(long AssetID);


};