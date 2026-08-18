#include <RPC/StaticRoutes.h>



namespace BG {
namespace NES {
namespace API {

std::string GetAPIVersion() {
    return NES_API_VERSION;
}

std::string Echo(std::string _Data) {
    return _Data;
}

} // Close Namespace API
} // Close Namespace NES
} // Close Namespace BG
