#include <Util/JSONHelpers.h>

namespace BG {
namespace NES {
namespace Util {

int GetInt(const nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON).at(_ParamName).template get<int>();
}

float GetFloat(const nlohmann::json *_JSON, std::string _ParamName) {
    try {
        return (*_JSON).at(_ParamName).template get<float>();
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

std::string GetString(const nlohmann::json *_JSON, std::string _ParamName) {
    return (*_JSON).at(_ParamName).template get<std::string>();
}

// From stringified JSON to JSON.
void SetVec3(nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units) {
    nlohmann::json Vec3 = nlohmann::json::parse(_Input);
    (*_Target)[_Prefix + "X_" + _Units] = Vec3[0].template get<float>();
    (*_Target)[_Prefix + "Y_" + _Units] = Vec3[1].template get<float>();
    (*_Target)[_Prefix + "Z_" + _Units] = Vec3[2].template get<float>();
}

// From stringified JSON to Vec3.
void SetVec3(Simulator::Geometries::Vec3D& _Vector, std::string _Input) {
    nlohmann::json Vec3 = nlohmann::json::parse(_Input);
    _Vector.x = Vec3[0].template get<float>();
    _Vector.y = Vec3[1].template get<float>();
    _Vector.z = Vec3[2].template get<float>();
}

// From Vec3 to JSON..
void SetVec3(nlohmann::json & _Target, const Simulator::Geometries::Vec3D& _Input, const std::string & _Prefix, const std::string & _Units) {
    _Target[_Prefix + "X_" + _Units] = _Input.x;
    _Target[_Prefix + "Y_" + _Units] = _Input.y;
    _Target[_Prefix + "Z_" + _Units] = _Input.z;
}

void GetVec3(Simulator::Geometries::Vec3D& _Vector, const nlohmann::json* _Input, std::string _Prefix, std::string _Units) {
    _Vector.x = Util::GetFloat(_Input, _Prefix + "X_" + _Units);
    _Vector.y = Util::GetFloat(_Input, _Prefix + "Y_" + _Units);
    _Vector.z = Util::GetFloat(_Input, _Prefix + "Z_" + _Units);
}


void GetArrVec3(Simulator::Geometries::Vec3D& _Vector, const nlohmann::json* _Input, std::string _Name) {
    std::vector<float> Values = (*_Input).at(_Name).get<std::vector<float>>();

    if (Values.size() != 3) {
        std::cout<<"Error (Util::GetArrVec3), Cannot Get Vector Array From JSON, Size Is Not 3\n";
    }

    _Vector.x = Values[0];
    _Vector.y = Values[1];
    _Vector.z = Values[2];

}


void GetIntVector(std::vector<int>* _Vector, const nlohmann::json* _Input, std::string _JSONKey) {
    _Vector->clear();
    nlohmann::json Vector = (*_Input).at(_JSONKey);
    for (nlohmann::json::iterator it = Vector.begin(); it != Vector.end(); it++) {
        _Vector->push_back(it.value());
    }    
}

void GetFloatVector(std::vector<float>* _Vector, const nlohmann::json* _Input, std::string _JSONKey) {
    _Vector->clear();
    nlohmann::json Vector = (*_Input).at(_JSONKey);
    for (nlohmann::json::iterator it = Vector.begin(); it != Vector.end(); it++) {
        _Vector->push_back(it.value());
    }    
}

void GetStringVector(std::vector<std::string>* _Vector, const nlohmann::json* _Input, std::string _JSONKey) {
    _Vector->clear();
    nlohmann::json Vector = (*_Input).at(_JSONKey);
    for (nlohmann::json::iterator it = Vector.begin(); it != Vector.end(); it++) {
        _Vector->push_back(it.value());
    }    
}

}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG
