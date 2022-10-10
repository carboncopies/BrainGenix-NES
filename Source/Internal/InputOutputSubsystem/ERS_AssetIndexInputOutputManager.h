//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <map>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_STRUCT_IOData.h>


class ERS_AssetIndexInputOutputManager {

private:

    ERS_LoggingSystem* Logger_; /**<Pointer To Logger Instance*/
    std::mutex Lock_; /**<Stops other threads from accessing the maps when in use*/


public:

    std::map<long, std::string> AssetTypeName_; /**<Name of the type of asset, ex: Image or Model*/
    std::map<long, std::string> AssetCreationDate_; /**<Date when initially Allocated*/
    std::map<long, std::string> AssetModificationDate_; /**<Date When Last Modified*/
    std::map<long, std::string> AssetFileName_; /**<Original name of object when imported*/
    std::vector<long> AssetIDsFound_; /**<List of all asset ids in metadata system*/

public:

    /**
     * @brief Construct a new ers class assetindexIOM object
     * 
     * @param Logger 
     */
    ERS_AssetIndexInputOutputManager(ERS_LoggingSystem* Logger);

    /**
     * @brief Destroy the ers class assetindexIOM object
     * 
     */
    ~ERS_AssetIndexInputOutputManager();
    
    /**
     * @brief Loads the asset index data into memory which is used by the io subsystem.
     * 
     * @param Data 
     * @return true 
     * @return false 
     */
    bool LoadAssetIndex(ERS_STRUCT_IOData* Data);

    /**
     * @brief Writes Asset Index Data To IOData Struct Passed In. Returns True/False For Success/Failure.
     * 
     * @param Data 
     * @return true 
     * @return false 
     */
    bool WriteAssetIndex(ERS_STRUCT_IOData* Data);

    /**
     * @brief This function is used to update the internal maps and metadata when an asset is saved.
     * 
     * @param AssetID 
     * @param Data 
     */
    bool UpdateAssetIndex(long AssetID, ERS_STRUCT_IOData* Data);

    /**
     * @brief Populates The Asset Metadata Parameters In the Data Struct Passed In Based On The AssetID Lookup
     * 
     * @param AssetID 
     * @param Data 
     */
    bool ReadAssetIndex(long AssetID, ERS_STRUCT_IOData* Data);

};