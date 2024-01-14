#include <Simulator/BallAndStick/BSMorphology.h>
//#include <Simulator/Structs/Simulation.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

/** (Randal)
 *  These must work with a Simulation to create new shapes, because all
 *  shape objects should be maintained and tracked in Simulation.Collection.
 *  Consequently, the functions here return only references to those
 *  objects.
 */

//! Create the soma of a neuron in the shape of a sphere.
Geometries::Sphere &
CreateBSSoma(Geometries::GeometryCollection & Collection, std::vector<std::vector<float>> domainBounds, Align align, float radius_um) {
    assert(radius_um > 0.0);

    Geometries::Vec3D center_um;
    switch (align) {
    case Align::ALIGN_LEFT:
        center_um =
            Geometries::Vec3D((domainBounds[0][0] + domainBounds[1][0]) / 2.0,
                              domainBounds[0][1] + radius_um,
                              (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        break;

    case Align::ALIGN_RIGHT:
        center_um =
            Geometries::Vec3D((domainBounds[0][0] + domainBounds[1][0]) / 2.0,
                              domainBounds[1][1] - radius_um,
                              (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        break;
    case Align::ALIGN_CENTER:
        center_um =
            Geometries::Vec3D((domainBounds[0][0] + domainBounds[1][0]) / 2.0,
                              (domainBounds[0][1] + domainBounds[1][1]) / 2.0,
                              (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        break;
    default:
        break;
    }

    return Collection.AddSphere(center_um, radius_um);
};

//! Create the axon of a neuron in the shape of a cylinder.
Geometries::Cylinder &
CreateBSAxon(Geometries::GeometryCollection & Collection, std::vector<std::vector<float>> domainBounds, Align align, float somaRadius_um, float end0Radius_um, float end1Radius_um) {
    Geometries::Vec3D End0Pos_um, End1Pos_um;
    switch (align) {
    case Align::ALIGN_LEFT:
        End0Pos_um = Geometries::Vec3D(
            (domainBounds[0][0] + domainBounds[1][0]) / 2.0, domainBounds[0][1],
            (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        End1Pos_um = Geometries::Vec3D(End0Pos_um.x,
                                       domainBounds[1][1] - somaRadius_um * 2.0,
                                       End0Pos_um.z);
        break;
    case Align::ALIGN_RIGHT:
        End0Pos_um = Geometries::Vec3D(
            (domainBounds[0][0] + domainBounds[1][0]) / 2.0, domainBounds[1][1],
            (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        End1Pos_um = Geometries::Vec3D(End0Pos_um.x,
                                       domainBounds[0][1] + somaRadius_um * 2.0,
                                       End0Pos_um.z);
        break;
    case Align::ALIGN_CENTER:
        throw std::invalid_argument(
            "Align::ALIGN_CENTER is invalid for creating a BSAxon.");
        break;
    default:
        break;
    };

    return Collection.AddCylinder(end0Radius_um, End0Pos_um, end1Radius_um, End1Pos_um);
};

//! Create a receptor in the form of a box.
//std::shared_ptr<Geometries::Box>
Geometries::Box &
CreateBSReceptor(Geometries::GeometryCollection & Collection, Geometries::Vec3D receptorLocation_um) {
    //return std::make_shared<Geometries::Box>(
    return Collection.AddBox(receptorLocation_um, Geometries::Vec3D(_DEFAULT_RECEPTOR_DIM_um, _DEFAULT_RECEPTOR_DIM_um, _DEFAULT_RECEPTOR_DIM_um));
    // return Geometries::Box(
    //     receptorLocation_um,
    //     Geometries::Vec3D(_DEFAULT_RECEPTOR_DIM_um, _DEFAULT_RECEPTOR_DIM_um,
    //                       _DEFAULT_RECEPTOR_DIM_um));
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
