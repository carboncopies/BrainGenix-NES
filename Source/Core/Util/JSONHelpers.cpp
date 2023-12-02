#include <Util/JSONHelpers.h>

namespace BG {
namespace NES {
namespace Util {

int GetInt(nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON)[_ParamName].template get<int>();
}

float GetFloat(nlohmann::json *_JSON, std::string _ParamName) {
    try {
        return (*_JSON)[_ParamName].template get<float>();
    } catch (nlohmann::json::type_error &Ex) {
        std::cerr<<"Cannot Get JSON Param: "<<_ParamName<<std::endl;
        std::cerr<<"ERR: JSON Float Type Error: "<<Ex.what()<<std::endl;
        return -1;
    } catch (nlohmann::json::parse_error &Ex) {
        std::cerr<<"Cannot Get JSON Param: "<<_ParamName<<std::endl;
        std::cerr<<"ERR: JSON Float Parsing Error: "<<Ex.what()<<std::endl;
        return -1;
    }
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

void SetVec3(Simulator::Geometries::Vec3D& _Vector, std::string _Input) {
    nlohmann::json Vec3 = nlohmann::json::parse(_Input);
    _Vector.x = Vec3[0].template get<float>();
    _Vector.y = Vec3[1].template get<float>();
    _Vector.z = Vec3[2].template get<float>();
}

void GetVec3(Simulator::Geometries::Vec3D& _Vector, nlohmann::json* _Input, std::string _Prefix, std::string _Units) {
    _Vector.x = Util::GetFloat(_Input, _Prefix + "X_" + _Units);
    _Vector.y = Util::GetFloat(_Input, _Prefix + "Y_" + _Units);
    _Vector.z = Util::GetFloat(_Input, _Prefix + "Z_" + _Units);
}


void GetFloatVector(std::vector<float>* _Vector, nlohmann::json* _Input, std::string _JSONKey) {
    _Vector->clear();
    nlohmann::json Vector = (*_Input)[_JSONKey];
    for (nlohmann::json::iterator it = Vector.begin(); it != Vector.end(); it++) {
        _Vector->push_back(it.value());
    }    
}

}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG
