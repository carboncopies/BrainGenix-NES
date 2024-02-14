//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides bgStatusCode.
    Additional Notes: None
    Date Created: 2024-02-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <functional>
// #include <memory>
// #include <map>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/ManagerTaskData.h>


namespace BG {
namespace NES {
namespace API {



// *** NOTE: By passing JSON objects/components as strings and then having to
//           parse them into JSON objects again, the handlers above are doing
//           a bunch of unnecessary extra work - you can just pass references
//           to components of a JSON object as a JSON object.
//           E.g. call AddBSNeuron(ReqParams) where ReqParams is as obtained
//           in NESRequest() below.

// typedef std::string NESRequest_func_t(const nlohmann::json&);

struct RouteAndHandler {
    std::string Route_;
    std::function<std::string>& Handler_;
};




}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG
