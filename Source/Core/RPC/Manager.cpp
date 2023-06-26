#include <RPC/Manager.h>



namespace BG {
namespace NES {
namespace API {


class Test {
private:
    int foo_;
public:
    Test(int foo) {
        foo_ = foo;
    };
    ~Test() {
    };
    int Add(int a, int b) {
        foo_ += a + b;
        return a + b;
    }

    std::string Echo(std::string _in) {
        return _in;
    }
};

Manager::Manager(Config::Config* _Config) {

    // Intiialize Server
    std::string ServerHost = _Config->Host;
    int ServerPort = _Config->PortNumber;

    RPCServer_ = std::make_unique<rpc::server>(ServerPort);

    // Register Basic Routes
    RPCServer_->bind("GetAPIVersion", &GetAPIVersion);
    RPCServer_->bind("Echo", &Echo);
    
    int ThreadCount = 4;
    RPCServer_->async_run(ThreadCount);

    // rpc::server srv(8001);
    // Test TestInstance(0);
    // srv.bind("GetAPIVersion", &GetVersion);
    // srv.bind("add", [&TestInstance](int a, int b){ return TestInstance.Add(a, b); });
    // srv.bind("echo", [&TestInstance](std::string _in){return TestInstance.Echo(_in); });
    // srv.run();

    // NOTE: you have to make sure that the lifetime of foo_obj
    // exceeds that of the server.

}

Manager::~Manager() {

}


}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG