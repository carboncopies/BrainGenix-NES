#include <BG/Renderer/Interface.h>



/**
 * MEMORY LEAK NOTICE:
 * 
 * There are false positives in this code! 
 * It's not actually leaking memory, valgrind just repots memory leaks due to dl_open calls.
 * To *actually* check for memory leaks, please use a null icd driver (see this for more info):
 *     https://github.com/KhronosGroup/Vulkan-Loader/issues/256
 * 
 * THIS IS A KNOWN BUG IN VALGRIND!
*/

namespace BG {
namespace NES {
namespace Renderer {


Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

    // Instantiate Subclass RendererManager
    RendererManager_ = std::make_unique<Manager>(_Logger);


}

Interface::~Interface() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down NES Rendering Subsystem", 3);


}


bool Interface::Initialize(bool _EnableDebugWindow, bool _EnableValidationLayers) {
    assert(Logger_ != nullptr);

    return RendererManager_->Initialize(_EnableDebugWindow, _EnableValidationLayers);
}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG