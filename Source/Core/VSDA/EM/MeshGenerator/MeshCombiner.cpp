#include <VSDA/EM/MeshGenerator/MeshCombiner.h>

namespace BG {
namespace NES {
namespace Simulator {

void MeshCombiner::Combine(Mesh& target, const Mesh& source) {
    size_t vertexOffset = target.vertices.size();
    target.vertices.insert(target.vertices.end(), 
                         source.vertices.begin(), source.vertices.end());
    
    for (auto index : source.indices) {
        target.indices.push_back(static_cast<uint32_t>(index + vertexOffset));
    }
}

} // namespace Simulator
} // namespace NES
} // namespace BG