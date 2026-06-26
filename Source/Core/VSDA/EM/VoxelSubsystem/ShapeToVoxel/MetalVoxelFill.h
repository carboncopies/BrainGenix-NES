//=================================//
// This file is part of BrainGenix //
//=================================//

#pragma once

#ifdef __APPLE__
#include <cstdint>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

/**
 * Rasterize a sphere into the voxel array using a Metal compute kernel.
 *
 * cx/cy/cz:  sphere center in world space (same coordinate system as the voxel array).
 * radius:    sphere radius in micrometres.
 * parentUID: parent neuron UID written to every filled voxel.
 *
 * Returns true on success. On any Metal failure the caller must fall back to
 * the CPU FillSpherePart path.
 *
 * Thread-safe: a shared MTLDevice/MTLCommandQueue/MTLComputePipelineState are
 * initialised once; each call creates its own MTLCommandBuffer.
 */
bool Metal_FillSphere(VoxelArray* array,
                      float cx, float cy, float cz,
                      float radius, uint64_t parentUID);

#endif // __APPLE__
