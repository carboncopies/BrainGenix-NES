#include <VSDA/RenderPool.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {


void RenderPool::RendererThreadMainFunction(int _ThreadNumber) {

    // Setup Renderer
    BG::NES::Renderer::Interface Renderer(Logger_);
    if (!Renderer.Initialize()) { 
        Logger_->Log("Error During Renderer Initialization, Aborting", 10);
        return;
    }


    



}



RenderPool::RenderPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);

    Logger_ = _Logger;

    // Create Renderer Instances
    // for (unsigned int i = 0; i < _NumThreads; i++) {

    // }
    RenderThread_ = std::thread(&RenderPool::RendererThreadMainFunction, this, 0);


}




}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG