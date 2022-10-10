//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_Shader.h>
#include <ERS_STRUCT_Mesh.h>
#include <ERS_STRUCT_Texture.h>
#include <ERS_STRUCT_OpenGLDefaults.h>



/**
 * @brief This structure creates a model object. The model object stores the mesh, textures, and positions. Please see the full list of attrbutes for more information.
 * 
 */
struct ERS_STRUCT_Model {


    // Model Data
    std::vector<ERS_STRUCT_Mesh> Meshes;
    std::vector<ERS_STRUCT_Texture> Textures_Loaded;
    std::string Directory;
    std::string Name = "Name Not Assigned";
    long ShaderOverrideIndex_ = -1; /**<If not -1, This indicates that the model is requesting a custom shader, and the value is the index of the shader in the system*/
    long AssetID;
    int NumMeshes_ = 0; /**<Number of loaded meshes*/

    


    // Shadows Information
    bool CastDynamicShadows_ = true; /**<Indicate if this model is to be rendered when generating the depth maps (dynamically)*/
    bool CastStaticShadows_ = true; /**<Set if this model is rendered when building static depth maps*/
    bool ReceiveShadows_ = true; /**<Determines if this model is to have shadows applied to it.*/
    bool TreatMissingTexturesAsTransparent_ = false; /**<Causes the renderer to bind to a transparent texture for diffuse maps*/


    // New Model Decoding System
    std::vector<ERS_STRUCT_Texture> Textures_;
    int TextureLevelInRAM_ = -1; /**<Determines the current texture level in RAM*/
    int TextureLevelInVRAM_ = -1; /**<Determines the current texture level in RAM*/

    int UserLimitedMinLOD_ = -1; /**<Set the minimum LOD for this asset, as defined by the user.*/
    int UserLimitedMaxLOD_ = 10; /**<Sets the maximul LOD for this asset, again as defined by the user*/

    int MaxTextureLevel_ = 0; /**<Determines the max texture level available, Only should be adjusted by the asset system*/
    
    int AssetLoadngStateVRAM = 0; /**<0 means nothing happening, -1 means unloading, 1 means loading next*/
    int AssetLoadngStateRAM = 0; /**<0 means nothing happening, -1 means unloading, 1 means loading next*/

    int TargetTextureLevelVRAM = 0; /**<Used in preliminary sorting to determine how much this needs to be loaded*/
    int TargetTextureLevelRAM = 0; /**<Used in preliminary sorting to determine how much this needs to be loaded*/

    // Bounding Box Info
    glm::vec3 BoxScale_;  /**<Bounding Box Size In Local Space (Multiply By Object's scale value to get true size)*/
    glm::vec3 BoxOffset_; /**<Offset the center of the box so it's synced with the center of the model*/


    // Texture Streaming System
    bool TexturesBeingLoaded = false; /**<Used to set if the system is already loading textures*/
    bool TexturesBeingPushed = false; /**<Used to set if the system is already pushing textures*/

    std::vector<long> AttachedScriptIndexes_; /**<Indexes of attached scripts (index in the project struct's list of scripts)*/

    double LoadingStartTime_     = 0.0f; /**<Time that the loading started*/
    double LoadingFinishTime_    = 0.0f; /**<Time When Loading Was Completed*/
    double TotalLoadingTime_     = 0.0f; /**<The Total Time Needed To Load*/
    unsigned long TotalVertices_ = 0;    /**<Total Number Of Verts In This Model*/ 
    unsigned long TotalIndices_  = 0;    /**<Total Number Of Indices In Model*/


 

    // Enable Config
    bool Enabled = true;

    // Loading Config
    bool HasTexturesLoaded = false;
    bool HasMeshLoaded = false;
    bool IsReadyForGPU = false;
    bool FullyReady = false;

    // Model Config
    bool GammaCorrection = false;
    bool IsTemplateModel = false; /**<This indicates if the model is a the "master" copy of instanced models.*/

    long MetadataID; /**<Asset ID containing the YAML string with the model's info*/
    long ModelDataID; /**<Asset ID containing the 3d model file used by the asset*/
    
    std::vector<long> TextureIDs;
    

    // Model Information
    glm::vec3 ModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 ModelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 ModelScale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 TrueModelScale = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::mat4 SourceModelLocRotScale_ = glm::mat4(1.0f);
    glm::mat4 ModelLocRotScale_ = glm::mat4(1.0f);




    /**
     * @brief Set the Position object (Overloaded). Accepts X,Y,Z Floats. Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param X 
     * @param Y 
     * @param Z 
     */
    void SetPosition(double X, double Y, double Z);

    /**
     * @brief Set the Position object (Overloaded). Accepts glm::vec3(x,y,z). Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param Position 
     */
    void SetPosition(glm::vec3 Position);

    /**
     * @brief Set the Rotation object (OVerloaded). Accepts X,Y,Z Floats. Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param X 
     * @param Y 
     * @param Z 
     */
    void SetRotation(double X, double Y, double Z);

    /**
     * @brief Set the Rotation object (Overloaded). Accepts glm::vec3(x,y,z). Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param Rotation 
     */
    void SetRotation(glm::vec3 Rotation);


    /**
     * @brief Set the Scale object (OVerloaded). Accepts X,Y,Z Floats. Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param X 
     * @param Y 
     * @param Z 
     */
    void SetScale(double X, double Y, double Z);

    /**
     * @brief Set the Scale object (Overloaded). Accepts glm::vec3(x,y,z). Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param Scale 
     */
    void SetScale(glm::vec3 Scale);

    /**
     * @brief Set the Loc Rot Scale object (Overloaded) Sets the location, rotation, and scale by taking three doubles for each category. (XYZ). Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param LocX 
     * @param LocY 
     * @param LocZ 
     * @param RotX 
     * @param RotY 
     * @param RotZ 
     * @param ScaleX 
     * @param ScaleY 
     * @param ScaleZ 
     */
    void SetLocRotScale(double LocX, double LocY, double LocZ, double RotX, double RotY, double RotZ, double ScaleX, double ScaleY, double ScaleZ);

    /**
     * @brief Set the Loc Rot Scale object (Overloaded). Sets the location, rotation, and scale by taking three glm::vec3 variables for positiom, rotation, scale. Remember to Call ApplyTrsnformations When Ready.
     * 
     * @param Position 
     * @param Rotation 
     * @param Scale 
     */
    void SetLocRotScale(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale);

    /**
     * @brief Applies Transformatins Set By Using the Set (Loc/Rot/Scale) Methods. Returns True on success and False on failure (caused if model is a template model).
     *
     * @return true 
     * @return false 
     */
    bool ApplyTransformations();

    /**
     * @brief Get the Mat 4 object
     * 
     * @return glm::mat4 
     */
    glm::mat4 GetMat4();

    /**
     * @brief Updates the transparency of all meshes contained by this model.
     * 
     */
    void UpdateMeshTransparency();


};



