#include <BG/Renderer/Manager.h>



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



Manager::Manager(BG::Common::Logger::LoggingSystem* _Logger) {
    Logger_ = _Logger;

}



Manager::~Manager() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down NES Rendering Subsystem", 3);



}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG