cmake_minimum_required(VERSION 3.5)
project(Lucifer)


# Include Libs
include(${PROJECT_SOURCE_DIR}/CMake/DisableTargetWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/CMake/FreeImage.cmake)
#include(${PROJECT_SOURCE_DIR}/CMake/DevIL.cmake)



# Setup Library Target
add_library(Lucifer

    "LuciferIL/Lucifer.cpp"
    "LuciferIL/Lucifer_IncludeOnce.cpp"
    
    "LuciferIL/Lucifer_LoadImage.cpp"
    "LuciferIL/Lucifer_LoadImageSTB.cpp"
    "LuciferIL/Lucifer_LoadImageFreeImage.cpp"
    #"LuciferIL/Lucifer_LoadImageDevIL.cpp"
    "LuciferIL/Lucifer_WriteImage.cpp"

    "LuciferIL/Lucifer_ImageUtils.cpp"

    "LuciferIL/Lucifer_LoadingStatusEnum.cpp"
    "LuciferIL/Lucifer_WritingStatusEnum.cpp"
    "LuciferIL/Lucifer_ImageFormatEnum.cpp"
    
    "LuciferIL/Lucifer_ImageStruct.cpp"
    
    "LuciferIL/Lucifer_EnumToString.cpp"
    "LuciferIL/Lucifer_EnumToLogLevel.cpp"
    


    "LuciferIL/Lucifer.h"
    "LuciferIL/Lucifer_IncludeOnce.h"
    
    "LuciferIL/Lucifer_LoadImage.h"
    "LuciferIL/Lucifer_LoadImageSTB.h"
    "LuciferIL/Lucifer_LoadImageFreeImage.h"
    #"LuciferIL/Lucifer_LoadImageDevIL.h"
    "LuciferIL/Lucifer_WriteImage.h"
    
    "LuciferIL/Lucifer_ImageUtils.h"

    "LuciferIL/Lucifer_LoadingStatusEnum.h"
    "LuciferIL/Lucifer_WritingStatusEnum.h"
    "LuciferIL/Lucifer_ImageFormatEnum.h"
    
    "LuciferIL/Lucifer_ImageStruct.h"
    
    "LuciferIL/Lucifer_EnumToString.h"
    "LuciferIL/Lucifer_EnumToLogLevel.h"

)
target_include_directories(Lucifer
    PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/stb

    PRIVATE
        ${PROJECT_SOURCE_DIR}/LuciferIL
        ${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/stb
)


target_link_libraries(Lucifer FreeImage-Lucifer)
#target_link_libraries(Lucifer IL-Lucifer)
