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


Interface::Interface(BG::Common::Logger::LoggingSystem* _Logger, int _NumArgs, char** _ArgValues) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

    // Instantiate Subclass RendererManager
    RendererManager_ = std::make_unique<Manager>(_Logger);





    

    // create the viewer and assign window(s) to it
    auto viewer = vsg::Viewer::create();

    vsg::ref_ptr<vsg::Window> window(vsg::Window::create(windowTraits));
    if (!window)
    {
        std::cout<<"Could not create window."<<std::endl;
        return;
    }

    viewer->addWindow(window);

    // compute the bounds of the scene graph to help position the camera
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min+computeBounds.bounds.max)*0.5;
    double radius = vsg::length(computeBounds.bounds.max-computeBounds.bounds.min)*0.6;
    double nearFarRatio = 0.0001;

    // set up the camera
    auto lookAt = vsg::LookAt::create(centre+vsg::dvec3(0.0, -radius*3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

    vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
    if (vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(scene->getObject<vsg::EllipsoidModel>("EllipsoidModel")); ellipsoidModel)
    {
        // EllipsoidPerspective is useful for whole earth databases where per frame management of the camera's near & far values is optimized
        // to the current view relative to an ellipsoid model of the earth so that when near to the earth the near and far planes are pulled in close to the eye
        // and when far away from the earth's surface the far plane is pushed out to ensure that it encompasses the horizon line, accounting for mountains over the horizon.
        perspective = vsg::EllipsoidPerspective::create(lookAt, ellipsoidModel, 30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio, horizonMountainHeight);
    }
    else
    {
        perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio*radius, radius * 4.5);
    }

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    // add close handler to respond to pressing the window's close window button and to pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add a trackball event handler to control the camera view using the mouse
    viewer->addEventHandler(vsg::Trackball::create(camera));

    // create a command graph to render the scene on the specified window
    auto commandGraph = vsg::createCommandGraphForView(window, camera, scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    // compile all the Vulkan objects and transfer data required to render the scene
    viewer->compile();

    // rendering main loop
    while (viewer->advanceToNextFrame())
    {
        // pass any events into EventHandlers assigned to the Viewer
        viewer->handleEvents();

        viewer->update();

        viewer->recordAndSubmit();

        viewer->present();
    }

}

Interface::~Interface() {
    assert(Logger_ != nullptr);
    Logger_->Log("Shutting Down NES Rendering Subsystem", 3);


}


bool Interface::Initialize(int _NumArgs, char** _ArgValues) {
    assert(Logger_ != nullptr);

    return RendererManager_->Initialize(_NumArgs, _ArgValues);
}

bool Interface::DrawFrame() {
    return RendererManager_->DrawFrame();
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG