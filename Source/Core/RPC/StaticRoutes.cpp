#include <RPC/StaticRoutes.h>



namespace BG {
namespace NES {
namespace API {

std::string GetAPIVersion() {
    return VERSION;
}

std::string Echo(std::string _Data) {
    return _Data;
}

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG