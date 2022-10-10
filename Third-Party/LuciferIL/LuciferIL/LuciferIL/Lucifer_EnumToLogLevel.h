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
 * @brief Converts the loading status enum into an int representing the severity of the state.
 * 
 * @param Status Enum returned by image loading functions.
 * @return int String containing name of enum.
 */
int Lucifer_GetLoadingStatusLevel(LoadingStatus Status);

/**
 * @brief Converts the Writing status enum into an int representing the severity of the state.
 * 
 * @param Status Enum returned by image Writing functions.
 * @return int String containing name of enum.
 */
int Lucifer_GetWritingStatusLevel(WritingStatus Status);

};