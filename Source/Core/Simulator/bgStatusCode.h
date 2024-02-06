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

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace API {

enum bgStatusCode {
    bgStatusSuccess = 0,
    bgStatusGeneralFailure = 1,
    bgStatusInvalidParametersPassed = 2,
    bgStatusUpstreamGatewayUnavailable = 3,
    bgStatusUnauthorizedInvalidNoToken = 4,
    bgStatusSimulationBusy = 5,
    NUMbgStatusCode
};

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG
