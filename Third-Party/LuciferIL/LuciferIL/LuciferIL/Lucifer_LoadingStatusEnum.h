//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once


namespace Lucifer {


enum LoadingStatus {
    Lucifer_LoadingStatus_Complete,
    Lucifer_LoadingStatus_InvalidData,
    Lucifer_LoadingStatus_UnsupportedFormat,
    Lucifer_LoadingStatus_InvalidNumChannels,
    Lucifer_LoadingStatus_InvalidWidth,
    Lucifer_LoadingStatus_InvalidHeight, 
    Lucifer_LoadingStatus_UnknownError
};


};