#include <Simulator/BallAndStick/BSMorphology.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Create the soma of a neuron in the shape of a sphere.
std::unique_ptr<Geometries::Sphere>
CreateBSSoma(std::vector<std::vector<float>> domainBounds, Align align,
             float radius_um) {

    std::unique_ptr<Geometries::Sphere> soma =
        std::make_unique<Geometries::Sphere>();
    return soma;
};

//! Create the axon of a neuron in the shape of a cylinder.
std::unique_ptr<Geometries::Cylinder>
CreateBSAxon(std::vector<std::vector<float>> domainBounds, Align align,
             float somaRadius_um, float end0Radius_um, float end1Radius_um) {
    std::unique_ptr<Geometries::Cylinder> axon =
        std::make_unique<Geometries::Cylinder>();
    return axon;
};

//! Create a receptor in the form of a box.
std::unique_ptr<Geometries::Box>
CreateBSReceptor(Geometries::Vec3D receptorLocation_um) {
    std::unique_ptr<Geometries::Box> receptor =
        std::make_unique<Geometries::Box>();
    return receptor;
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
