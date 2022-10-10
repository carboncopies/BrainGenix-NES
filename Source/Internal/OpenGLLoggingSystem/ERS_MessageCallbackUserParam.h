//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Simple struct used to pass in user parameters to the callback function.
 * 
 */
struct ERS_STRUCT_MessageCallbackParam {

    void* Logger_;
    void* OpenGLLoggingSystem_;

    bool* LogSeverityHigh_;
    bool* LogSeverityMedium_;
    bool* LogSeverityLow_;
    bool* LogSeverityNotification_;

    bool* LogTypeError_;
    bool* LogTypeDeprecatedBehavior_;
    bool* LogTypeUndefinedBehavior_;
    bool* LogTypePortability_;
    bool* LogTypePerformance_;
    bool* LogTypeMarker_;
    bool* LogTypePushGroup_;
    bool* LogTypePopGroup_;
    bool* LogTypeOther_;

    bool* LogSourceApi_;
    bool* LogSourceWindowSystem_;
    bool* LogSourceShaderCompiler_;
    bool* LogSourceThirdParty_;
    bool* LogSourceApplication_;
    bool* LogSourceOther_;
};