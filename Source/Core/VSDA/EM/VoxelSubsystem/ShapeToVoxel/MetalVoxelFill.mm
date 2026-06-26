//=================================//
// This file is part of BrainGenix //
//=================================//

#ifdef __APPLE__

#include <mutex>
#include <cstdint>

#import <Metal/Metal.h>

#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/MetalVoxelFill.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>


// ── Metal Shading Language source (compiled at first call, cached by Metal) ─────

static const char* kShaderSource = R"MSL(
#include <metal_stdlib>
using namespace metal;

// Must mirror VoxelType in VoxelArray.h (sizeof = 16 on all platforms):
//   uint8_t State_               offset 0
//   uint8_t DistanceToEdge_vox_  offset 1
//   [6 bytes padding]            offsets 2-7
//   uint64_t ParentUID           offset 8
struct VoxelType {
    uchar state;        // VoxelState_EMPTY=0, VoxelState_INTERIOR=1, ...
    uchar distToEdge;
    uchar _pad[6];
    ulong parentUID;
};

struct SphereParams {
    float cx, cy, cz;             // sphere centre (world space, um)
    float radius;
    float voxelScale;             // um per voxel
    float bbOriginX, bbOriginY, bbOriginZ; // voxel array bounding-box origin
    int   bbStartX, bbStartY, bbStartZ;   // bounding-box of sphere in voxel indices
    int   bbEndX,   bbEndY,   bbEndZ;
    int   sizeY, sizeZ;           // full array dimensions (for flat index calc)
    ulong parentUID;
};

kernel void fillSphere(
    device VoxelType*      voxels [[buffer(0)]],
    constant SphereParams& p      [[buffer(1)]],
    uint3 gid [[thread_position_in_grid]])
{
    int xi = p.bbStartX + (int)gid.x;
    int yi = p.bbStartY + (int)gid.y;
    int zi = p.bbStartZ + (int)gid.z;

    // Guard: threads beyond bounding-box edge (grid is rounded up to threadgroup size)
    if (xi >= p.bbEndX || yi >= p.bbEndY || zi >= p.bbEndZ) return;

    // Reconstruct world-space position for this voxel
    float wx = p.bbOriginX + (float)xi * p.voxelScale;
    float wy = p.bbOriginY + (float)yi * p.voxelScale;
    float wz = p.bbOriginZ + (float)zi * p.voxelScale;

    float dx = wx - p.cx;
    float dy = wy - p.cy;
    float dz = wz - p.cz;
    float dist = sqrt(dx*dx + dy*dy + dz*dz);

    if (dist > p.radius) return;

    ulong idx = ulong(xi) * (ulong(p.sizeY) * ulong(p.sizeZ))
              + ulong(yi) * ulong(p.sizeZ)
              + ulong(zi);

    // Distance to sphere surface (mirrors CompositeVoxelAtIndex logic)
    float distToEdge_um = p.radius - dist;
    uchar distVox = (uchar)min(255.0f, distToEdge_um / p.voxelScale);

    // Benign concurrent write: mirrors existing CPU concurrent-write behaviour.
    // Keep the higher distToEdge; set state to INTERIOR (1) if not already BLACK/WHITE.
    device VoxelType& v = voxels[idx];
    if (v.distToEdge < distVox) v.distToEdge = distVox;
    if (v.state < 3) v.state = 1;   // VoxelState_INTERIOR = 1, VoxelState_BLACK = 3
    v.parentUID = p.parentUID;
}
)MSL";


// ── Shared Metal state ───────────────────────────────────────────────────────

static id<MTLDevice>               gDevice   = nil;
static id<MTLCommandQueue>         gQueue    = nil;
static id<MTLComputePipelineState> gSphere   = nil;
static std::once_flag              gInitFlag;
static bool                        gInitOk   = false;

static void InitMetal() {
    std::call_once(gInitFlag, [](){
        gDevice = MTLCreateSystemDefaultDevice();
        if (!gDevice) return;

        gQueue = [gDevice newCommandQueue];
        if (!gQueue) return;

        NSError* err = nil;
        MTLCompileOptions* opts = [MTLCompileOptions new];
        id<MTLLibrary> lib = [gDevice newLibraryWithSource:@(kShaderSource)
                                                   options:opts
                                                     error:&err];
        if (!lib) return;

        id<MTLFunction> fn = [lib newFunctionWithName:@"fillSphere"];
        if (!fn) return;

        gSphere = [gDevice newComputePipelineStateWithFunction:fn error:&err];
        gInitOk = (gSphere != nil);
    });
}


// ── C++ bridge struct (must match SphereParams in the shader exactly) ────────

struct alignas(4) SphereKernelParams {
    float    cx, cy, cz;
    float    radius;
    float    voxelScale;
    float    bbOriginX, bbOriginY, bbOriginZ;
    int      bbStartX, bbStartY, bbStartZ;
    int      bbEndX,   bbEndY,   bbEndZ;
    int      sizeY, sizeZ;
    uint64_t parentUID;   // at offset 64, naturally 8-byte aligned
};


// ── Public entry point ───────────────────────────────────────────────────────

bool Metal_FillSphere(VoxelArray* array,
                      float cx, float cy, float cz,
                      float radius, uint64_t parentUID) {
    InitMetal();
    if (!gInitOk) return false;

    // Gather array geometry
    BoundingBox bb  = array->GetBoundingBox();
    float voxScale  = array->GetResolution();
    int   sizeX     = array->GetX();
    int   sizeY     = array->GetY();
    int   sizeZ     = array->GetZ();
    uint64_t total  = array->GetSize();

    VoxelType* rawPtr = array->GetRawData();

    // Require page-aligned backing memory for zero-copy Metal buffer.
    // Falls back gracefully if the array was not allocated with posix_memalign.
    if ((reinterpret_cast<uintptr_t>(rawPtr) & 4095) != 0) return false;

    // Compute bounding-box voxel indices for this sphere (clamped to array bounds)
    auto toIdx = [&](float world, float origin, int maxDim) -> int {
        return std::max(0, std::min(maxDim - 1, (int)std::round((world - origin) / voxScale)));
    };

    int bbStartX = toIdx(cx - radius, bb.bb_point1[0], sizeX);
    int bbStartY = toIdx(cy - radius, bb.bb_point1[1], sizeY);
    int bbStartZ = toIdx(cz - radius, bb.bb_point1[2], sizeZ);
    int bbEndX   = toIdx(cx + radius, bb.bb_point1[0], sizeX) + 1;
    int bbEndY   = toIdx(cy + radius, bb.bb_point1[1], sizeY) + 1;
    int bbEndZ   = toIdx(cz + radius, bb.bb_point1[2], sizeZ) + 1;

    int dispX = std::max(1, bbEndX - bbStartX);
    int dispY = std::max(1, bbEndY - bbStartY);
    int dispZ = std::max(1, bbEndZ - bbStartZ);

    __block bool success = false;

    @autoreleasepool {

        // Zero-copy buffer: the voxel array lives in Unified Memory on Apple Silicon.
        // The GPU accesses the exact same physical pages as the CPU — no DMA transfer.
        size_t byteLen = (size_t)total * sizeof(VoxelType);
        id<MTLBuffer> voxelBuf = [gDevice newBufferWithBytesNoCopy:rawPtr
                                                            length:byteLen
                                                           options:MTLResourceStorageModeShared
                                                       deallocator:nil];
        if (!voxelBuf) return;

        // Fill params struct
        SphereKernelParams p{};
        p.cx = cx;  p.cy = cy;  p.cz = cz;
        p.radius     = radius;
        p.voxelScale = voxScale;
        p.bbOriginX  = bb.bb_point1[0];
        p.bbOriginY  = bb.bb_point1[1];
        p.bbOriginZ  = bb.bb_point1[2];
        p.bbStartX   = bbStartX;  p.bbStartY = bbStartY;  p.bbStartZ = bbStartZ;
        p.bbEndX     = bbEndX;    p.bbEndY   = bbEndY;    p.bbEndZ   = bbEndZ;
        p.sizeY      = sizeY;
        p.sizeZ      = sizeZ;
        p.parentUID  = parentUID;

        id<MTLBuffer> paramBuf = [gDevice newBufferWithBytes:&p
                                                      length:sizeof(p)
                                                     options:MTLResourceStorageModeShared];
        if (!paramBuf) return;

        // Encode and dispatch
        id<MTLCommandBuffer>         cmdBuf  = [gQueue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];

        [encoder setComputePipelineState:gSphere];
        [encoder setBuffer:voxelBuf  offset:0 atIndex:0];
        [encoder setBuffer:paramBuf  offset:0 atIndex:1];

        // Threadgroup: 8×8×4 = 256 threads — fits M-series GPU SIMD width.
        MTLSize tg   = MTLSizeMake(8, 8, 4);
        MTLSize grid = MTLSizeMake((NSUInteger)dispX, (NSUInteger)dispY, (NSUInteger)dispZ);
        [encoder dispatchThreads:grid threadsPerThreadgroup:tg];

        [encoder endEncoding];
        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];

        success = (cmdBuf.status == MTLCommandBufferStatusCompleted);
    }

    return success;
}

#endif // __APPLE__
