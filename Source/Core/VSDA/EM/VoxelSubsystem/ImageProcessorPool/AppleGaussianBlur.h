//=================================//
// This file is part of BrainGenix //
//=================================//

#pragma once

#ifdef __APPLE__
#include <cstdint>

/**
 * Apply a Gaussian blur via Metal Performance Shaders (GPU).
 * pixels: grayscale single-channel uint8 buffer, modified in-place.
 * Returns true on success; caller should fall back to vImage on failure.
 * Thread-safe: shares a single MTLDevice/MTLCommandQueue across all callers.
 */
bool MPS_GaussianBlur(uint8_t* pixels, int width, int height, float sigma);

#endif // __APPLE__
