//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_PythonInterpreterIntegration.h>


// yes this is bad i know, but idk how else to fix it
#include <PyBind11ModelClass.cpp>
#include <PyBind11SystemInfo.cpp>
#include <PyBind11DirectionalLightClass.cpp>
#include <PyBind11PointLightClass.cpp>
#include <PyBind11SpotLightClass.cpp>



ERS_CLASS_PythonInterpreterIntegration::ERS_CLASS_PythonInterpreterIntegration(ERS_LoggingSystem* Logger) {

    Logger_ = Logger;

    Logger_->Log("Initializing Python Interpreter Subsystem", 5);


    // Start Python Interpreter
    Logger_->Log("Starting Python Interpreter", 5);
    Guard_ = std::make_unique<pybind11::scoped_interpreter>();
    Logger_->Log("Finished Starting Python Interpreter", 4);
    
}

ERS_CLASS_PythonInterpreterIntegration::~ERS_CLASS_PythonInterpreterIntegration() {

    Logger_->Log("Shutting Down Python Interpreter Subsystem", 6);

}

bool ERS_CLASS_PythonInterpreterIntegration::ExecuteModelScript(std::string ScriptSource, ERS_STRUCT_Model* Model, std::vector<std::string>* ErrorMessageString) {


    // Inport The Model Module, Set System Info
    pybind11::module ModelModule = pybind11::module_::import("Model");
    SetSystemInfoData(&ModelModule);

    // Set System Parameters
    ModelModule.attr("ModelPosX") = Model->ModelPosition.x;
    ModelModule.attr("ModelPosY") = Model->ModelPosition.y;
    ModelModule.attr("ModelPosZ") = Model->ModelPosition.z;
    ModelModule.attr("ModelRotX") = Model->ModelRotation.x;
    ModelModule.attr("ModelRotY") = Model->ModelRotation.y;
    ModelModule.attr("ModelRotZ") = Model->ModelRotation.z;
    ModelModule.attr("ModelScaleX") = Model->ModelScale.x;
    ModelModule.attr("ModelScaleY") = Model->ModelScale.y;
    ModelModule.attr("ModelScaleZ") = Model->ModelScale.z;

    ModelModule.attr("ModelEnabled") = Model->Enabled;


    // Get Local Dict
    pybind11::dict Locals = ModelModule.attr("__dict__");

    // If No Message String Vec Provided, Run All At Once, Else Run Line By Line
    if (ErrorMessageString == nullptr) {
        try {
            pybind11::exec(ScriptSource, pybind11::globals(), Locals);
        } catch (pybind11::value_error const&) {
            return false;
        } catch (pybind11::key_error const&) {
            return false;
        } catch (pybind11::reference_cast_error const&) {
            return false;
        } catch (pybind11::attribute_error const&) {
            return false;
        } catch (pybind11::import_error const&) {
            return false;
        } catch (pybind11::buffer_error const&) {
            return false;
        } catch (pybind11::index_error const&) {
            return false;
        } catch (pybind11::type_error const&) {
            return false;
        } catch (pybind11::cast_error const&) {
            return false;
        } catch (pybind11::error_already_set &Exception) {
            return false;
        }

    } else {
        ErrorMessageString->erase(ErrorMessageString->begin(), ErrorMessageString->end());
        std::string Line;
        std::stringstream StringStream(ScriptSource);
        unsigned long i = 0;

        while (getline(StringStream, Line, '\n')) {
            
            i++;
            try {
                pybind11::exec(Line, pybind11::globals(), Locals);
            } catch (pybind11::value_error const&) {
                ErrorHandle(ErrorMessageString, i, "ValueError");
            } catch (pybind11::key_error const&) {
                ErrorHandle(ErrorMessageString, i, "KeyError");
            } catch (pybind11::reference_cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "ReferenceCastError");
            } catch (pybind11::attribute_error const&) {
                ErrorHandle(ErrorMessageString, i, "AttributeError");
            } catch (pybind11::import_error const&) {
                ErrorHandle(ErrorMessageString, i, "ImportError");
            } catch (pybind11::buffer_error const&) {
                ErrorHandle(ErrorMessageString, i, "BufferError");
            } catch (pybind11::index_error const&) {
                ErrorHandle(ErrorMessageString, i, "IndexError");
            } catch (pybind11::type_error const&) {
                ErrorHandle(ErrorMessageString, i, "TypeError");
            } catch (pybind11::cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "CastError");
            } catch (pybind11::error_already_set &Exception) {
                ErrorHandle(ErrorMessageString, i, Exception.what());
            }

        }

    }

    // Write Back Model Data
    double ModelPosX, ModelPosY, ModelPosZ;
    double ModelRotX, ModelRotY, ModelRotZ;
    double ModelScaleX, ModelScaleY, ModelScaleZ;
    bool Successful = true;

    try {
        ModelPosX = ModelModule.attr("ModelPosX").cast<double>();
        ModelPosY = ModelModule.attr("ModelPosY").cast<double>();
        ModelPosZ = ModelModule.attr("ModelPosZ").cast<double>();
        Model->SetPosition(glm::vec3(ModelPosX, ModelPosY, ModelPosZ));
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("Model Position CAST_ERROR");
        Successful = false;
    }
    try {
        ModelRotX = ModelModule.attr("ModelRotX").cast<double>();
        ModelRotY = ModelModule.attr("ModelRotY").cast<double>();
        ModelRotZ = ModelModule.attr("ModelRotZ").cast<double>();
        Model->SetRotation(glm::vec3(ModelRotX, ModelRotY, ModelRotZ));
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("Model Rotation CAST_ERROR");
        Successful = false;
    }
    try {
        ModelScaleX = ModelModule.attr("ModelScaleX").cast<double>();
        ModelScaleY = ModelModule.attr("ModelScaleY").cast<double>();
        ModelScaleZ = ModelModule.attr("ModelScaleZ").cast<double>();
        Model->SetScale(glm::vec3(ModelScaleX, ModelScaleY, ModelScaleZ));
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("Model Scale CAST_ERROR");
        Successful = false;
    }

    try {
        Model->Enabled = ModelModule.attr("ModelEnabled").cast<bool>(); 
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("Model Enable CAST_ERROR");
        Successful = false;
    }


    if (Successful) {
        Model->ApplyTransformations();
    }
    


    // Return Status
    return true;
    

}

