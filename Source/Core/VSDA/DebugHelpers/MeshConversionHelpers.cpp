#include <VSDA/DebugHelpers/MeshBuilder.h>

namespace BG {
namespace NES {
namespace Simulator {

vsg::vec3 ConvertVector(const Geometries::Vec3D _Source) {
    return vsg::vec3(_Source.x, _Source.y, _Source.z);
}

Geometries::Vec3D ConvertVector(const vsg::vec3 _Source) {
    Geometries::Vec3D Out;
    Out.x = _Source.x;
    Out.y = _Source.y;
    Out.z = _Source.z;
    return Out;
}

vsg::vec3 RadsToDeg(const vsg::vec3 _Source) {
    return vsg::vec3(vsg::degrees(_Source.x), vsg::degrees(_Source.y),
                     vsg::degrees(_Source.z));
}

vsg::vec3 DegToRads(const vsg::vec3 _Source) {
    return vsg::vec3(vsg::radians(_Source.x), vsg::radians(_Source.y),
                     vsg::radians(_Source.z));
}

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
