//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>



#include <ERS_OpenGLLogItem.h>
#include <ERS_MessageCallbackUserParam.h>


/**
 * @brief Used as the callback to collect opengl debug messages and log them in the ERS system.
 * 
 * @param GLSource 
 * @param GLType 
 * @param GLID 
 * @param GLSeverity 
 * @param GLMessage 
 * @param UserData 
 */
void APIENTRY ERS_MessageCallback(GLenum GLSource, GLenum GLType, GLuint GLID, GLenum GLSeverity, GLsizei, const GLchar *GLMessage, const void *UserData);


/**
 * @brief This Class Provides A System To Get OpenGL Context Issues as well as debug other things related to opengl.
 * 
 */
class ERS_OpenGLLoggingSystem {

private:

    ERS_LoggingSystem* Logger_; /**<Class to prettyify the opengl logs once they're extracted*/
    ERS_STRUCT_MessageCallbackParam UserParam_; /**<Used In Debug Callback*/

public:

    std::vector<ERS_OpenGLLogItem> LogItems_; /**<List of log items as gotten from the opengl system*/
    std::mutex LogItemMutex_; /**<Mutex To Enxure Thread Safety For The Log Items Vector*/

    bool LogSeverityHigh_ =             true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSeverityMedium_ =           true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSeverityLow_ =              true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSeverityNotification_ =    false; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/

    bool LogTypeError_ =                true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypeDeprecatedBehavior_ =   true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypeUndefinedBehavior_ =    true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypePortability_ =          true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypePerformance_ =          true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypeMarker_ =               true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypePushGroup_ =            true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypePopGroup_ =             true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogTypeOther_ =                true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/

    bool LogSourceApi_ =                true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSourceWindowSystem_ =       true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSourceShaderCompiler_ =     true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSourceThirdParty_ =         true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSourceApplication_ =        true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/
    bool LogSourceOther_ =              true; /**<Parameter Passed To Message Callback System Controlling Log Verbosity*/


    /**
     * @brief Construct a new ers class openglloggingsystem object
     * 
     * @param Logger 
     */
    ERS_OpenGLLoggingSystem(ERS_LoggingSystem* Logger);

    /**
     * @brief Destroy the ers class openglloggingsystem object
     * 
     */
    ~ERS_OpenGLLoggingSystem();


    /**
     * @brief Use This To Enable Or Disable Collection of Logs From OpenGL
     * 
     * @param Status 
     */
    void SetCollectionStatus(bool Status);

};