bool ERS_CLASS_PythonInterpreterIntegration::ExecutePointLightScript(std::string ScriptSource, ERS_STRUCT_PointLight* PointLight, std::vector<std::string>* ErrorMessageString) {



    // Inport The PointLight Module, Set System Info
    pybind11::module PointLightModule = pybind11::module_::import("PointLight");
    SetSystemInfoData(&PointLightModule);

    // Set System Parameters
    PointLightModule.attr("PointLightPosX") = PointLight->Pos.x;
    PointLightModule.attr("PointLightPosY") = PointLight->Pos.y;
    PointLightModule.attr("PointLightPosZ") = PointLight->Pos.z;

    PointLightModule.attr("PointLightColorR") = PointLight->Color.r;
    PointLightModule.attr("PointLightColorG") = PointLight->Color.g;
    PointLightModule.attr("PointLightColorB") = PointLight->Color.b;
    
    PointLightModule.attr("PointLightIntensity") = PointLight->Intensity;
    PointLightModule.attr("PointLightMaxDistance") = PointLight->MaxDistance;
    

    // Get Local Dict
    pybind11::dict Locals = PointLightModule.attr("__dict__");

    // If No Message String Vec Provided, Run All At Once, Else Run Line By Line
    if (ErrorMessageString == nullptr) {
        try {
            pybind11::exec(ScriptSource, pybind11::globals(), Locals);
        } catch (pybind11::value_error const&) {
            return false;
        } catch (pybind11::key_error const&) {
            return false;
        } catch (pybind11::reference_cast_error const&) {
            return false;
        } catch (pybind11::attribute_error const&) {
            return false;
        } catch (pybind11::import_error const&) {
            return false;
        } catch (pybind11::buffer_error const&) {
            return false;
        } catch (pybind11::index_error const&) {
            return false;
        } catch (pybind11::type_error const&) {
            return false;
        } catch (pybind11::cast_error const&) {
            return false;
        } catch (pybind11::error_already_set &Exception) {
            return false;
        }

    } else {
        ErrorMessageString->erase(ErrorMessageString->begin(), ErrorMessageString->end());
        std::string Line;
        std::stringstream StringStream(ScriptSource);
        unsigned long i = 0;

        while (getline(StringStream, Line, '\n')) {
            
            i++;
            try {
                pybind11::exec(Line, pybind11::globals(), Locals);
            } catch (pybind11::value_error const&) {
                ErrorHandle(ErrorMessageString, i, "ValueError");
            } catch (pybind11::key_error const&) {
                ErrorHandle(ErrorMessageString, i, "KeyError");
            } catch (pybind11::reference_cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "ReferenceCastError");
            } catch (pybind11::attribute_error const&) {
                ErrorHandle(ErrorMessageString, i, "AttributeError");
            } catch (pybind11::import_error const&) {
                ErrorHandle(ErrorMessageString, i, "ImportError");
            } catch (pybind11::buffer_error const&) {
                ErrorHandle(ErrorMessageString, i, "BufferError");
            } catch (pybind11::index_error const&) {
                ErrorHandle(ErrorMessageString, i, "IndexError");
            } catch (pybind11::type_error const&) {
                ErrorHandle(ErrorMessageString, i, "TypeError");
            } catch (pybind11::cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "CastError");
            } catch (pybind11::error_already_set &Exception) {
                ErrorHandle(ErrorMessageString, i, Exception.what());
            }

        }

    }

    // Write Back PointLight Data
    double PointLightPosX, PointLightPosY, PointLightPosZ;
    float ColorR, ColorG, ColorB;

    try {
        PointLightPosX = PointLightModule.attr("PointLightPosX").cast<double>();
        PointLightPosY = PointLightModule.attr("PointLightPosY").cast<double>();
        PointLightPosZ = PointLightModule.attr("PointLightPosZ").cast<double>();
        PointLight->Pos = glm::vec3(PointLightPosX, PointLightPosY, PointLightPosZ);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("PointLight Position CAST_ERROR");
    }

    try {
        ColorR = PointLightModule.attr("PointLightColorR").cast<double>();
        ColorG = PointLightModule.attr("PointLightColorG").cast<double>();
        ColorB = PointLightModule.attr("PointLightColorB").cast<double>();
        PointLight->Color = glm::vec3(ColorR, ColorG, ColorB);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("PointLight Color CAST_ERROR");
    }

    try {
        PointLight->Intensity = PointLightModule.attr("PointLightIntensity").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("PointLight Intensity CAST_ERROR");
    }

    try {
        PointLight->Intensity = PointLightModule.attr("PointLightMaxDistance").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("PointLight MaxDistance CAST_ERROR");
    }


    // Return Status
    return true;
    

}

