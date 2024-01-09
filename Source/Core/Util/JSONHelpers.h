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
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Util {


/**
 * @brief Helper function that returns the integer from the given JSON object with the given string.
 * 
 * @param _JSON 
 * @param _ParamName 
 * @return int 
 */
int GetInt(nlohmann::json *_JSON, std::string _ParamName);

/**
 * @brief Helper function that gets a float from the JSON object given with the parameter name passed.
 * 
 * @param _JSON 
 * @param _ParamName 
 * @return float 
 */
float GetFloat(nlohmann::json *_JSON, std::string _ParamName);

/**
 * @brief Another helper function that gets the string from the provided JSON object, with the given _ParamName.
 * 
 * @param _JSON 
 * @param _ParamName 
 * @return std::string 
 */
std::string GetString(nlohmann::json *_JSON, std::string _ParamName);

/**
 * @brief Helper function which creates a JSON vector at _Target, with the given input, prefix, and units.
 * 
 * @param _Target 
 * @param _Input 
 * @param _Prefix 
 * @param _Units 
 */
void SetVec3(nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units="um");

/**
 * @brief Another helper function, which will set the vector to the given JSON string.
 * 
 * @param _Vector 
 * @param _Input 
 */
void SetVec3(Simulator::Geometries::Vec3D& _Vector, std::string _Input);

/**
 * @brief Helper function that sets the given vector given a pointer to some JSON, and a prefix and units to search for.
 * 
 * @param _Vector 
 * @param _Input 
 * @param _Prefix 
 * @param _Units 
 */
void GetVec3(Simulator::Geometries::Vec3D& _Vector, nlohmann::json* _Input, std::string _Prefix, std::string _Units="um");

/**
 * @brief Attempts to get values from an array parameter, example: foo=[1,2,3]
 * 
 * @param _Vector Vector that we're going to populate with the result.
 * @param _Input Input bit of json (pointer to).
 * @param _Name Name of parameter we're trying to get.
 */
void GetArrVec3(Simulator::Geometries::Vec3D& _Vector, nlohmann::json* _Input, std::string _Name);

/**
 * @brief Simple helper that will populate a vector given the _Input data and the JSONKey input.
 * 
 * @param _Vector 
 * @param _Input 
 * @param _JSONKey 
 */
void GetFloatVector(std::vector<float>* _Vector, nlohmann::json* _Input, std::string _JSONKey);


}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG
