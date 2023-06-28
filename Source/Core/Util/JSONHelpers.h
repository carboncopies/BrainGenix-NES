//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides helper utils for managing json requests/responses.
    Additional Notes: None
    Date Created: 2023-06-27
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Util {


int GetInt(nlohmann::json *_JSON, std::string _ParamName);
float GetFloat(nlohmann::json *_JSON, std::string _ParamName);
std::string GetString(nlohmann::json *_JSON, std::string _ParamName);

void SetVec3(nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units="nm");
void SetVec3(float* _Vector, std::string _Input);

void GetVec3(float* _Vector, nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units="nm");

}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG