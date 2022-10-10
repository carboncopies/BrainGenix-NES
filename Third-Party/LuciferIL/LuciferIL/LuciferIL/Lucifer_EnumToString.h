//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once

// Std Lib Includes
#include <string>


// Internal Includes
#include <LuciferIL/Lucifer_LoadingStatusEnum.h>
#include <LuciferIL/Lucifer_WritingStatusEnum.h>


namespace Lucifer {

/**
 * @brief Converts the loading status enum into a string that can be used for logging.
 * 
 * @param Status Enum returned by image loading functions.
 * @return std::string String containing name of enum.
 */
std::string Lucifer_GetLoadingStatusString(LoadingStatus Status);


/**
 * @brief Converts the writing status enum into a string that can be used for logging.
 * 
 * @param Status Enum returned by image Writing functions.
 * @return std::string String containing name of enum.
 */
std::string Lucifer_GetWritingStatusString(WritingStatus Status);


};