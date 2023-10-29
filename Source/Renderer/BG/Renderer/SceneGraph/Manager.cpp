#include <BG/Renderer/SceneGraph/Manager.h>



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
    assert(_Logger != nullptr);
    Logger_ = _Logger;





}

Manager::~Manager() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down Rendering Subsystem", 3);


    
}


bool Manager::Initialize(int _NumArgs, char** _ArgValues) {
    assert(Logger_ != nullptr);
    Logger_->Log("Initializing NES Rendering Subsystem", 5);

    // Iniitalize State Structs
    RenderData_ = std::make_unique<State::RenderData>();
    Scene_      = std::make_unique<State::Scene>();

    // Setup Renderer


    return true;
}

bool Manager::DrawFrame() {

    

    return false;
}


bool Manager::IsReady() {
}

bool Manager::IsWindowed() {
}

bool Manager::IsDebugging() {
}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG