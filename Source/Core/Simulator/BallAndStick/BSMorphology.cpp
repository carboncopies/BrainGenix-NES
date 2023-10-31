#include <Simulator/BallAndStick/BSMorphology.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Create the soma of a neuron in the shape of a sphere.
std::shared_ptr<Geometries::Sphere>
CreateBSSoma(std::vector<std::vector<float>> domainBounds, Align align,
             float radius_um) {
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

    return std::make_shared<Geometries::Sphere>(center_um, radius_um);
};

//! Create the axon of a neuron in the shape of a cylinder.
std::shared_ptr<Geometries::Cylinder>
CreateBSAxon(std::vector<std::vector<float>> domainBounds, Align align,
             float somaRadius_um, float end0Radius_um, float end1Radius_um) {
    Geometries::Vec3D End0Pos_um, End1Pos_um;
    switch (align) {
    case Align::ALIGN_LEFT:
        End0Pos_um = Geometries::Vec3D(
            (domainBounds[0][0] + domainBounds[1][0]) / 2.0, domainBounds[0][1],
            (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        End1Pos_um = Geometries::Vec3D(End0Pos_um.x_um,
                                       domainBounds[1][1] - somaRadius_um * 2.0,
                                       End0Pos_um.z_um);
        break;
    case Align::ALIGN_RIGHT:
        End0Pos_um = Geometries::Vec3D(
            (domainBounds[0][0] + domainBounds[1][0]) / 2.0, domainBounds[1][1],
            (domainBounds[0][2] + domainBounds[1][2]) / 2.0);
        End1Pos_um = Geometries::Vec3D(End0Pos_um.x_um,
                                       domainBounds[0][1] + somaRadius_um * 2.0,
                                       End0Pos_um.z_um);
        break;
    case Align::ALIGN_CENTER:
        throw std::invalid_argument(
            "Align::ALIGN_CENTER is invalid for creating a BSAxon.");
        break;
    default:
        break;
    };

    return std::make_shared<Geometries::Cylinder>(end0Radius_um, End0Pos_um,
                                                  end1Radius_um, End1Pos_um);
};

//! Create a receptor in the form of a box.
std::shared_ptr<Geometries::Box>
CreateBSReceptor(Geometries::Vec3D receptorLocation_um) {
    return std::make_shared<Geometries::Box>(
        receptorLocation_um,
        Geometries::Vec3D(_DEFAULT_RECEPTOR_DIM_um, _DEFAULT_RECEPTOR_DIM_um,
                          _DEFAULT_RECEPTOR_DIM_um));
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
