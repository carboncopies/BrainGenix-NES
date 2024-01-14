#include <Simulator/BallAndStick/BSAlignedNCRandomUniform.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

static Geometries::Vec3D
findSomaPosition(std::vector<Geometries::Vec3D> &somaPositions,
                 Geometries::Vec3D &domainCenter_um,
                 Geometries::Vec3D &domainDims_um, float distThreshold,
                 size_t seed) {
    static std::mt19937 _gen(seed);
    static std::uniform_real_distribution<> _dist(-0.5, 0.5);

    bool needPos = true;
    Geometries::Vec3D coord{};

    while (needPos) {
        coord.x = _dist(_gen);
        coord.y = _dist(_gen);
        coord.z = _dist(_gen);

        coord = coord * domainDims_um + domainCenter_um;

        // Check it isn't too close to other neurons already placed
        needPos = false;

        for (const auto &somaPos : somaPositions) {
            float dist = coord.Distance(somaPos);

            if (dist > distThreshold)
                continue;
            needPos = true;
            break;
        }
    }
    somaPositions.emplace_back(coord);

    return coord;
}

static size_t findNearestIdx(size_t idx, std::vector<int> axonsTo,
                             std::vector<Geometries::Vec3D> &somaPositions,
                             Geometries::Vec3D domainDims_um) {
    int nearest = -1;
    float minDist = domainDims_um.Max();

    for (size_t i = 0; i < somaPositions.size(); ++i) {
        if (i == idx)
            continue;
        if (axonsTo[i] == idx)
            continue;
        float dist = somaPositions[idx].Distance(somaPositions[i]);
        if (dist >= minDist)
            continue;
        minDist = dist;
        nearest = i;
    }
    return static_cast<size_t>(nearest);
}

//! Constructors
BSAlignedNCRandomUniform::BSAlignedNCRandomUniform(int _ID, Geometries::GeometryCollection * _Collection_ptr)
    : BSAlignedNC(_ID, _Collection_ptr){};

BSAlignedNCRandomUniform::BSAlignedNCRandomUniform(int _ID, int _NumCells, Geometries::GeometryCollection * _Collection_ptr)
    : BSAlignedNC(_ID, _NumCells, _Collection_ptr){};

BSAlignedNCRandomUniform::BSAlignedNCRandomUniform(int _ID, int _NumCells, size_t _Seed, Geometries::GeometryCollection * _Collection_ptr)
    : BSAlignedNC(_ID, _NumCells, _Collection_ptr) {
    this->Seed = _Seed;
};

//! Initializes the neurons in the neural circuit.
void BSAlignedNCRandomUniform::InitCells(Geometries::Box * domain) {
    //auto boxDomain = std::dynamic_pointer_cast<Geometries::Box>(domain);

    assert(domain);

    float somaRadius_um = _DEFAULT_SOMA_RADIUS_um;
    float end0Radius_um = _DEFAULT_END0_RADIUS_um;
    float end1Radius_um = _DEFAULT_END0_RADIUS_um;
    float distThreshold = 2 * somaRadius_um;
    std::vector<Geometries::Vec3D> somaPositions{};
    std::vector<Geometries::Sphere *> somas{};  // Regular pointers, because the objects are maintained in Simulation.Collection.
    std::vector<int> axonsTo{};

    for (size_t i = 0; i < this->NumCells; ++i) {
        // Pick a random location
        auto coord = findSomaPosition(somaPositions, domain->Center_um, domain->Dims_um, distThreshold, this->Seed);

        // Create and place a soma
        Geometries::Sphere * soma_ptr = &Collection_ptr->AddSphere(coord, somaRadius_um);
        somas.emplace_back(soma_ptr);

        // Initialize axonsTo
        axonsTo.emplace_back(-1);
    }

    for (size_t i = 0; i < this->NumCells; ++i) {
        // Create an axon and direct it.
        size_t idxTo = findNearestIdx(i, axonsTo, somaPositions, domain->Dims_um);
        axonsTo[i] = idxTo;

        auto dvAxon = somaPositions[idxTo] - somaPositions[i];
        float mag = sqrt(dvAxon.Dot(dvAxon));

        dvAxon = dvAxon / mag;
        auto end0 = somaPositions[i] + dvAxon * somaRadius_um;
        auto end1 = somaPositions[idxTo] - dvAxon * somaRadius_um;
        Geometries::Cylinder * axon_ptr = &Collection_ptr->AddCylinder(end0Radius_um, end0, end1Radius_um, end1);

        // Create neuron
        this->Cells[std::to_string(i)] = std::make_shared<BSNeuron>(i, somas[i], axon_ptr);
    }
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
