//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_OpenGLLoggingSystem.h>



// Callback function for printing debug statements
void APIENTRY ERS_MessageCallback(GLenum GLSource, GLenum GLType, GLuint GLID, GLenum GLSeverity, GLsizei, const GLchar *GLMessage, const void *UserData) {

    // Get System Pointers From User Data Void Ptr
    ERS_STRUCT_MessageCallbackParam* UserParamStruct = (ERS_STRUCT_MessageCallbackParam*)UserData;
    ERS_LoggingSystem* Logger = (ERS_LoggingSystem*)UserParamStruct->Logger_;
    ERS_OpenGLLoggingSystem* OpenGLLoggingSystem = (ERS_OpenGLLoggingSystem*)UserParamStruct->OpenGLLoggingSystem_;

    // Convert GL Enum To Readable Strings
    std::string Source;
    std::string Type;
    std::string Severity;
    
    int LogLevel;
    bool WillLog = true;

    switch (GLSource) {
        case GL_DEBUG_SOURCE_API:
        Source = "API";
        if (!OpenGLLoggingSystem->LogSourceApi_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        Source = "WINDOW SYSTEM";
        if (!OpenGLLoggingSystem->LogSourceWindowSystem_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        Source = "SHADER COMPILER";
        if (!OpenGLLoggingSystem->LogSourceShaderCompiler_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        Source = "THIRD PARTY";
        if (!OpenGLLoggingSystem->LogSourceThirdParty_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        Source = "APPLICATION";
        if (!OpenGLLoggingSystem->LogSourceApplication_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SOURCE_OTHER:
        Source = "OTHER";
        if (!OpenGLLoggingSystem->LogSourceOther_){
            WillLog = false;
        }
        break;

        default:
        Source = "UNKNOWN";
        break;
    }

    switch (GLType) {
        case GL_DEBUG_TYPE_ERROR:
        Type = "ERROR";
        if (!OpenGLLoggingSystem->LogTypeError_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        Type = "DEPRECATED BEHAVIOR";
        if (!OpenGLLoggingSystem->LogTypeDeprecatedBehavior_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        Type = "UDEFINED BEHAVIOR";
        if (!OpenGLLoggingSystem->LogTypeUndefinedBehavior_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        Type = "PORTABILITY";
        if (!OpenGLLoggingSystem->LogTypePortability_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        Type = "PERFORMANCE";
        if (!OpenGLLoggingSystem->LogTypePerformance_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_MARKER:
        Type = "MARKER";
        if (!OpenGLLoggingSystem->LogTypeMarker_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_PUSH_GROUP:
        Type = "PUSH_GROUP";
        if (!OpenGLLoggingSystem->LogTypePushGroup_){
            WillLog = false;
        }
        break;
    
        case GL_DEBUG_TYPE_POP_GROUP:
        Type = "POP_GROUP";
        if (!OpenGLLoggingSystem->LogTypePopGroup_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_TYPE_OTHER:
        Type = "OTHER";
        if (!OpenGLLoggingSystem->LogTypeOther_){
            WillLog = false;
        }
        break;


        default:
        Type = "UNKNOWN";
        break;
    }

    switch (GLSeverity) {
        case GL_DEBUG_SEVERITY_HIGH:
        Severity= "HIGH";
        LogLevel = 10;
        if (!OpenGLLoggingSystem->LogSeverityHigh_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        Severity= "MEDIUM";
        LogLevel = 9;
        if (!OpenGLLoggingSystem->LogSeverityMedium_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SEVERITY_LOW:
        Severity= "LOW";
        LogLevel = 8;
        if (!OpenGLLoggingSystem->LogSeverityLow_){
            WillLog = false;
        }
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        Severity= "NOTIFICATION";
        LogLevel = 5;
        if (!OpenGLLoggingSystem->LogSeverityNotification_){
            WillLog = false;
        }
        break;

        default:
        Severity= "UNKNOWN";
        LogLevel = 7;
        break;
    }


    // OVerride For System Info Data
    if (GLSeverity == GL_DEBUG_SEVERITY_NOTIFICATION && GLSource == GL_DEBUG_SOURCE_APPLICATION && GLType == GL_DEBUG_TYPE_OTHER) {
        WillLog = true;
    }

    // Generate Log Message
    if (WillLog) {
        std::string Message = "["+ std::to_string(GLID) + "] ["
        + std::string(Source) + "] [" + std::string(Type) + "] ["
        + std::string(Severity) + "] " + std::string(GLMessage);

        Logger->Log(Message, LogLevel);
    }

    
    // Create Struct
    ERS_OpenGLLogItem Item;
    Item.Message_ = std::string(GLMessage);
    Item.ID_ = GLID;
    Item.Severity_ = GLSeverity;
    Item.Source_ = GLSource;
    Item.Type_ = GLType;

    // Append To List
    std::unique_lock<std::mutex> Lock =  std::unique_lock<std::mutex>(OpenGLLoggingSystem->LogItemMutex_);
    OpenGLLoggingSystem->LogItems_.push_back(Item);

}


ERS_OpenGLLoggingSystem::ERS_OpenGLLoggingSystem(ERS_LoggingSystem* Logger) {

    Logger_ = Logger;

    Logger_->Log("Initializing OpenGL Log Collection Subsystem", 5);
    glDisable(GL_DEBUG_OUTPUT);

}

ERS_OpenGLLoggingSystem::~ERS_OpenGLLoggingSystem() {

    Logger_->Log("OpenGL Log Collection Subsystem Destructor Called", 6);

    // Turn Off Output As Callback Relies On PointerS Owned By This Class, Causing A Segfault If Not Disabled Here
    glDisable(GL_DEBUG_OUTPUT);

}

void ERS_OpenGLLoggingSystem::SetCollectionStatus(bool Status) {

    // If Enabled, Setup Logging
    if (Status) {
        Logger_->Log("(Re)Enabling OpenGL Debug Output", 5);
        glEnable(GL_DEBUG_OUTPUT);
        Logger_->Log("Registering OpenGL Message Callback", 4);
        UserParam_.Logger_ = (void*)Logger_;
        UserParam_.OpenGLLoggingSystem_ = (void*)this;
        
        UserParam_.LogSeverityHigh_ =               &LogSeverityHigh_;
        UserParam_.LogSeverityMedium_ =             &LogSeverityMedium_;
        UserParam_.LogSeverityLow_ =                &LogSeverityLow_;
        UserParam_.LogSeverityNotification_ =       &LogSeverityNotification_;

        UserParam_.LogTypeError_ =                  &LogTypeError_;
        UserParam_.LogTypeDeprecatedBehavior_ =     &LogTypeDeprecatedBehavior_;
        UserParam_.LogTypeUndefinedBehavior_ =      &LogTypeUndefinedBehavior_;
        UserParam_.LogTypePortability_ =            &LogTypePortability_;
        UserParam_.LogTypePerformance_ =            &LogTypePerformance_;
        UserParam_.LogTypeMarker_ =                 &LogTypeMarker_;
        UserParam_.LogTypePushGroup_ =              &LogTypePushGroup_;
        UserParam_.LogTypePopGroup_ =               &LogTypePopGroup_;
        UserParam_.LogTypeOther_ =                  &LogTypeOther_;

        UserParam_.LogSourceApi_ =                  &LogSourceApi_;
        UserParam_.LogSourceWindowSystem_ =         &LogSourceWindowSystem_;
        UserParam_.LogSourceShaderCompiler_ =       &LogSourceShaderCompiler_;
        UserParam_.LogSourceThirdParty_ =           &LogSourceThirdParty_;
        UserParam_.LogSourceApplication_ =          &LogSourceApplication_;
        UserParam_.LogSourceOther_ =                &LogSourceOther_;


        glDebugMessageCallback(ERS_MessageCallback, &UserParam_);
        Logger_->Log("Done Registering OpenGL Message Callback", 3);


        Logger_->Log("Inserting Test Message", 3);
        Logger_->Log("A Test Message Saying Debug Messages Should Be Produced (Below) Shortly, If It Isn't Debug Messages Are Not Working. In This Case, You Can Try Enabling A Debug OpenGL Context In The Config File", 6);
        std::string DebugMessage = "Debug Messages Are Now Enabled And Working Properly";
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, DebugMessage.size(), DebugMessage.c_str());


    } else {
        Logger_->Log("Disabling OpenGL Debug Output", 5);
        glDisable(GL_DEBUG_OUTPUT);
    }

}