bool ERS_CLASS_PythonInterpreterIntegration::ExecuteDirectionalLightScript(std::string ScriptSource, ERS_STRUCT_DirectionalLight* DirectionalLight, std::vector<std::string>* ErrorMessageString) {



    // Inport The DirectionalLight Module, Set System Info
    pybind11::module DirectionalLightModule = pybind11::module_::import("DirectionalLight");
    SetSystemInfoData(&DirectionalLightModule);

    // Set System Parameters
    DirectionalLightModule.attr("DirectionalLightPosX") = DirectionalLight->Pos.x;
    DirectionalLightModule.attr("DirectionalLightPosY") = DirectionalLight->Pos.y;
    DirectionalLightModule.attr("DirectionalLightPosZ") = DirectionalLight->Pos.z;

    DirectionalLightModule.attr("DirectionalLightRotX") = DirectionalLight->Rot.x;
    DirectionalLightModule.attr("DirectionalLightRotY") = DirectionalLight->Rot.y;
    DirectionalLightModule.attr("DirectionalLightRotZ") = DirectionalLight->Rot.z;

    DirectionalLightModule.attr("DirectionalLightColorR") = DirectionalLight->Color.r;
    DirectionalLightModule.attr("DirectionalLightColorG") = DirectionalLight->Color.g;
    DirectionalLightModule.attr("DirectionalLightColorB") = DirectionalLight->Color.b;

    DirectionalLightModule.attr("DirectionalLightIntensity") = DirectionalLight->Intensity;    
    DirectionalLightModule.attr("DirectionalLightMaxDistance") = DirectionalLight->MaxDistance;   

    // Get Local Dict
    pybind11::dict Locals = DirectionalLightModule.attr("__dict__");

    // If No Message String Vec Provided, Run All At Once, Else Run Line By Line
    if (ErrorMessageString == nullptr) {
        try {
            pybind11::exec(ScriptSource, pybind11::globals(), Locals);
        } catch (pybind11::value_error const&) {
            return false;
        } catch (pybind11::key_error const&) {
            return false;
        } catch (pybind11::reference_cast_error const&) {
            return false;
        } catch (pybind11::attribute_error const&) {
            return false;
        } catch (pybind11::import_error const&) {
            return false;
        } catch (pybind11::buffer_error const&) {
            return false;
        } catch (pybind11::index_error const&) {
            return false;
        } catch (pybind11::type_error const&) {
            return false;
        } catch (pybind11::cast_error const&) {
            return false;
        } catch (pybind11::error_already_set &Exception) {
            return false;
        }

    } else {
        ErrorMessageString->erase(ErrorMessageString->begin(), ErrorMessageString->end());
        std::string Line;
        std::stringstream StringStream(ScriptSource);
        unsigned long i = 0;

        while (getline(StringStream, Line, '\n')) {
            
            i++;
            try {
                pybind11::exec(Line, pybind11::globals(), Locals);
            } catch (pybind11::value_error const&) {
                ErrorHandle(ErrorMessageString, i, "ValueError");
            } catch (pybind11::key_error const&) {
                ErrorHandle(ErrorMessageString, i, "KeyError");
            } catch (pybind11::reference_cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "ReferenceCastError");
            } catch (pybind11::attribute_error const&) {
                ErrorHandle(ErrorMessageString, i, "AttributeError");
            } catch (pybind11::import_error const&) {
                ErrorHandle(ErrorMessageString, i, "ImportError");
            } catch (pybind11::buffer_error const&) {
                ErrorHandle(ErrorMessageString, i, "BufferError");
            } catch (pybind11::index_error const&) {
                ErrorHandle(ErrorMessageString, i, "IndexError");
            } catch (pybind11::type_error const&) {
                ErrorHandle(ErrorMessageString, i, "TypeError");
            } catch (pybind11::cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "CastError");
            } catch (pybind11::error_already_set &Exception) {
                ErrorHandle(ErrorMessageString, i, Exception.what());
            }

        }

    }

    // Write Back DirectionalLight Data
    double DirectionalLightPosX, DirectionalLightPosY, DirectionalLightPosZ;
    double DirectionalLightRotX, DirectionalLightRotY, DirectionalLightRotZ;
    float ColorR, ColorG, ColorB;

    try {
        DirectionalLightPosX = DirectionalLightModule.attr("DirectionalLightPosX").cast<double>();
        DirectionalLightPosY = DirectionalLightModule.attr("DirectionalLightPosY").cast<double>();
        DirectionalLightPosZ = DirectionalLightModule.attr("DirectionalLightPosZ").cast<double>();
        DirectionalLight->Pos = glm::vec3(DirectionalLightPosX, DirectionalLightPosY, DirectionalLightPosZ);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("DirectionalLight Position CAST_ERROR");
    }

    try {
        DirectionalLightRotX = DirectionalLightModule.attr("DirectionalLightRotX").cast<double>();
        DirectionalLightRotY = DirectionalLightModule.attr("DirectionalLightRotY").cast<double>();
        DirectionalLightRotZ = DirectionalLightModule.attr("DirectionalLightRotZ").cast<double>();
        DirectionalLight->Rot = glm::vec3(DirectionalLightRotX, DirectionalLightRotY, DirectionalLightRotZ);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("DirectionalLight Rotation CAST_ERROR");
    }

    try {
        ColorR = DirectionalLightModule.attr("DirectionalLightColorR").cast<double>();
        ColorG = DirectionalLightModule.attr("DirectionalLightColorG").cast<double>();
        ColorB = DirectionalLightModule.attr("DirectionalLightColorB").cast<double>();
        DirectionalLight->Color = glm::vec3(ColorR, ColorG, ColorB);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("DirectionalLight Color CAST_ERROR");
    }


    try {
        DirectionalLight->Intensity = DirectionalLightModule.attr("DirectionalLightIntensity").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("DirectionalLight Intensity CAST_ERROR");
    }

    try {
        DirectionalLight->Intensity = DirectionalLightModule.attr("DirectionalLightMaxDistance").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("DirectionalLight MaxDistance CAST_ERROR");
    }



    // Return Status
    return true;
    
}

bool ERS_CLASS_PythonInterpreterIntegration::ExecuteSpotLightScript(std::string ScriptSource, ERS_STRUCT_SpotLight* SpotLight, std::vector<std::string>* ErrorMessageString) {



    // Inport The SpotLight Module, Set System Info
    pybind11::module SpotLightModule = pybind11::module_::import("SpotLight");
    SetSystemInfoData(&SpotLightModule);

    // Set System Parameters
    SpotLightModule.attr("SpotLightPosX") = SpotLight->Pos.x;
    SpotLightModule.attr("SpotLightPosY") = SpotLight->Pos.y;
    SpotLightModule.attr("SpotLightPosZ") = SpotLight->Pos.z;

    SpotLightModule.attr("SpotLightRotX") = SpotLight->Rot.x;
    SpotLightModule.attr("SpotLightRotY") = SpotLight->Rot.y;
    SpotLightModule.attr("SpotLightRotZ") = SpotLight->Rot.z;

    SpotLightModule.attr("SpotLightColorR") = SpotLight->Color.r;
    SpotLightModule.attr("SpotLightColorG") = SpotLight->Color.g;
    SpotLightModule.attr("SpotLightColorB") = SpotLight->Color.b;
    
    SpotLightModule.attr("SpotLightIntensity") = SpotLight->Intensity;
    SpotLightModule.attr("SpotLightMaxDistance") = SpotLight->MaxDistance;
    
    SpotLightModule.attr("SpotLightCutoff") = SpotLight->CutOff;
    SpotLightModule.attr("SpotLightRolloff") = SpotLight->Rolloff;
 

    // Get Local Dict
    pybind11::dict Locals = SpotLightModule.attr("__dict__");

    // If No Message String Vec Provided, Run All At Once, Else Run Line By Line
    if (ErrorMessageString == nullptr) {
        try {
            pybind11::exec(ScriptSource, pybind11::globals(), Locals);
        } catch (pybind11::value_error const&) {
            return false;
        } catch (pybind11::key_error const&) {
            return false;
        } catch (pybind11::reference_cast_error const&) {
            return false;
        } catch (pybind11::attribute_error const&) {
            return false;
        } catch (pybind11::import_error const&) {
            return false;
        } catch (pybind11::buffer_error const&) {
            return false;
        } catch (pybind11::index_error const&) {
            return false;
        } catch (pybind11::type_error const&) {
            return false;
        } catch (pybind11::cast_error const&) {
            return false;
        } catch (pybind11::error_already_set &Exception) {
            return false;
        }

    } else {
        ErrorMessageString->erase(ErrorMessageString->begin(), ErrorMessageString->end());
        std::string Line;
        std::stringstream StringStream(ScriptSource);
        unsigned long i = 0;

        while (getline(StringStream, Line, '\n')) {
            
            i++;
            try {
                pybind11::exec(Line, pybind11::globals(), Locals);
            } catch (pybind11::value_error const&) {
                ErrorHandle(ErrorMessageString, i, "ValueError");
            } catch (pybind11::key_error const&) {
                ErrorHandle(ErrorMessageString, i, "KeyError");
            } catch (pybind11::reference_cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "ReferenceCastError");
            } catch (pybind11::attribute_error const&) {
                ErrorHandle(ErrorMessageString, i, "AttributeError");
            } catch (pybind11::import_error const&) {
                ErrorHandle(ErrorMessageString, i, "ImportError");
            } catch (pybind11::buffer_error const&) {
                ErrorHandle(ErrorMessageString, i, "BufferError");
            } catch (pybind11::index_error const&) {
                ErrorHandle(ErrorMessageString, i, "IndexError");
            } catch (pybind11::type_error const&) {
                ErrorHandle(ErrorMessageString, i, "TypeError");
            } catch (pybind11::cast_error const&) {
                ErrorHandle(ErrorMessageString, i, "CastError");
            } catch (pybind11::error_already_set &Exception) {
                ErrorHandle(ErrorMessageString, i, Exception.what());
            }

        }

    }

    // Write Back SpotLight Data
    double SpotLightPosX, SpotLightPosY, SpotLightPosZ;
    double SpotLightRotX, SpotLightRotY, SpotLightRotZ;
    float ColorR, ColorG, ColorB;

    try {
        SpotLightPosX = SpotLightModule.attr("SpotLightPosX").cast<double>();
        SpotLightPosY = SpotLightModule.attr("SpotLightPosY").cast<double>();
        SpotLightPosZ = SpotLightModule.attr("SpotLightPosZ").cast<double>();
        SpotLight->Pos = glm::vec3(SpotLightPosX, SpotLightPosY, SpotLightPosZ);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Position CAST_ERROR");
    }

    try {
        SpotLightRotX = SpotLightModule.attr("SpotLightRotX").cast<double>();
        SpotLightRotY = SpotLightModule.attr("SpotLightRotY").cast<double>();
        SpotLightRotZ = SpotLightModule.attr("SpotLightRotZ").cast<double>();
        SpotLight->Rot = glm::vec3(SpotLightRotX, SpotLightRotY, SpotLightRotZ);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Rotation CAST_ERROR");
    }

    try {
        ColorR = SpotLightModule.attr("SpotLightColorR").cast<double>();
        ColorG = SpotLightModule.attr("SpotLightColorG").cast<double>();
        ColorB = SpotLightModule.attr("SpotLightColorB").cast<double>();
        SpotLight->Color = glm::vec3(ColorR, ColorG, ColorB);
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Color CAST_ERROR");
    }

    try {
        SpotLight->Intensity = SpotLightModule.attr("SpotLightIntensity").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Intensity CAST_ERROR");
    }


    try {
        SpotLight->Rolloff = SpotLightModule.attr("SpotLightRolloff").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Rolloff CAST_ERROR");
    }



    try {
        SpotLight->CutOff = SpotLightModule.attr("SpotLightCutoff").cast<float>();
        SpotLight->Rolloff = SpotLightModule.attr("SpotLightRolloff").cast<float>();
    } catch (pybind11::cast_error const&) {
        ErrorMessageString->push_back("SpotLight Cutoff CAST_ERROR");
    }


    // Return Status
    return true;
    
}

void ERS_CLASS_PythonInterpreterIntegration::ErrorHandle(std::vector<std::string>* Target, unsigned long LineNumber, std::string Error) {

    std::string ErrorMessage = std::string("Error On Line '") + std::to_string(LineNumber) + std::string("': ") + Error + std::string("\n");
    Target->push_back(ErrorMessage);


}

void ERS_CLASS_PythonInterpreterIntegration::UpdateSystemInfoData(double RunTime) {

    RunTime_ = RunTime;

}

void ERS_CLASS_PythonInterpreterIntegration::SetSystemInfoData(pybind11::module* Locals) {

    // Set System Info Module
    pybind11::module SystemInfo = pybind11::module_::import("SystemInfo");
    Locals->attr("GameTime") = pybind11::float_(RunTime_);

    auto Clock = std::chrono::system_clock::now();
    double UnixEpoch = std::chrono::duration_cast<std::chrono::seconds>(Clock.time_since_epoch()).count();
    Locals->attr("SystemTime") = UnixEpoch;

}