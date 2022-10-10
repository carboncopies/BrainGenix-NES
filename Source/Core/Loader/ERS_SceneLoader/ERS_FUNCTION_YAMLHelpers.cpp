//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_YAMLHelpers.h>

// Standart Type Helper Functions
bool ERS_FUNCTION_GetInt(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, int &Target) {
    try {
        Target = Data[Name].as<int>();
        return true;
    } catch (YAML::TypedBadConversion<int>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'int'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetFloat(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, float &Target) {
    try {
        Target = Data[Name].as<float>();
        return true;
    } catch (YAML::TypedBadConversion<float>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'float'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetBool(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, bool &Target) {
    try {
        Target = Data[Name].as<bool>();
        return true;
    } catch (YAML::TypedBadConversion<bool>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'bool'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetLong(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, long &Target) {
    try {
        Target = Data[Name].as<long>();
        return true;
    } catch (YAML::TypedBadConversion<long>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'long'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetDouble(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, double &Target) {
    try {
        Target = Data[Name].as<double>();
        return true;
    } catch (YAML::TypedBadConversion<double>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'double'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetUnsignedInt(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, unsigned int &Target) {
    try {
        Target = Data[Name].as<unsigned int>();
        return true;
    } catch (YAML::TypedBadConversion<unsigned int>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'unsigned int'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetBool(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, unsigned long &Target) {
    try {
        Target = Data[Name].as<unsigned long>();
        return true;
    } catch (YAML::TypedBadConversion<unsigned long>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'unsigned long'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetLongLong(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, long long &Target) {
    try {
        Target = Data[Name].as<long long>();
        return true;
    } catch (YAML::TypedBadConversion<long long>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'long long'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetUnsignedLongLong(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, unsigned long long &Target) {
    try {
        Target = Data[Name].as<unsigned long long>();
        return true;
    } catch (YAML::TypedBadConversion<unsigned long long>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'unsigned long long'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetString(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, std::string &Target) {
    try {
        Target = Data[Name].as<std::string>();
        return true;
    } catch (YAML::TypedBadConversion<std::string>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'std::string'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetNode(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, YAML::Node &Target) {
    try {
        Target = Data[Name];
        return true;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}


// Vector Helpers
bool ERS_FUNCTION_GetStringVector(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, std::vector<std::string> &Target) {
    try {
        YAML::Node TargetNode = Data[Name];
        for (YAML::const_iterator it=TargetNode.begin(); it!=TargetNode.end(); ++it) {
            Target.push_back(it->second.as<std::string>());
        }
        return true;
    } catch (YAML::TypedBadConversion<std::string>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'std::string'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetIntVector(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, std::vector<int> &Target) {
    try {
        YAML::Node TargetNode = Data[Name];
        for (YAML::const_iterator it=TargetNode.begin(); it!=TargetNode.end(); ++it) {
            Target.push_back(it->second.as<int>());
        }
        return true;
    } catch (YAML::TypedBadConversion<int>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'int'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetLongVector(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, std::vector<long> &Target) {
    try {
        YAML::Node TargetNode = Data[Name];
        for (YAML::const_iterator it=TargetNode.begin(); it!=TargetNode.end(); ++it) {
            Target.push_back(it->second.as<long>());
        }
        return true;
    } catch (YAML::TypedBadConversion<long>&) {
        Logger->Log(std::string("Failed To Cast Parameter '") + Name + "' To Type 'long'", 8);
        return false;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}
bool ERS_FUNCTION_GetNodeVector(ERS_LoggingSystem* Logger, YAML::Node Data, std::string Name, std::vector<YAML::Node> &Target) {
    try {
        YAML::Node TargetNode = Data[Name];
        for (YAML::const_iterator it=TargetNode.begin(); it!=TargetNode.end(); ++it) {
            Target.push_back(it->second);
        }
        return true;
    } catch (YAML::KeyNotFound&) {
        Logger->Log(std::string("Failed To Find Parameter '") + Name + "'", 7);
        return false;
    }
}


// GLM Helper Functions
bool ERS_FUNCTION_GetVec3(ERS_LoggingSystem* Logger, YAML::Node Data, std::string NameBase, glm::vec3 &Target) {

    float X,Y,Z = 0.0f;
    bool Status = true;

    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "X", X);
    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "Y", Y);
    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "Z", Z);

    Target = glm::vec3(X,Y,Z);
    return Status;

}
bool ERS_FUNCTION_GetVec3Color(ERS_LoggingSystem* Logger, YAML::Node Data, std::string NameBase, glm::vec3 &Target) {
    float X,Y,Z = 0.0f;
    bool Status = true;

    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "Red", X);
    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "Green", Y);
    Status &= ERS_FUNCTION_GetFloat(Logger, Data, NameBase + "Blue", Z);
    
    Target = glm::vec3(X,Y,Z);
    return Status;
}
