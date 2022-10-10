//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <future>
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <ThreadNamingUtils/SetThreadName.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ModelWriterData.h>

#include <ERS_ModelWriter.h>
#include <ERS_ExternalModelLoader.h>

/**
 * @brief This class provides the backend to the import asset option within the file menu.
 *
 */
class ERS_ModelImporter {

    private:

        ERS_STRUCT_SystemUtils* SystemUtils_; /**<used to get access to system utilites like IOmanager, logger, etc.*/
        std::mutex LockAssetImportQueue_; /**<Mutex used to control access to list of assets to be imported*/
        std::mutex BlockThread_; /**<Use This To Block The Thread*/
        bool StopThread_ = false; /**<Set this to true to make the importer thread exit*/
        bool HasJobFinished_ = false; /**<Indicate If A Job Has Finished*/
        std::thread ImportThread_; /**<Import Processor Thread*/
        std::vector<std::string> AssetImportQueue_; /**<List of assets to be imported, accessed by other threads so use mutex to control access*/
        std::vector<bool> AssetQueueFlipTextures_; /**<List of assets flip texture parameter*/
        std::vector<std::future<void>> ProcessingItems_; /**<List of items currently being worked on by the threads*/

        std::unique_ptr<ERS_CLASS_ModelWriter>         ModelWriter_; /**<Instance of the model writer, used to save models to the ERS project*/
        std::unique_ptr<BrainGenix::ERS::Module::ExternalModelLoader> ModelLoader_; /**<Used to load models from outside the ERS project*/

        std::unique_ptr<ERS_CLASS_ModelWriter> ERS_ModelWriter_; /**<Pointer To ERS Model Writer Instance*/


        // Stats
        long TotalItemsToImport_ = 0; /**<Stats for the loading bar*/
        long TotalItemsProcessed_ = 0; /**<Stats for the loading bar*/


        void ImportThread(); /**<Import Item Thread*/

    public:


        /**
         * @brief Construct a new ers class importasset object
         * 
         * @param SystemUtils 
         */
        ERS_ModelImporter(ERS_STRUCT_SystemUtils* SystemUtils);

        /**
         * @brief Destroy the ers class importasset object
         * 
         */
        ~ERS_ModelImporter();


        /**
         * @brief Add items to be imported.
         * 
         * @param AssetPaths 
         */
        void AddToImportQueue(std::vector<std::string> AssetPaths, std::vector<bool> FlipTextures);



        /**
         * @brief Get the Total Subitems Imported number
         * 
         * @return long 
         */
        long GetTotalSubitemsImported();

        /**
         * @brief Get the Total Subitems number
         * 
         * @return long 
         */
        long GetTotalSubitems();

        /**
         * @brief Get the Total Items To Import object
         * 
         * @return long 
         */
        long GetTotalItemsToImport();

        /**
         * @brief Get the Total Items Imported object
         * 
         * @return long 
         */
        long GetTotalItemsImported();


        /**
         * @brief Check if the import job is done, (to close the import progress bar)
         * 
         * @return true 
         * @return false 
         */
        bool HasJobFinished();

};