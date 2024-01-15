#include <RPC/Manager.h>



namespace BG {
namespace NES {
namespace API {


Manager::Manager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger) {

    // Initialize Server
    std::string ServerHost = _Config->Host;
    int ServerPort = _Config->PortNumber;

    // Create a unique pointer to the RPC server and initialize it with the provided port number
    RPCServer_ = std::make_unique<rpc::server>(ServerPort);

    // Register Basic Routes
    // Add predefined routes to the RPC server
    AddRoute("GetAPIVersion", _Logger, &GetAPIVersion);
    AddRoute("Echo", _Logger, &Echo);
    
    int ThreadCount = 1;

    // Start the RPC server asynchronously with the specified thread count
    RPCServer_->async_run(ThreadCount);
}

Manager::~Manager() {
    // Destructor
    // No explicit cleanup needed as smart pointers manage the RPC server's memory
}


}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG