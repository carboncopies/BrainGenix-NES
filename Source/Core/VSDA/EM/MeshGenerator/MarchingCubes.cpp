#include <VSDA/EM/MeshGenerator/MarchingCubes.h>

namespace BG {
namespace NES {
namespace Simulator {

// Linear interpolation for vertex positions
Geometries::Vec3D MarchingCubes::InterpolateVertex(float isolevel, const Geometries::Vec3D& p1, const Geometries::Vec3D& p2, float val1, float val2) {
    float mu = (isolevel - val1) / (val2 - val1);
    return {
        p1.x + mu * (p2.x - p1.x),
        p1.y + mu * (p2.y - p1.y),
        p1.z + mu * (p2.z - p1.z)
    };
}

// Generate meshes for each neuron in the given region of the voxel array
std::unordered_map<uint64_t, Mesh> MarchingCubes::GenerateMeshes(
    VoxelArray* voxelArray, // Pointer to the voxel array
    int startX, int startY, int startZ,
    int endX, int endY, int endZ,
    float isolevel
) {
    std::unordered_map<uint64_t, Mesh> neuronMeshes; // Maps ParentUID to its mesh

    // Iterate over the specified region of the voxel array
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            for (int z = startZ; z < endZ; ++z) {
                // Get the 8 voxels forming the current cube
                std::array<VoxelType, 8> cubeVoxels = {
                    voxelArray->GetVoxel(x, y, z),
                    voxelArray->GetVoxel(x + 1, y, z),
                    voxelArray->GetVoxel(x + 1, y, z + 1),
                    voxelArray->GetVoxel(x, y, z + 1),
                    voxelArray->GetVoxel(x, y + 1, z),
                    voxelArray->GetVoxel(x + 1, y + 1, z),
                    voxelArray->GetVoxel(x + 1, y + 1, z + 1),
                    voxelArray->GetVoxel(x, y + 1, z + 1)
                };

                // Determine the cube index
                int cubeIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    if (cubeVoxels[i].State_ == VoxelState_INTERIOR || cubeVoxels[i].State_ == VoxelState_BORDER) {
                        cubeIndex |= (1 << i);
                    }
                }

                // Skip empty cubes
                if (cubeIndex == 0 || cubeIndex == 255) continue;

                // Generate triangles using the TriangleTable
                for (int i = 0; TriangleTable[cubeIndex][i] != -1; i += 3) {
                    int edge1 = TriangleTable[cubeIndex][i];
                    int edge2 = TriangleTable[cubeIndex][i + 1];
                    int edge3 = TriangleTable[cubeIndex][i + 2];

                    // Get the vertices for the triangle
                    Geometries::Vec3D v1, v2, v3;

                    // Edge 1
                    int v1a = EdgeVertexIndices[edge1][0];
                    int v1b = EdgeVertexIndices[edge1][1];
                    v1 = InterpolateVertex(
                        isolevel,
                        voxelArray->GetPositionAtIndex(x + (v1a & 1), y + ((v1a >> 1) & 1), z + ((v1a >> 2) & 1)),
                        voxelArray->GetPositionAtIndex(x + (v1b & 1), y + ((v1b >> 1) & 1), z + ((v1b >> 2) & 1)),
                        cubeVoxels[v1a].DistanceToEdge_vox_,
                        cubeVoxels[v1b].DistanceToEdge_vox_
                    );

                    // Edge 2
                    int v2a = EdgeVertexIndices[edge2][0];
                    int v2b = EdgeVertexIndices[edge2][1];
                    v2 = InterpolateVertex(
                        isolevel,
                        voxelArray->GetPositionAtIndex(x + (v2a & 1), y + ((v2a >> 1) & 1), z + ((v2a >> 2) & 1)),
                        voxelArray->GetPositionAtIndex(x + (v2b & 1), y + ((v2b >> 1) & 1), z + ((v2b >> 2) & 1)),
                        cubeVoxels[v2a].DistanceToEdge_vox_,
                        cubeVoxels[v2b].DistanceToEdge_vox_
                    );

                    // Edge 3
                    int v3a = EdgeVertexIndices[edge3][0];
                    int v3b = EdgeVertexIndices[edge3][1];
                    v3 = InterpolateVertex(
                        isolevel,
                        voxelArray->GetPositionAtIndex(x + (v3a & 1), y + ((v3a >> 1) & 1), z + ((v3a >> 2) & 1)),
                        voxelArray->GetPositionAtIndex(x + (v3b & 1), y + ((v3b >> 1) & 1), z + ((v3b >> 2) & 1)),
                        cubeVoxels[v3a].DistanceToEdge_vox_,
                        cubeVoxels[v3b].DistanceToEdge_vox_
                    );

                    // Determine the ParentUID for this triangle
                    uint64_t parentUID = cubeVoxels[0].ParentUID; // Use the first voxel's ParentUID

                    // Add the triangle to the corresponding mesh
                    Mesh& mesh = neuronMeshes[parentUID];
                    uint32_t baseIndex = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(v1);
                    mesh.vertices.push_back(v2);
                    mesh.vertices.push_back(v3);
                    mesh.indices.push_back(baseIndex);
                    mesh.indices.push_back(baseIndex + 1);
                    mesh.indices.push_back(baseIndex + 2);
                }
            }
        }
    }

    return neuronMeshes;
}

} // namespace Simulator
} // namespace NES
} // namespace BG