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


// TODO: Update Doxygen Docs!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


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


bool Interface::Initialize(int _NumArgs, char** _ArgValues) {
    assert(Logger_ != nullptr);

    if (!RendererManager_->Initialize(_NumArgs, _ArgValues)) {
        return false;
    }
    if (!RendererManager_->SetupScene()) {
        return false;
    }
    return RendererManager_->SetupViewer();
}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG