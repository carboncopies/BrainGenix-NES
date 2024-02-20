//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface which connects the NES routes for VSDA to the API.
    Additional Notes: None
    Date Created: 2023-12-02
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <iostream>
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <VSDA/RPCRoutes/EM.h>
#include <VSDA/RPCRoutes/Ca.h>

#include <BG/Renderer/Interface.h>
#include <BG/Renderer/SceneGraph/Primitive/Cube.h>

#include <BG/Common/Logger/Logger.h>
#include <Util/JSONHelpers.h>

#include <RPC/RPCManager.h>



namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



/**
 * @brief This class provides the infrastructure to run simulations.
 */
class VSDARPCInterface {

private:
    std::vector<std::unique_ptr<Simulation>>* SimulationsPtr_; /**Vector pointer containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/

    BG::NES::Renderer::Interface* Renderer_ = nullptr; /**Pointer to instance of renderer*/
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to instance of logging system*/


public:

    /**
     * @brief Construct a new VSDARPCInterface for VSDA Slice Generator object
     * 
     * @param _Logger Pointer to logging interface
     * @param _RPCManager Pointer to instance of the RPC manager.
     * @param _SimulationsPointerVector Pointer to vector which contains the other simulations. Allows us to access them and modify them as needed.
     */
    VSDARPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer);


    /**
     * @brief Destroy the VSDARPCInterface for VSDA Slice Generator object
     * 
     */
    ~VSDARPCInterface();


    /**
     * @brief Handles the EM Initialize route for NES.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMInitialize(std::string _JSONRequest);

    /**
     * @brief Sets up the microscope parameters.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMSetupMicroscope(std::string _JSONRequest);

    /**
     * @brief Defines the scan region for the given simulation.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMDefineScanRegion(std::string _JSONRequest);

    /**
     * @brief Queues a simulation render operation for the EM renderer.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMQueueRenderOperation(std::string _JSONRequest);

    /**
     * @brief Checks the Result of a given render operation.
     * Will return if it's busy or doing other things.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMGetRenderStatus(std::string _JSONRequest);

    /**
     * @brief Returns a completed image stack.
     * Must have already been rendered prior to calling, otherwise an error will be returned to the client.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAEMGetImageStack(std::string _JSONRequest);

    /**
     * @brief Will get an image that has been rendered and returned in the getstack function.
     * Will return this as a base64 encoded string which can be then converted to an image file later on by the renderer.
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string VSDAGetImage(std::string _JSONRequest);


    std::string VSDACAInitialize(std::string _JSONRequest);
    std::string VSDACASetupMicroscope(std::string _JSONRequest);
    std::string VSDACADefineScanRegion(std::string _JSONRequest);
    std::string VSDACAQueueRenderOperation(std::string _JSONRequest);
    std::string VSDACAGetRenderStatus(std::string _JSONRequest);
    std::string VSDACAGetImageStack(std::string _JSONRequest);
    



};


}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
