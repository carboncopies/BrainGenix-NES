//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for BackendServiceCommunicator.
    Additional Notes: Integration test - requires BrainGenix-API to be running
    Date Created: 2025-12-24
*/

#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <chrono>
#include <memory>

#include <BG/Common/Logger/Logger.h>
#include <RPC/BackendServiceCommunicator.h>
#include <RPC/SafeClient.h>

/**
 * @brief Test class for BackendServiceCommunicator Integration Tests
 *  
 * Note: These tests require the BrainGenix-API service to be running
 * and accessible. The API should be configured to handle "QueryBackendService"
 * route and route requests to NES service.
 */
 struct BackendServiceCommunicatorTest : testing::Test {
    BG::Common::Logger::LoggingSystem Logger;
    std::unique_ptr<BG::SafeClient> APIClient;
    std::unique_ptr<BackendServiceCommunicator> Communicator;

    // API connection settings - adjust these for your test environment
    std::string APIHost = "localhost";  // Change if API is on different host
    int APIPort = 8006;                  // rpclib port (8005 is HTTP, 8006 is rpclib) CONFIGURE THIS TO THE RPCCALLBACK PORT

    void SetUp() override {
        // Initialize Logger
        Logger.SetKeepVectorLogs(false);

        // Create and configure SafeClient for API connection
        APIClient = std::make_unique<BG::SafeClient>(&Logger);
        APIClient->SetHostPort(APIHost, APIPort);
        APIClient->SetTimeout(5000);  // 5 second timeout for tests

        // Wait a moment for connection to establish
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Create BackendServiceCommunicator
        Communicator = std::make_unique<BackendServiceCommunicator>(
            APIClient.get(),
            &Logger
        );
    }

    void TearDown() override {
        // Cleanup happens automatically via unique_ptr destructors
    }
};

/**
 * @brief Test querying NES backend service through the API.
 * 
 * This test verifies that:
 * 1. BackendServiceCommunicator can successfully query the API
 * 2. The API routes the request to NES service
 * 3. NES service returns "PING PING" response
 * 
 * Prerequisites:
 * - BrainGenix-API must be running and accessible
 * - API must be configured to route "QueryBackendService" requests
 * - NES service handler must return "PING PING" for test queries
 */
 TEST_F(BackendServiceCommunicatorTest, test_QueryNES_ReturnsPing) {
    // Arrange
    std::string BackendService = "NES";
    std::string RPCQuery = "TestPing";  // Any RPC query name for testing
    std::string QueryContent = "{}";     // Empty JSON for ping test
    std::string ExpectedResponse = "PING PING";

    // Act
    std::string Response = Communicator->QueryOtherBackendServiceCommunicator(
        BackendService,
        RPCQuery,
        QueryContent
    );

    // Assert
    ASSERT_FALSE(Response.empty()) << "Response should not be empty";
    
    // Check if response contains the expected ping message
    // Note: The response might be wrapped in JSON, so we check if it contains the ping
    ASSERT_TRUE(Response.find(ExpectedResponse) != std::string::npos) 
        << "Expected response to contain '" << ExpectedResponse 
        << "', but got: " << Response;
}

/**
 * @brief Test that invalid service name returns error.
 */
TEST_F(BackendServiceCommunicatorTest, test_InvalidServiceName_ReturnsError) {
    // Arrange
    std::string InvalidService = "INVALID_SERVICE";
    std::string RPCQuery = "TestQuery";
    std::string QueryContent = "{}";

    // Act
    std::string Response = Communicator->QueryOtherBackendServiceCommunicator(
        InvalidService,
        RPCQuery,
        QueryContent
    );

    // Assert
    ASSERT_FALSE(Response.empty()) << "Error response should not be empty";
    
    // Response should be a JSON error
    // Check if it contains error indicators
    ASSERT_TRUE(Response.find("StatusCode") != std::string::npos ||
                Response.find("ErrorMessage") != std::string::npos ||
                Response.find("Invalid") != std::string::npos)
        << "Expected error response, but got: " << Response;
}

/**
 * @brief Test that null APIClient returns error gracefully.
 */
TEST_F(BackendServiceCommunicatorTest, test_NullAPIClient_ReturnsError) {
    // Arrange - create communicator with null client
    BackendServiceCommunicator NullClientCommunicator(nullptr, &Logger);
    
    std::string BackendService = "NES";
    std::string RPCQuery = "TestQuery";
    std::string QueryContent = "{}";

    // Act
    std::string Response = NullClientCommunicator.QueryOtherBackendServiceCommunicator(
        BackendService,
        RPCQuery,
        QueryContent
    );

    // Assert
    ASSERT_FALSE(Response.empty()) << "Error response should not be empty";
    ASSERT_TRUE(Response.find("StatusCode") != std::string::npos ||
                Response.find("ErrorMessage") != std::string::npos ||
                Response.find("not initialized") != std::string::npos)
        << "Expected error about uninitialized client, but got: " << Response;
}