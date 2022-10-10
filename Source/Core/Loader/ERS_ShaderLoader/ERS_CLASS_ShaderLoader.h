//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Library (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_SystemUtils.h>



/**
 * @brief The Shader Loader Is Responsible For Populating Shader Structs.
 * 
 */
class ERS_CLASS_ShaderLoader {


private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointer To Instance Of SystemUtils*/


public:


    /**
     * @brief Construct a new Shader Loader object
     * 
     * @param SystemUtils
     */
    ERS_CLASS_ShaderLoader(ERS_STRUCT_SystemUtils* SystemUtils);


    /**
     * @brief Destroy the Shader Loader object
     * 
     */
    ~ERS_CLASS_ShaderLoader();
    

    /**
     * @brief Create a Shader Object object
     * 
     * @param VertexText 
     * @param FragmentText 
     * @return ERS_STRUCT_Shader 
     */
   void CreateShaderObject(ERS_STRUCT_Shader* ShaderStruct, std::string VertexText, std::string FragmentText, std::string GeometryText, std::string ComputeText, std::string TCText, std::string TEText, bool LogBuild = true);


    /**
     * @brief Load vertex/fragment shaders from the IOSubsystem.
     * 
     * @param VertexID 
     * @param FragmentID 
     * @return std::shared_ptr<ERS_STRUCT_Shader> 
     */
    void LoadShaderFromAsset(ERS_STRUCT_Shader* ShaderStruct, long VertexID, long FragmentID, long GeometryID = -1, long ComputeID = -1, long TCID = -1, long TEID = -1, std::string ShaderName = "Untitled Shader");



};