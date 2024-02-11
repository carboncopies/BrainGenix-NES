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
 * @param _JSON Pointer to the JSON object.
 * @param _ParamName Name of the parameter to retrieve.
 * @return int The integer value retrieved from the JSON object.
 */
int GetInt(const nlohmann::json *_JSON, std::string _ParamName);


/**
 * @brief Helper function that gets a float from the JSON object given with the parameter name passed.
 *
 * @param _JSON Pointer to the JSON object.
 * @param _ParamName Name of the parameter to retrieve.
 * @return float The float value retrieved from the JSON object.
 */
float GetFloat(const nlohmann::json *_JSON, std::string _ParamName);


/**
 * @brief Another helper function that gets the string from the provided JSON object, with the given _ParamName.
 *
 * @param _JSON Pointer to the JSON object.
 * @param _ParamName Name of the parameter to retrieve.
 * @return std::string The string value retrieved from the JSON object.
 */
std::string GetString(const nlohmann::json *_JSON, std::string _ParamName);


/**
 * @brief Helper function which creates a JSON vector at _Target, with the given input, prefix, and units.
 *
 * @param _Target Pointer to the target JSON object.
 * @param _Input The input string.
 * @param _Prefix The prefix to search for in the input.
 * @param _Units The units for the vector (default is "um").
 */
void SetVec3(nlohmann::json* _Target, std::string _Input, std::string _Prefix, std::string _Units="um");


/**
 * @brief Another helper function, which will set the vector to the given JSON string.
 *
 * @param _Vector Reference to the Vec3D vector to be set.
 * @param _Input The input string.
 */
void SetVec3(Simulator::Geometries::Vec3D& _Vector, std::string _Input);

/**
 * @brief Set the JSON target from a 3D vector.
 *
 * @param _Target Reference to the target JSON object.
 * @param _Input The input 3D vector.
 * @param _Prefix The prefix to search for in the input.
 * @param _Units The units for the vector (default is "um").
 */
void SetVec3(nlohmann::json & _Target, const Simulator::Geometries::Vec3D& _Input, const std::string & _Prefix, const std::string & _Units="um");

/**
 * @brief Helper function that sets the given vector given a pointer to some JSON, and a prefix and units to search for.
 *
 * @param _Vector Reference to the Vec3D vector to be set.
 * @param _Input Pointer to the JSON object.
 * @param _Prefix The prefix to search for in the JSON.
 * @param _Units The units for the vector (default is "um").
 */
void GetVec3(Simulator::Geometries::Vec3D& _Vector, const nlohmann::json* _Input, std::string _Prefix, std::string _Units="um");


/**
 * @brief Attempts to get values from an array parameter, example: foo=[1,2,3].
 *
 * @param _Vector Reference to the Vec3D vector to be populated with the result.
 * @param _Input Pointer to the JSON array object.
 * @param _Name Name of the parameter to retrieve.
 */
void GetArrVec3(Simulator::Geometries::Vec3D& _Vector, const nlohmann::json* _Input, std::string _Name);


/**
 * @brief Simple helper that will populate a vector given the _Input data and the JSONKey input.
 *
 * @param _Vector Pointer to the vector to be populated.
 * @param _Input Pointer to the JSON object.
 * @param _JSONKey The key in the JSON object.
 */
void GetIntVector(std::vector<int>* _Vector, const nlohmann::json* _Input, std::string _JSONKey);


/**
 * @brief Simple helper that will populate a vector given the _Input data and the JSONKey input.
 *
 * @param _Vector Pointer to the vector to be populated.
 * @param _Input Pointer to the JSON object.
 * @param _JSONKey The key in the JSON object.
 */
void GetFloatVector(std::vector<float>* _Vector, const nlohmann::json* _Input, std::string _JSONKey);

/**
 * @brief Simple helper that will populate a vector given the _Input data and the JSONKey input.
 *
 * @param _Vector Pointer to the vector to be populated.
 * @param _Input Pointer to the JSON object.
 * @param _JSONKey The key in the JSON object.
 */
void GetStringVector(std::vector<std::string>* _Vector, const nlohmann::json* _Input, std::string _JSONKey);



}; // Close Namespace Util
}; // Close Namespace NES
}; // Close Namespace BG
