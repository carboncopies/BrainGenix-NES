//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#include <ERS_STRUCT_Shader.h>


ERS_STRUCT_Shader::ERS_STRUCT_Shader() {

    ShaderProgram_ = 0;
    VertexShader = 0;
    FragmentShader = 0;
    GeometryShader = 0;
    ComputeShader = 0;
    TCShader = 0;
    TEShader = 0;

    HasGeometryShader = false;
    HasComputeShader = false;
    HasTCShader = false;
    HasTEShader = false;

    VertexID = -1;
    FragmentID = -1;
    GeometryID = -1;
    ComputeID = -1;
    TessellationControlShaderID = -1;
    TessellationEvaluationShaderID = -1;
}

ERS_STRUCT_Shader::~ERS_STRUCT_Shader() {

    // Deallocate Shader Program
    glUseProgram(0);
    glDeleteProgram(ShaderProgram_);

}

void ERS_STRUCT_Shader::ResetProgram() {

    glDeleteProgram(ShaderProgram_);

}

std::string ERS_STRUCT_Shader::CompileVertexShader(const char* VertexText, ERS_LoggingSystem* Logger) {

    // Compile The Vertex Shader Text Into A Binary
    std::string ErrorMessage;
    VertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(VertexShader, 1, &VertexText, NULL);
    glCompileShader(VertexShader);

    // Report Compilation Status
    int VertexSuccess;
    char VertexInfoLog[65535];
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &VertexSuccess);
    if (!VertexSuccess) {
        glGetShaderInfoLog(VertexShader, 65535, NULL, VertexInfoLog);
        ErrorMessage = std::string(VertexInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Vertex Shader Compile Error: " +  std::string(VertexInfoLog), 8);
        }
    }


    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CompileFragmentShader(const char* FragmentText, ERS_LoggingSystem* Logger) {

    // Compile The Fragment Shader Text Into A Binary
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(FragmentShader);

    // Report Compilation Status
    int FragmentSuccess;
    char FragmentInfoLog[65535];
    std::string ErrorMessage;
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &FragmentSuccess);
    if (!FragmentSuccess) {
        glGetShaderInfoLog(FragmentShader, 65535, NULL, FragmentInfoLog);
        ErrorMessage = std::string(FragmentInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Fragment Shader Compile Error: " +  std::string(FragmentInfoLog), 8);
        }
    }

    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CompileGeometryShader(const char* GeometryText, ERS_LoggingSystem* Logger) {

    // Compile The Fragment Shader Text Into A Binary
    GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    glShaderSource(GeometryShader, 1, &GeometryText, NULL);
    glCompileShader(GeometryShader);

    // Report Compilation Status
    int FragmentSuccess;
    char FragmentInfoLog[65535];
    std::string ErrorMessage;
    glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &FragmentSuccess);
    if (!FragmentSuccess) {
        glGetShaderInfoLog(GeometryShader, 65535, NULL, FragmentInfoLog);
        ErrorMessage = std::string(FragmentInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Geometry Shader Compile Error: " +  std::string(FragmentInfoLog), 8);
        }
    } else {
        HasGeometryShader = true;
    }

    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CompileComputeShader(const char* ComputeText, ERS_LoggingSystem* Logger) {

    // Compile The Fragment Shader Text Into A Binary
    ComputeShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(ComputeShader, 1, &ComputeText, NULL);
    glCompileShader(ComputeShader);

    // Report Compilation Status
    int FragmentSuccess;
    char FragmentInfoLog[65535];
    std::string ErrorMessage;
    glGetShaderiv(ComputeShader, GL_COMPILE_STATUS, &FragmentSuccess);
    if (!FragmentSuccess) {
        glGetShaderInfoLog(ComputeShader, 65535, NULL, FragmentInfoLog);
        ErrorMessage = std::string(FragmentInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Compute Shader Compile Error: " +  std::string(FragmentInfoLog), 8);
        }
    } else {
        HasComputeShader = true;
    }

    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CompileTCShader(const char* TCText, ERS_LoggingSystem* Logger) {

    // Compile The Fragment Shader Text Into A Binary
    TCShader = glCreateShader(GL_TESS_CONTROL_SHADER);

    glShaderSource(TCShader, 1, &TCText, NULL);
    glCompileShader(TCShader);

    // Report Compilation Status
    int FragmentSuccess;
    char FragmentInfoLog[65535];
    std::string ErrorMessage;
    glGetShaderiv(TCShader, GL_COMPILE_STATUS, &FragmentSuccess);
    if (!FragmentSuccess) {
        glGetShaderInfoLog(TCShader, 65535, NULL, FragmentInfoLog);
        ErrorMessage = std::string(FragmentInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Tess Control Shader Compile Error: " +  std::string(FragmentInfoLog), 8);
        }
    } else {
        HasTCShader = true;
    }

    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CompileTEShader(const char* TEText, ERS_LoggingSystem* Logger) {

    // Compile The Fragment Shader Text Into A Binary
    TEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);

    glShaderSource(TEShader, 1, &TEText, NULL);
    glCompileShader(TEShader);

    // Report Compilation Status
    int FragmentSuccess;
    char FragmentInfoLog[65535];
    std::string ErrorMessage;
    glGetShaderiv(TEShader, GL_COMPILE_STATUS, &FragmentSuccess);
    if (!FragmentSuccess) {
        glGetShaderInfoLog(TEShader, 65535, NULL, FragmentInfoLog);
        ErrorMessage = std::string(FragmentInfoLog);
        if (Logger != nullptr) {
            Logger->Log("Tess Eval Shader Compile Error: " +  std::string(FragmentInfoLog), 8);
        }
    } else {
        HasTEShader = true;
    }

    return ErrorMessage;

}

std::string ERS_STRUCT_Shader::CreateShaderProgram(ERS_LoggingSystem* Logger, bool LogEnable) {

    // Create Shader Program
    ShaderProgram_ = glCreateProgram();

    // Attach Shaders To Program
    Logger->Log("Attaching Vertex Shader", 3, LogEnable);
    glAttachShader(ShaderProgram_, VertexShader);

    Logger->Log("Attaching Fragment Shader", 3, LogEnable);
    glAttachShader(ShaderProgram_, FragmentShader);

    if (HasGeometryShader) {
        Logger->Log("Attaching Geometry Shader", 3, LogEnable);
        glAttachShader(ShaderProgram_, GeometryShader);
    }

    if (HasComputeShader) {
        Logger->Log("Attaching Compute Shader", 3, LogEnable);
        glAttachShader(ShaderProgram_, ComputeShader);
    }

    if (HasTCShader) {
        Logger->Log("Attaching TC Shader", 3, LogEnable);
        glAttachShader(ShaderProgram_, TCShader);
    }

    if (HasTEShader) {
        Logger->Log("Attaching TE Shader", 3, LogEnable);
        glAttachShader(ShaderProgram_, TEShader);
    }

    // Link Program
    glLinkProgram(ShaderProgram_);


    // Get Link Status
    int Success;
    std::string ErrorMessage;
    glGetProgramiv(ShaderProgram_, GL_LINK_STATUS, &Success);
    if (!Success) {
        char InfoLog[65535];
        glGetProgramInfoLog(ShaderProgram_, 65535, NULL, InfoLog);
        ErrorMessage = std::string(InfoLog);
        Logger->Log("Shader Link Error: " +  std::string(InfoLog), 8, LogEnable);
    }

    // Free RAM
    glDetachShader(ShaderProgram_, VertexShader);
    glDeleteShader(VertexShader);

    glDetachShader(ShaderProgram_, FragmentShader);
    glDeleteShader(FragmentShader);

    if (HasGeometryShader) {
        glDetachShader(ShaderProgram_, GeometryShader);
        glDeleteShader(GeometryShader);
    }

    if (HasComputeShader) {
        glDetachShader(ShaderProgram_, ComputeShader);
        glDeleteShader(ComputeShader);
    }

    if (HasTCShader) {
        glDetachShader(ShaderProgram_, TCShader);
        glDeleteShader(TCShader);
    }

    if (HasTEShader) {
        glDetachShader(ShaderProgram_, TEShader);
        glDeleteShader(TEShader);
    }


    // Return Status
    return ErrorMessage;

}

bool ERS_STRUCT_Shader::MakeActive() {

    glUseProgram(ShaderProgram_);
    return true;

}

// Population Functions
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ShaderProgram_, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ShaderProgram_, name.c_str()), value); 
}
void ERS_STRUCT_Shader::SetFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ShaderProgram_, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, &value[0]); 
}
void ERS_STRUCT_Shader::SetVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(ShaderProgram_, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, &value[0]); 
}
void ERS_STRUCT_Shader::SetVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(ShaderProgram_, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, &value[0]); 
}
void ERS_STRUCT_Shader::SetVec4(const std::string &name, float x, float y, float z, float w) 
{ 
    glUniform4f(glGetUniformLocation(ShaderProgram_, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ERS_STRUCT_Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgram_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
