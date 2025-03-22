#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <vector>
#include <unordered_map>
#include <array>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h> // Include your VoxelArray header
#include <VSDA/EM/MeshGenerator/MarchingCubesLookupTable.h> // Include the lookup tables

namespace BG {
namespace NES {
namespace Simulator {


// Define a mesh structure compatible with meshoptimizer
struct Mesh {
    std::vector<Geometries::Vec3D> vertices; // List of vertices
    std::vector<uint32_t> indices; // List of indices (triangles)
};

// Marching Cubes algorithm implementation
class MarchingCubes {
public:
    // Generate meshes for each neuron in the given region of the voxel array
    std::unordered_map<uint64_t, Mesh> GenerateMeshes(
        VoxelArray* voxelArray, // Pointer to the voxel array
        int startX, int startY, int startZ,
        int endX, int endY, int endZ,
        float isolevel = 0.5f
    );

private:
    // Linear interpolation for vertex positions
    Geometries::Vec3D InterpolateVertex(float isolevel, const Geometries::Vec3D& p1, const Geometries::Vec3D& p2, float val1, float val2);
};

} // namespace Simulator
} // namespace NES
} // namespace BG

#endif // MARCHING_CUBES_H