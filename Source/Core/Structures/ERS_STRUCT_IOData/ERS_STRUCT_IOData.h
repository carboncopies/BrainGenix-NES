//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Structure to shuttle around data to and from loading/writing.
 * 
 */
struct ERS_STRUCT_IOData {

    std::unique_ptr<unsigned char[]> Data; /**<Char Array Containing Raw Data*/
   

    std::string AssetTypeName = "Undefined"; /**<Name Of The Type Of Asset Data Stored Here*/
    std::string AssetCreationDate = "1970-01-01-00-00-01"; /**<Date when Asset Created (YYYY-MM-DD-HH-MM-SS)*/
    std::string AssetModificationDate = "1970-01-01-00-00-01"; /**<Date when Asset Was Last Modified (YYYY-MM-DD-HH-MM-SS)*/
    std::string AssetFileName = ""; /**<Name of the original file that the asset waS named prior to importing*/


    bool HasLoaded = false; /**<Status Variable*/

    float LoadTime_s = -1; /**<Float Indicating Number Of Seconds Loading Took, If Not Set, Value Is -1*/
    float WriteTime_s = -1; /**<Float Indiciating Number Of Seconds Writing Took, If Not Set, Value Is -1*/

    double Size_B; /**<Number Indicating Size Of Data*/
    double Size_KB; /**<Size In Kilobytes*/
    double Size_MB; /**<Size In Megabytes*/
    double Size_GB; /**<Size In Gigabytes*/

    float LoadSpeed_GBs; /**<Loading Speed In Gigabytes/Second*/
    float LoadSpeed_MBs; /**<Loading Speed In Megabytes/Second*/
    float LoadSpeed_KBs; /**<Loading Speed In Kilobytes/Second*/

    float WriteSpeed_GBs; /**<Write Speed In Gigabytes/Second*/
    float WriteSpeed_MBs; /**<Write Speed In Megabytes/Second*/
    float WriteSpeed_KBs; /**<Write Speed In Kilobytes/Second*/


};