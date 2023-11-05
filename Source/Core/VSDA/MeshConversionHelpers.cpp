#include <VSDA/MeshBuilder.h>



namespace BG {
namespace NES {
namespace Simulator {


vsg::vec3 ConvertVector(Geometries::Vec3D _Source) {
    return vsg::vec3(_Source.x_um, _Source.y_um, _Source.z_um);
}

Geometries::Vec3D ConvertVector(vsg::vec3 _Source) {
    Geometries::Vec3D Out;
    Out.x_um = _Source.x;
    Out.y_um = _Source.y;
    Out.z_um = _Source.z;
    return Out;
}

vsg::vec3 RadsToDeg(vsg::vec3 _Source)  {
    return vsg::vec3(
        vsg::degrees(_Source.x),
        vsg::degrees(_Source.y),
        vsg::degrees(_Source.z)
    );
}

vsg::vec3 DegToRads(vsg::vec3 _Source) {
    return vsg::vec3(
        vsg::radians(_Source.x),
        vsg::radians(_Source.y),
        vsg::radians(_Source.z)
    );
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG