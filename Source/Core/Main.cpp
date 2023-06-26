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

