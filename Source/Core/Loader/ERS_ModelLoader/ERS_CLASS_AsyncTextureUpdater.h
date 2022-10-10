//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <FreeImage.h>

#include <ThreadNamingUtils/SetThreadName.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Scene.h>

#include <ERS_CLASS_AssetStreamingSystemResourceMonitor.h>



/**
 * @brief This class is responsible for managing the different textures that the model needs. 
 * As the Asset Streaming Manager Tells us what to do, here we use lots of threads to load the different textures and move them in and out of GPU memory and regular system RAM.
 * 
 * Texture Caching:
 * We have two different levels of caching here, firstly there's GPU ram for textures actively in use or frequently in use.
 * When a texture is deemed not importent enough to be moved out of that level, we store it in CPU memory until that's full. 
 * This lets us keep our textures as close to the GPU as possble which lets us reduce pop-in issues.
 * 
 */
class ERS_CLASS_AsyncTextureUpdater {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Struct containing essential services such as logging and Asset IO*/
    ERS_CLASS_AssetStreamingSystemResourceMonitor* ResourceMonitor_; /**<used to keep track of system resource budgets*/
    GLFWwindow* MainThreadWindowContext_; /**<GLFWWindow Object Used To Setup Shared Contexts*/

    int NumPusherThreads_ = 0; /**<Target Number of threads that we should be using*/
    int NumLoaderThreads_ = 0; /**<Target Number of threads that we should be using*/
    int WorkQueueLimit_ = 512; /**<Sets the maxinum number of itesm in the queue - won't add anything else to queue once this limit reached - prevents loading where the player already left*/
    bool PreventDuplicateWorkItems_ = true; /**<Stop Dupe Queue Entries*/
    bool PrioritizeQueueByVisualImpact_ = true; /**<Sort the queue by the texture level to be loaded*/

    long long unsigned int MinRAMCutoff_  = 268435456; /**<Set the minimum RAM cutoff value*/
    long long unsigned int MinVRAMCutoff_ = 134217728; /**<Set the minimum VRAM cutoff value*/
    

    std::vector<std::thread> TextureLoaderThreads_; /**<Vector containing thread objects for the texture loading pool*/
    std::vector<std::thread> TexturePusherThreads_; /**<Vector containing thread objects for the texture pushing pool*/
    
    std::mutex BlockLoaderThreads_; /**<Lock this to block all the treads (Usually done to add new items to the work queue)*/
    std::mutex BlockPusherThreads_; /**<Lock this to block all the treads (Usually done to add new items to the work queue)*/

    std::atomic_bool StopPusherThreads_; /**<Used to start/stop threads*/
    std::atomic_bool StopLoaderThreads_; /**<Used to start/stop threads*/
    std::atomic_bool PusherThreadReady_; /**<Causes this system to wait until the pusher thread is ready before creating another one*/

    std::vector<std::shared_ptr<ERS_STRUCT_Model>> LoadWorkItems_; /**<Models that need to have textures loaded from disk into memory*/
    std::vector<std::shared_ptr<ERS_STRUCT_Model>> PushWorkItems_; /**<Models that need to have textures loaded to gpu from memory*/




    void TexturePusherThread(int Index);
    void TextureLoaderThread(int Index);


    void FreeRAMAllocation(ERS_STRUCT_TextureLevel &Level);
    void FreeVRAMAllocation(ERS_STRUCT_TextureLevel &Level);
    


    /**
     * @brief Loads the given mip-map level fromt the texture provided as a FIBITMAP
     * Returns true on success, false otherwise
     * 
     * @param Texture Target texture struct
     * @param Level What texture level should we load
     * @param LogEnable Enable or disable error logging here
     * 
     * @return true 
     * @return false 
     */
    bool LoadImageDataRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable = true);

    /**
     * @brief Unloads the target texture level from the texture struct
     * returns true on success, false otherwise
     * 
     * @param Texture 
     * @param Level 
     * @param LogEnable 
     * @return true 
     * @return false 
     */
    bool UnloadImageDataRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable = true);

    /**
     * @brief Load the given texture level into opengl, and assign an object id for use.
     * Returns true on success, false otherwise.
     * 
     * @param Texture 
     * @param Level 
     * @param LogEnable 
     * @return true 
     * @return false 
     */
    bool LoadImageDataVRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable = true);


    /**
     * @brief Removes a given texture level from opengl memory 
     * Returns true on success, false otherwise.
     * 
     * @param Texture 
     * @param Level 
     * @param LogEnable 
     * @return true 
     * @return false 
     */
    bool UnloadImageDataVRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable = true);

    /**
     * @brief Process a model that needs some texture modification operation performed
     * 
     * @param Model 
     */
    //void ProcessWorkItem(ERS_STRUCT_Model* Model);

    void ProcessLoadWorkItem(ERS_STRUCT_Model* Model);
    void ProcessPushWorkItem(ERS_STRUCT_Model* Model);

    /**
     * @brief Loads/Unloads all textures in the model as needed to get the target level (For RAM).
     * 
     * @param Model 
     * @param LogEnable
     */
    void SetLevelRAM(ERS_STRUCT_Model* Model, bool LogEnable = true);

    /**
     * @brief Loads/Unloads all textures in the model as needed to get the target level (For VRAM).
     * 
     * @param Model 
     * @param LogEnable
     */
    void SetLevelVRAM(ERS_STRUCT_Model* Model, bool LogEnable = true);

    void ProcessVRAMUpdate(int Index, ERS_STRUCT_Scene* Scene);
    void ProcessRAMUpdate(int Index, ERS_STRUCT_Scene* Scene);

public:

    std::vector<bool>  RAMBeingProcessed_;
    std::vector<bool> VRAMBeingProcessed_;
    
    std::string RAMQueueString = "";
    std::string VRAMQueueString = "";

    /**
     * @brief Create instance of the texture updater. The optional threads parameters sets the limit on the number of threads we can use.
     * 
     * @param SystemUtils 
     * @param Threads Setting this to 0 will autodetect the number of threads we should use. Otherwise, we'll use whatever is passed in.
     */
    ERS_CLASS_AsyncTextureUpdater(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_AssetStreamingSystemResourceMonitor* ResourceMonitor, GLFWwindow* Window, unsigned int Threads = 0);

    /**
     * @brief Destroy the ers class asynctextureupdater object
     * 
     */
    ~ERS_CLASS_AsyncTextureUpdater();



    /**
     * @brief Dumps all push items in a last-ditch effort to free memory before the sytem runs out.
     * 
     */
    void QueuePanic();


    /**
     * @brief Sorts through all items in the scene and automatically adds them to the update queue.
     * Ideally this should be called every frame to ensure that the models marked for updates are updated.
     * 
     * @param Scene 
     */
    void SortModels(ERS_STRUCT_Scene* Scene);


    /**
     * @brief Returns the number of threads being used by the streaming system
     * 
     * @return int 
     */
    int GetNumLoaderThreads();
    int GetNumStreamerThreads();

    /**
     * @brief Tell the system how many threads we should be using - only applies when we teardown threads and then set them up again.
     * 
     * @param NumThreads
     */
    void SetNumLoaderThreads(int NumThreads);
    void SetNumStreamerThreads(int NumThreads);

    /**
     * @brief Create the threads based on the numthreads setting
     * 
     */
    void SetupPusherThreads();
    void SetupLoaderThreads();
    
    /**
     * @brief Destroy threads, used in destructor or when changing number of threads.
     * 
     */
    void TeardownPusherThreads();
    void TeardownLoaderThreads();


    int GetQueueLimit();
    void SetQueueLimit(int QueueLimit);
    bool GetDupeQueueEntryPrevention();
    void SetDupeQueueEntryPrevention(bool State);
    bool GetWorkQueuePrioritization();
    void SetDupeQueueEntryPreventi(bool State);
    bool GetQueuePrioritizationEnabled();
    void SetQueuePrioritizationEnabled(bool State);


};

