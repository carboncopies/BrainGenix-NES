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
 * @brief This class provides the infrastructure to run simulations and manage VSDA routes.
 * 
 * The VSDARPCInterface class connects the NES routes for VSDA to the API, enabling
 * operations such as initializing simulations, setting up microscopes, defining scan regions,
 * and managing render operations.
 */
class VSDARPCInterface {

private:
    std::vector<std::unique_ptr<Simulation>>* SimulationsPtr_; /**< Pointer to a vector containing simulation instances. The index in this vector corresponds to the simulation's ID. */

    BG::NES::Renderer::Interface* Renderer_ = nullptr; /**< Pointer to the renderer instance. */
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**< Pointer to the logging system instance. */
    API::RPCManager* RPCManager_; /**< Pointer to the RPC Manager instance. */

public:

    /**
     * @brief Construct a new VSDARPCInterface object.
     * 
     * @param _Logger Pointer to the logging system interface.
     * @param _RPCManager Pointer to the RPC Manager instance.
     * @param _SimulationsVectorPointer Pointer to the vector containing simulation instances.
     */
    VSDARPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer);

    /**
     * @brief Destroy the VSDARPCInterface object.
     */
    ~VSDARPCInterface();

    /**
     * @brief Handles the EM Initialize route for NES.
     * 
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMInitialize(std::string _JSONRequest);

    /**
     * @brief Sets up the microscope parameters.
     * 
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMSetupMicroscope(std::string _JSONRequest);

    /**
     * @brief Defines the scan region for the given simulation.
     * 
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMDefineScanRegion(std::string _JSONRequest);

    /**
     * @brief Queues a simulation render operation for the EM renderer.
     * 
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMQueueRenderOperation(std::string _JSONRequest);

    /**
     * @brief Checks the result of a given render operation.
     * 
     * Returns whether the operation is busy or completed.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMGetRenderStatus(std::string _JSONRequest);

    /**
     * @brief Returns a completed image stack.
     * 
     * Must have already been rendered prior to calling, otherwise an error will be returned to the client.
     * Modifies the simulation struct for the requested ID.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMGetImageStack(std::string _JSONRequest);

    /**
     * @brief Retrieves index data for the simulation.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMGetIndexData(std::string _JSONRequest);

    /**
     * @brief Prepares a Neuroglancer dataset for visualization.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMPrepareNeuroglancerDataset(std::string _JSONRequest);

    /**
     * @brief Retrieves a handle for the Neuroglancer dataset.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMGetDatasetHandle(std::string _JSONRequest);

    /**
     * @brief Retrieves the URL for the Neuroglancer dataset.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDAEMGetNeuroglancerDatasetURL(std::string _JSONRequest);

    /**
     * @brief Retrieves a rendered image as a base64-encoded string.
     * 
     * The image must have been rendered and returned in the getstack function.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Base64-encoded image string.
     */
    std::string VSDAGetImage(std::string _JSONRequest);

    /**
     * @brief Initializes the CA route for NES.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACAInitialize(std::string _JSONRequest);

    /**
     * @brief Sets up the microscope parameters for the CA route.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACASetupMicroscope(std::string _JSONRequest);

    /**
     * @brief Defines the scan region for the CA route.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACADefineScanRegion(std::string _JSONRequest);

    /**
     * @brief Queues a render operation for the CA renderer.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACAQueueRenderOperation(std::string _JSONRequest);

    /**
     * @brief Checks the result of a render operation for the CA renderer.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACAGetRenderStatus(std::string _JSONRequest);

    /**
     * @brief Retrieves a completed image stack for the CA renderer.
     * 
     * @param _JSONRequest JSON string containing the request data.
     * @return std::string Response as a JSON string.
     */
    std::string VSDACAGetImageStack(std::string _JSONRequest);

};

}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
