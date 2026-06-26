//=================================//
// This file is part of BrainGenix //
//=================================//

#ifdef __APPLE__

#include <mutex>
#include <vector>
#include <cstring>
#include <algorithm>

#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <Accelerate/Accelerate.h>

#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/AppleGaussianBlur.h>


// Shared Metal state — MTLDevice and MTLCommandQueue are documented thread-safe.
// Initialized once at first call; never torn down (process lifetime).
static id<MTLDevice>       gDevice = nil;
static id<MTLCommandQueue> gQueue  = nil;
static std::once_flag      gInitFlag;

static void InitMetal() {
    std::call_once(gInitFlag, [](){
        gDevice = MTLCreateSystemDefaultDevice();
        if (gDevice) {
            gQueue = [gDevice newCommandQueue];
        }
    });
}


bool MPS_GaussianBlur(uint8_t* pixels, int width, int height, float sigma) {
    InitMetal();
    if (!gDevice || !gQueue) return false;

    __block bool Success = false;

    @autoreleasepool {

        // MTLStorageModeShared: on Apple Silicon the texture lives in Unified Memory,
        // so replaceRegion/getBytes are in-place CPU writes with no DMA transfer.
        MTLTextureDescriptor* Desc = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatR8Unorm
                                         width:(NSUInteger)width
                                        height:(NSUInteger)height
                                     mipmapped:NO];
        Desc.usage       = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
        Desc.storageMode = MTLStorageModeShared;

        id<MTLTexture> SrcTex = [gDevice newTextureWithDescriptor:Desc];
        id<MTLTexture> DstTex = [gDevice newTextureWithDescriptor:Desc];
        if (!SrcTex || !DstTex) return false;

        // Upload source pixels
        [SrcTex replaceRegion:MTLRegionMake2D(0, 0, (NSUInteger)width, (NSUInteger)height)
                  mipmapLevel:0
                    withBytes:pixels
                  bytesPerRow:(NSUInteger)width];

        // Encode MPS Gaussian blur
        MPSImageGaussianBlur* Blur = [[MPSImageGaussianBlur alloc]
            initWithDevice:gDevice sigma:sigma];

        id<MTLCommandBuffer> CmdBuf = [gQueue commandBuffer];
        [Blur encodeToCommandBuffer:CmdBuf
                      sourceTexture:SrcTex
                 destinationTexture:DstTex];
        [CmdBuf commit];
        [CmdBuf waitUntilCompleted];

        if (CmdBuf.status == MTLCommandBufferStatusError) return false;

        // Read blurred pixels back (still in unified memory — no physical transfer)
        [DstTex getBytes:pixels
             bytesPerRow:(NSUInteger)width
              fromRegion:MTLRegionMake2D(0, 0, (NSUInteger)width, (NSUInteger)height)
             mipmapLevel:0];

        Success = true;
    }

    return Success;
}


bool Accelerate_GaussianBlur(uint8_t* pixels, int width, int height, float sigma) {
    vImagePixelCount W = (vImagePixelCount)width;
    vImagePixelCount H = (vImagePixelCount)height;
    std::vector<uint8_t> TmpBuf((size_t)W * H);
    vImage_Buffer Src = { pixels,         H, W, (size_t)W };
    vImage_Buffer Dst = { TmpBuf.data(),  H, W, (size_t)W };
    uint32_t Ks = std::max(3u, (uint32_t)((uint32_t)(sigma * 4 + 1) | 1));
    vImageTentConvolve_Planar8(&Src, &Dst, nullptr, 0, 0, Ks, Ks, 0, kvImageEdgeExtend);
    std::memcpy(pixels, TmpBuf.data(), (size_t)W * H);
    return true;
}

#endif // __APPLE__
