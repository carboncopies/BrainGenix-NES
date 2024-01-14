add_library(NESRenderer 

    "BG/Renderer/Interface.h"
    "BG/Renderer/Interface.cpp"


    "BG/Renderer/State/Scene.h"
    "BG/Renderer/State/Scene.cpp"
    "BG/Renderer/State/RenderData.h"
    "BG/Renderer/State/RenderData.cpp"

    "BG/Renderer/EncoderPool/Image.h"
    "BG/Renderer/EncoderPool/Image.cpp"
    "BG/Renderer/EncoderPool/EncoderPool.h"
    "BG/Renderer/EncoderPool/EncoderPool.cpp"

    "BG/Renderer/SceneGraph/Manager.h"
    "BG/Renderer/SceneGraph/Manager.cpp"
    "BG/Renderer/SceneGraph/GeometryBuilder.h"
    "BG/Renderer/SceneGraph/GeometryBuilder.cpp"
    "BG/Renderer/SceneGraph/CameraHelpers.h"
    "BG/Renderer/SceneGraph/CameraHelpers.cpp"
    "BG/Renderer/SceneGraph/OffscreenRenderHelpers.h"
    "BG/Renderer/SceneGraph/OffscreenRenderHelpers.cpp"

    "BG/Renderer/SceneGraph/Primitive/Cube.h"
    "BG/Renderer/SceneGraph/Primitive/Cube.cpp"
    "BG/Renderer/SceneGraph/Primitive/Sphere.h"
    "BG/Renderer/SceneGraph/Primitive/Sphere.cpp"
    "BG/Renderer/SceneGraph/Primitive/Cylinder.h"
    "BG/Renderer/SceneGraph/Primitive/Cylinder.cpp"

    "BG/Renderer/SceneGraph/Shader/Shader.h"
    "BG/Renderer/SceneGraph/Shader/Shader.cpp"
    "BG/Renderer/SceneGraph/Shader/Phong.h"
    "BG/Renderer/SceneGraph/Shader/Phong.cpp"

    "BG/Renderer/Math/MathHelpers.h"
    "BG/Renderer/Math/MathHelpers.cpp"

)
target_link_libraries(NESRenderer
    bg-common-logger
    Vulkan::Vulkan
    vsg::vsg


)
target_include_directories(NESRenderer PUBLIC ${SRC_DIR}/Renderer)

target_include_directories(NESRenderer PRIVATE ${Stb_INCLUDE_DIR})