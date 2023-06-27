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


}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG