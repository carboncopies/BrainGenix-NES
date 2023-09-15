#include <BG/Renderer/Interface.h>



namespace BG {
namespace NES {
namespace Renderer {



Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger, bool _EnableDebugWindow) {
    EnableDebugWindow_ = _EnableDebugWindow;
    Logger_ = _Logger;



}



Interface::~Interface() {


}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG