#include <RPC/Manager.h>



namespace BG {
namespace NES {
namespace API {


Manager::Manager(Config::Config* _Config) {

    // Intiialize Server
    std::string ServerHost = _Config->Host;
    int ServerPort = _Config->PortNumber;

    RPCServer_ = std::make_unique<rpc::server>(ServerPort);

    // Register Basic Routes
    AddRoute("GetAPIVersion", &GetAPIVersion);
    AddRoute("Echo", &Echo);
    
    int ThreadCount = 1;
    RPCServer_->async_run(ThreadCount);

}

Manager::~Manager() {

}




}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG