#include <RPC/StaticRoutes.h>



namespace BG {
namespace NES {
namespace API {

std::string GetAPIVersion() {
    return "2023-06-25";
}

std::string Echo(std::string _Data) {
    return _Data;
}

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG