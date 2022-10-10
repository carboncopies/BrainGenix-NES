//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#pragma once

// Standard Library (BG convention: use <> instead of "")
#include <vector>
#include <string>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>



/**
 * @brief Structure To Store Shader Object
 * 
 */
struct ERS_STRUCT_Shader {


    unsigned int ShaderProgram_; /**<OpenGL refernece ID for Shader Program */
    unsigned int VertexShader; /**<OpenGL Reference ID for Vertex Shader */
    unsigned int FragmentShader; /**<OpenGL Reference ID For Fragment Shader */

    bool HasGeometryShader; /**<Indicate if the shader uses geometry shaders or not*/
    unsigned int GeometryShader; /*<<OpenGL ID For Geometry Shader (NOTE: Optional)*/
    bool HasComputeShader; /**<Indicate if the shader uses compute shaders or not*/
    unsigned int ComputeShader; /**<OpenGL ID For Compute Shader (NOTE: Optional)*/
    bool HasTCShader; /**<Indicate if the shader uses TC shaders or not*/
    unsigned int TCShader; /**<OpenGL ID For TC Shader (NOTE: Optional)*/
    bool HasTEShader; /**<Indicate if the shader uses TE shaders or not*/
    unsigned int TEShader; /**<OpenGL ID For TE Shader (NOTE: Optional)*/

    

    std::string DisplayName = "Undefined"; /**<Name of shader to be shown to the user*/
    std::string InternalName = "Undefined"; /**<Intenral name of the shader, these are usually the same (Just not in the shader editor as it creates the preview shader)*/

    long VertexID; /**<Asset ID of vertex shader (optional, used for saving) */
    long FragmentID; /**<Asset ID of fragment shader (optional, used for saving) */
    long GeometryID; /**<Asset ID of geometry shader (optional, used for saving) */
    long ComputeID; /**<Asset ID of compute shader (optional, used for saving) */
    long TessellationControlShaderID; /**<Asset ID of TCS shader (optional, used for saving) */
    long TessellationEvaluationShaderID; /**<Asset ID of TES shader (optional, used for saving) */


    /**
     * @brief Construct a new ers struct shader object
     * 
     */
    ERS_STRUCT_Shader();

    /**
     * @brief Destroy the ers struct shader object
     * 
     */
    ~ERS_STRUCT_Shader();

    /**
     * @brief Function To Compile Vertex Shader With Given Code (const char*)
     * 
     * @param VertexText 
     */
    std::string CompileVertexShader(const char* VertexText, ERS_LoggingSystem* Logger = nullptr);

    /**
     * @brief Function To Compile Fragment Shader With Given Code (const char*)
     * 
     * @param FragmentText 
     */
    std::string CompileFragmentShader(const char* FragmentText, ERS_LoggingSystem* Logger = nullptr);

    /**
     * @brief Function To Compile Geometry Shader With Given Code (const char*)
     * 
     * @param GeometryText 
     */
    std::string CompileGeometryShader(const char* GeometryText, ERS_LoggingSystem* Logger = nullptr);

    /**
     * @brief Function To Compile Compute Shader With Given Code (const char*)
     * 
     * @param ComputeText 
     */
    std::string CompileComputeShader(const char* ComputeText, ERS_LoggingSystem* Logger = nullptr);

    /**
     * @brief Function To Compile Compute Shader With Given Code (const char*)
     * 
     * @param ComputeText 
     */
    std::string CompileTCShader(const char* TCText, ERS_LoggingSystem* Logger = nullptr);

    /**
     * @brief Function To Compile Compute Shader With Given Code (const char*)
     * 
     * @param ComputeText 
     */
    std::string CompileTEShader(const char* TEText, ERS_LoggingSystem* Logger = nullptr);


    /**
     * @brief Create a Shader Program object from Fragment and Vertex Shaders.
     * 
     * @param DeleteShadersUponLink 
     */
    std::string CreateShaderProgram(ERS_LoggingSystem* Logger, bool LogEnable = true);

    /**
     * @brief Make The Shader Stored Within The Struct Active.
     * 
     */
    bool MakeActive();


    /**
     * @brief Clears and resets the shader program, used in the shader editor.
     * 
     */
    void ResetProgram();


    // Population Functions
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Bool object
     * 
     * @param name 
     * @param value 
     */
    void SetBool(const std::string &name, bool value) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Int object
     * 
     * @param name 
     * @param value 
     */
    void SetInt(const std::string &name, int value) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Float object
     * 
     * @param name 
     * @param value 
     */
    void SetFloat(const std::string &name, float value) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Vec 2 object
     * 
     * @param name 
     * @param value 
     */
    void SetVec2(const std::string &name, const glm::vec2 &value) const;
    /**
     * @brief Set the Vec 2 object
     * 
     * @param name 
     * @param x 
     * @param y 
     */
    void SetVec2(const std::string &name, float x, float y) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Vec 3 object
     * 
     * @param name 
     * @param value 
     */
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    /**
     * @brief Set the Vec 3 object
     * 
     * @param name 
     * @param x 
     * @param y 
     * @param z 
     */
    void SetVec3(const std::string &name, float x, float y, float z) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Vec 4 object
     * 
     * @param name 
     * @param value 
     */
    void SetVec4(const std::string &name, const glm::vec4 &value) const;
    /**
     * @brief Set the Vec 4 object
     * 
     * @param name 
     * @param x 
     * @param y 
     * @param z 
     * @param w 
     */
    void SetVec4(const std::string &name, float x, float y, float z, float w);
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Mat 2 object
     * 
     * @param name 
     * @param mat 
     */
    void SetMat2(const std::string &name, const glm::mat2 &mat) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Mat 3 object
     * 
     * @param name 
     * @param mat 
     */
    void SetMat3(const std::string &name, const glm::mat3 &mat) const;
    // ------------------------------------------------------------------------
    /**
     * @brief Set the Mat 4 object
     * 
     * @param name 
     * @param mat 
     */
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

};

// FIXME: MOVE THISE TO OTHER (seperate) Shader Loader Class

