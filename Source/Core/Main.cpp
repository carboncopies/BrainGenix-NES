#include <Main.h>

std::string GetVersion() {
    return "2023.06.25";
}

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
int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::NES::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::NES::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    rpc::server srv(8001);
    Test TestInstance(0);
    srv.bind("GetAPIVersion", &GetVersion);
    srv.bind("add", [&TestInstance](int a, int b){ return TestInstance.Add(a, b); });
    srv.bind("echo", [&TestInstance](std::string _in){return TestInstance.Echo(_in); });
    srv.run();

    // NOTE: you have to make sure that the lifetime of foo_obj
    // exceeds that of the server.

    return 0;


}

