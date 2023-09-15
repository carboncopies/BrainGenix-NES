//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This is the main file of the system.
    Additional Notes: None
    Date Created: 2023-06-24
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <functional>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/Interface.h>


#include <Config/ConfigurationManager.h>
#include <Config/Config.h>

#include <RPC/Manager.h>
#include <Simulator/Manager.h>


/**
 * Main function.
*/
int main(int NumArguments, char** ArgumentValues);