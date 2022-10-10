//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Scene.h>
#include <ERS_STRUCT_Camera.h>


#include <ERS_CLASS_AssetStreamingSystemResourceMonitor.h>
#include <ERS_CLASS_AsyncTextureUpdater.h>


/**
 * @brief This class is responsible for indicating to the worker threads what needs to be done.
 * The other threads handle loading the vertex data and textures as well as unloading them when told to do so.
 * They also update the model struct to reflect the status of what's in GPU memory to ensure that there are no issues resulting from "cunfusion" as to what's in memory.
 * 
 * To summarize, this class looks at the current scene, and determines what's around the player camera(s) and determines if it needs to be in memory or not.
 * Additionally, it tells the loading system when to start loading more into memory, so that as the camera gets closer to an object, the higher res textures will be loaded.
 * 
 */
class ERS_CLASS_AssetStreamingManager {

private:

    // System Vars
    ERS_STRUCT_Scene* CurrentScene_ = nullptr; /**<Pointer to the current scene*/
    bool HasSceneChanged_ = false; /**<Indicate if the scene has changed since the last frame*/
    std::vector<ERS_STRUCT_Camera*> Cameras_; /**<List of cameras that assets are loaded around*/
    ERS_STRUCT_SystemUtils* SystemUtils_ = nullptr; /**<Struct containing essential services such as logging and Asset IO*/



public:

    // Set Configuration Parameters To Be Used By The Settings Window
    float DistanceCutoffVRAM_ = 9.0f;
    float DistanceCutoffRAM_ = 25.0f;

    bool UseQuadraticDistanceRolloff_ = true; /**<Use Linear Or Quadratic Equations To Determine What LOD the Textures Should Be At*/
    float QuadraticDistanceComponent_ = 0.5f; /**<Parameters to set the loading system's LOD*/
    float LinearDistanceComponent_ = 0.5f; /**<Parameters to set the loading system's LOD*/
    float ConstantDistanceComponent_ = 1.0f; /**<Parameters to set the loading system's LOD*/

    int MinLOD_ = 0; /**<Sets the minimum LOD*/
    int MaxLOD_ = 0; /**<Sets the maximum LOD*/

    int MaxTextureResolution_ = 0; /**<Sets the maximum texture resolution*/
    


    // Class Instances
    std::unique_ptr<ERS_CLASS_AssetStreamingSystemResourceMonitor> ResourceMonitor_; /**<Instance of System Monitor Used For Tex Streaming Choices*/
    std::unique_ptr<ERS_CLASS_AsyncTextureUpdater> AsyncTextureUpdater_; /**<Texture loader/unloader system*/

private:
    // Threads
    std::thread SceneSortingThread_; /**<Thread used to sort the different models in the scene and indicate what textures they should have*/
    std::mutex SortingThreadMutex_; /**<Mutex that can be locked to block the sorting thread*/
    bool SortingThreadShouldExit_ = false; /**<Used to indicate if the thread should be shut down.*/


    /**
     * @brief This function is for internal use, and is the main loop that the sorter thread.
     * 
     */
    void WorkerThread();


    /**
     * @brief Checks to ensure that there's available hardware resources to continue loading. 
     * 
     */
    void CheckHardwareLimitations(ERS_STRUCT_Scene* Scene);

    /**
     * @brief Sorts the models in the given scene based on their distance from the camera.
     * Returns a map containing the distance as well as the index in the model list for that model.
     * 
     * @param Scene 
     * @param Camera 
     * @return std::map<float, unsigned int> 
     */
    std::vector<std::pair<float, unsigned int>> SortModelsByDistanceFromCamera(ERS_STRUCT_Scene* Scene, ERS_STRUCT_Camera* Camera);

    /**
     * @brief Sorts all models in the scene for every camera.
     * Returns map of distances and model index sorted by distance for each camera.
     * 
     * @param Scene 
     * @param Cameras 
     * @return std::vector<std::map<float, unsigned int>> 
     */
    std::vector<std::vector<std::pair<float, unsigned int>>> SortModelsByDistanceFromCameras(ERS_STRUCT_Scene* Scene, std::vector<ERS_STRUCT_Camera*> Cameras);

    /**
     * @brief Generates a dictionary based on the camera's pointer indicating how many loads/unloads should be allowed for every camera
     * This is based on the camera's priority setting.
     * 
     * @param NumberMaxUpdates The total amount of updates that are to be divided up between the different cameras
     * @param Cameras Vector containing all the cameras pointers in it
     * @return std::map<unsigned int, int> Dict containing the camera's index as the key, and the value being the number of loads/unloads it should have
     */
    std::map<unsigned int, int> CalculateCameraMaxUpdates(int NumberMaxUpdates, std::vector<ERS_STRUCT_Camera*> Cameras);

    
    /**
     * @brief Creates a list of models that should be loaded into VRAM ASAP.
     * Prioritizes based on the camera update quotas generated earlier.
     * 
     * @param CameraUpdatesQuota 
     * @param Scene
     * @param DistancesFromCamera
     * @return std::vector<ERS_STRUCT_Model*> 
     */
    std::vector<ERS_STRUCT_Model*> CreateListOfModelsToLoadNextLevelToVRAM(std::map<unsigned int, int> CameraUpdatesQuota, ERS_STRUCT_Scene* Scene, std::vector<std::map<float, unsigned int>> DistancesFromCamera);

    /**
     * @brief Creates a list of models that should be loaded into RAM ASAP.
     * Prioritizes based on the camera update quotas generated earlier.
     * 
     * @param CameraUpdatesQuota 
     * @param Scene
     * @param DistancesFromCamera
     * @return std::vector<ERS_STRUCT_Model*> 
     */
    std::vector<ERS_STRUCT_Model*> CreateListOfModelsToLoadNextLevelToRAM(std::map<unsigned int, int> CameraUpdatesQuota, ERS_STRUCT_Scene* Scene, std::vector<std::map<float, unsigned int>> DistancesFromCamera);

    
    /**
     * @brief Iterates over all models in the scene and sorts what they should be doing.
     * 
     * @param CameraUpdatesQuota 
     * @param DistancesFromCamera 
     * @param Scene 
     */
    void SortSceneModels(std::map<unsigned int, int> CameraUpdatesQuota, std::vector<std::vector<std::pair<float, unsigned int>>> DistancesFromCamera, ERS_STRUCT_Scene* Scene);



public:

    /**
     * @brief Constructor, sets up threads used to sort what models are prioritized, etc.
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_AssetStreamingManager(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Cleans up sorting threads, destroys class instance.
     * 
     */
    ~ERS_CLASS_AssetStreamingManager();

    /**
     * @brief Call this once there's a valid opengl context setup - only once that has been created can this be called.
     * Calling this earlier will result in a segmentation fault.
     * 
     */
    void SetupTextureStreamer(GLFWwindow* Window);


    /**
     * @brief Tells the streaming manager what the current scene is so that it knows what assets are actually relevant and what aren't.
     * We handle what the current scene is internally, so calling this every frame is okay, we check if the pointer is the same.
     * If you want to start loading another scene in advance (perhaps the camera is getting close to a scene switch, use the PreloadScene function).
     * 
     * @param CurrentScene 
     */
    void SetCurrentScene(ERS_STRUCT_Scene* CurrentScene);

    /**
     * @brief Not yet implemented!
     * 
     * Will start loading this new scene in the background so our loading time is shorter when we switch scenes.
     * 
     * @param Scene 
     */
    void PreloadScene(ERS_STRUCT_Scene* Scene);

    /**
     * @brief Give us a list containing the cameras in the scene so we know where we need to keep the textures loaded in versus where we can dump them.
     * Feel free to call this every frame or only when the cameras change, we monitor what's going on internally so it doesn't matter. (Just make sure we have no null pointers!)
     * 
     * @param Cameras 
     */
    void SetCameraStructs(std::vector<ERS_STRUCT_Camera*> Cameras);

    /**
     * @brief Goes through the scene and marks different models to be updated
     * 
     * @param Scene 
     * @param Cameras 
     */
    void UpdateSceneStreamingQueue(ERS_STRUCT_Scene* Scene, std::vector<ERS_STRUCT_Camera*> Cameras);

};
