#pragma once

#include <VSDA/EM/MeshGenerator/MarchingCubes.h>

namespace BG {
namespace NES {
namespace Simulator {

class MeshCombiner {
public:
    static void Combine(Mesh& target, const Mesh& source);
};

} // namespace Simulator
} // namespace NES
} // namespace BG