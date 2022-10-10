//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once


namespace Lucifer {


enum WritingStatus {
    Lucifer_WritingStatus_Complete,
    Lucifer_WritingStatus_InvalidData,
    Lucifer_WritingStatus_UnsupportedFormat,
    Lucifer_WritingStatus_InvalidNumChannels,
    Lucifer_WritingStatus_InvalidWidth,
    Lucifer_WritingStatus_InvalidHeight, 
    Lucifer_WritingStatus_UnknownError
};


};