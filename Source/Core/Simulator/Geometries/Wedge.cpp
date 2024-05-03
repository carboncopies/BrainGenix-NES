#include <iostream>

#include <Simulator/Geometries/Wedge.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

// Constructors
Wedge::Wedge(){
    GeometryShape = GeometryWedge;
};

Wedge::Wedge(const Vec3D & _End0Pos_um, const Vec3D & _End1Pos_um, float _End0Width_um, float _End0Height_um, float _End1Width_um, float _End1Height_um):
	End0Pos_um(_End0Pos_um), End1Pos_um(_End1Pos_um), End0Width_um(_End0Width_um), End1Width_um(_End1Width_um), End0Height_um(_End0Height_um), End1Height_um(_End1Height_um) {

    GeometryShape = GeometryWedge;
};


}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
