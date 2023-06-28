#include <Util/JSONHelpers.h>


namespace BG {
namespace NES {
namespace Util {

int GetInt(nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON)[_ParamName].template get<int>();
}

float GetFloat(nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON)[_ParamName].template get<float>();
}

std::string GetString(nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON)[_ParamName].template get<std::string>();
}


void SetVec3(nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units) {
    nlohmann::json Vec3 = nlohmann::json::parse(_Input);
    (*_Target)[_Prefix + "X_" + _Units] = Vec3[0].template get<float>();
    (*_Target)[_Prefix + "Y_" + _Units] = Vec3[1].template get<float>();
    (*_Target)[_Prefix + "Z_" + _Units] = Vec3[2].template get<float>();
}

void SetVec3(float* _Vector, std::string _Input) {
    nlohmann::json Vec3 = nlohmann::json::parse(_Input);
    _Vector[0] = Vec3[0].template get<float>();
    _Vector[1] = Vec3[1].template get<float>();
    _Vector[2] = Vec3[2].template get<float>();
}

void GetVec3(float* _Vector, nlohmann::json* _Input, std::string _Prefix, std::string _Units) {
    _Vector[0] = Util::GetFloat(_Input, _Prefix + "X_" + _Units);
    _Vector[1] = Util::GetFloat(_Input, _Prefix + "Y_" + _Units);
    _Vector[2] = Util::GetFloat(_Input, _Prefix + "Z_" + _Units);
}


}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG