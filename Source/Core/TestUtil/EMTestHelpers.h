//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Test-only helpers for the VSDA EM pipeline tests (dev_frame NT-14).
                 Header-only, compiled only into the test binary. Contains: polling, PNG/JPEG
                 read-back (stb_image; the stb implementations live in the library), a
                 neuroglancer compressed_segmentation decoder (the library only has the
                 encoder), a stub-venv builder for ProcessIgneousPipeline and builders for
                 ProcessingTask / VSDAData with every optional image effect switched off.
*/

#pragma once

#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <exception>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <stb_image.h>
#include <stb_image_write.h>

#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VSDAData.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

namespace BG {
namespace NES {
namespace TestUtil {

/**
 * Death-test children re-run the fixture SetUp (a fresh /tmp/nes-test-XXXXXX) and then leave through _exit or a
 * crash, so TearDown never runs. ArmChildCleanup(Root) makes the child remove its own root on std::terminate and on
 * SIGABRT/SIGFPE/SIGSEGV; ChildExit(code) removes it on the normal path. Call only inside a death-test statement.
 */
inline std::string& ChildRoot() {
    static std::string Root;
    return Root;
}
inline void ChildRemoveRoot() {
    std::error_code Ec;
    if (!ChildRoot().empty()) std::filesystem::remove_all(ChildRoot(), Ec);
}
[[noreturn]] inline void ChildExit(int Code) {
    ChildRemoveRoot();
    ::_exit(Code);
}
inline void ArmChildCleanup(const std::string& Root) {
    ChildRoot() = Root;
    std::set_terminate([] { ChildExit(134); });
    for (int Sig : {SIGABRT, SIGFPE, SIGSEGV}) std::signal(Sig, [](int S) { ChildExit(128 + S); });
}

/** Poll Pred every few ms until it is true or the timeout expires. Returns the final value of Pred. */
inline bool WaitFor(const std::function<bool()>& Pred, int Timeout_ms = 10000) {
    auto Deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(Timeout_ms);
    while (!Pred()) {
        if (std::chrono::steady_clock::now() > Deadline) return Pred();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return true;
}

inline std::string ReadFileBytes(const std::string& Path) {
    std::ifstream In(Path, std::ios::binary);
    std::ostringstream Ss;
    Ss << In.rdbuf();
    return Ss.str();
}

inline void WriteFileBytes(const std::string& Path, const std::string& Bytes) {
    std::filesystem::create_directories(std::filesystem::path(Path).parent_path());
    std::ofstream Out(Path, std::ios::binary);
    Out.write(Bytes.data(), Bytes.size());
}

/** Write a uniform grey PNG with the given channel count. */
inline void WriteGreyPng(const std::string& Path, int W, int H, uint8_t Value, int Channels = 1) {
    std::filesystem::create_directories(std::filesystem::path(Path).parent_path());
    std::vector<unsigned char> Px(size_t(W) * H * Channels, Value);
    stbi_write_png(Path.c_str(), W, H, Channels, Px.data(), W * Channels);
}

/** Decoded image (PNG or JPEG), via stbi_load with the file's own channel count. */
struct LoadedImage {
    int W = 0, H = 0, C = 0;
    std::vector<unsigned char> Px;
    bool Ok() const { return !Px.empty(); }
    unsigned char At(int X, int Y, int Ch = 0) const { return Px[(size_t(Y) * W + X) * C + Ch]; }
};

inline LoadedImage LoadImageFile(const std::string& Path) {
    LoadedImage Img;
    unsigned char* Data = stbi_load(Path.c_str(), &Img.W, &Img.H, &Img.C, 0);
    if (Data == nullptr) return Img;
    Img.Px.assign(Data, Data + size_t(Img.W) * Img.H * Img.C);
    stbi_image_free(Data);
    return Img;
}

/**
 * Decoder for the neuroglancer compressed_segmentation format as produced by
 * compress_segmentation::CompressChannels<uint64_t> with 2x2x2 blocks and one channel.
 * Input layout is that of SegmentationCompressor::CompressSegmentationRegion (x slowest in the source array;
 * the encoded stream itself is addressed per block, x fastest inside a block).
 */
inline std::vector<uint64_t> DecodeCompressedSegmentation(const std::vector<uint32_t>& Words, int SX, int SY, int SZ,
                                                           int Block = 2) {
    std::vector<uint64_t> Out(size_t(SX) * SY * SZ, ~uint64_t(0));  // index = (x*SY + y)*SZ + z
    if (Words.empty()) return Out;
    const size_t Base = Words[0];  // offset of channel 0
    const int GX = (SX + Block - 1) / Block, GY = (SY + Block - 1) / Block;
    for (int Z = 0; Z < SZ; Z++)
        for (int Y = 0; Y < SY; Y++)
            for (int X = 0; X < SX; X++) {
                const int BX = X / Block, BY = Y / Block, BZ = Z / Block;
                const size_t BlockIdx = BX + size_t(GX) * (BY + size_t(GY) * BZ);
                const uint32_t H0 = Words.at(Base + BlockIdx * 2), H1 = Words.at(Base + BlockIdx * 2 + 1);
                const size_t TableOff = H0 & 0xFFFFFF;
                const unsigned Bits = H0 >> 24;
                const size_t Off = (X % Block) + size_t(Block) * ((Y % Block) + size_t(Block) * (Z % Block));
                uint32_t Index = 0;
                if (Bits > 0) {
                    uint32_t Word = Words.at(Base + H1 + Off * Bits / 32);
                    Index = (Word >> (Off * Bits % 32)) & ((Bits >= 32) ? 0xFFFFFFFFu : ((1u << Bits) - 1));
                }
                const uint64_t Lo = Words.at(Base + TableOff + 2 * size_t(Index));
                const uint64_t Hi = Words.at(Base + TableOff + 2 * size_t(Index) + 1);
                Out[(size_t(X) * SY + Y) * SZ + Z] = Lo | (Hi << 32);
            }
    return Out;
}

/** Read a .seg file (raw uint32 words). */
inline std::vector<uint32_t> ReadSegWords(const std::string& Path) {
    std::string Bytes = ReadFileBytes(Path);
    std::vector<uint32_t> Words(Bytes.size() / 4);
    if (!Words.empty()) std::memcpy(Words.data(), Bytes.data(), Words.size() * 4);
    return Words;
}

/**
 * Creates <Dir>/bin/python as a /bin/sh script with the given body (an executable stub standing in
 * for a virtualenv interpreter). Returns Dir.
 */
inline std::string MakeStubVenv(const std::string& Dir, const std::string& ScriptBody) {
    std::filesystem::create_directories(Dir + "/bin");
    const std::string Py = Dir + "/bin/python";
    {
        std::ofstream Out(Py);
        Out << "#!/bin/sh\n" << ScriptBody << "\n";
    }
    ::chmod(Py.c_str(), 0755);
    return Dir;
}

/** MicroscopeParameters with every optional image effect off and a deterministic, flat render. */
inline Simulator::MicroscopeParameters MakePlainParams() {
    Simulator::MicroscopeParameters P{};
    P.GenerateSegmentation = false;
    P.GenerateSegmentationPNGs = false;
    P.GenerateMeshes = false;
    P.VoxelResolution_um = 1.0f;
    P.ImageWidth_px = 4;
    P.ImageHeight_px = 4;
    P.ScanRegionOverlap_percent = 0.0f;
    P.SliceThickness_um = 1.0f;
    P.MicroscopeFOV_deg = 0.0f;
    P.NumPixelsPerVoxel_px = 1;
    P.GeneratePerlinNoise_ = false;
    P.DefaultIntensity_ = 200;
    P.RenderBorders = false;
    P.GenerateImageNoise = false;
    P.EnableGaussianBlur = false;
    P.EnableInterferencePattern = false;
    P.AdjustContrast = false;
    P.TearingEnabled = false;
    P.TearNumPerSlice = 0;
    P.TearNumVariation = 0;
    return P;
}

/** A ProcessingTask (image or segmentation) with every optional effect off. */
inline void ConfigurePlainTask(Simulator::ProcessingTask& T, Simulator::VoxelArray* Array, Simulator::MicroscopeParameters* Params,
                               int X0, int X1, int Y0, int Y1, int Z, int OutW, int OutH) {
    T.Array_ = Array;
    T.Params_ = Params;
    T.IsSegmentation_ = false;
    T.Width_px = OutW;
    T.Height_px = OutH;
    T.VoxelStartingX = X0;
    T.VoxelEndingX = X1;
    T.VoxelStartingY = Y0;
    T.VoxelEndingY = Y1;
    T.VoxelZ = Z;
    T.SliceThickness_vox = 1;
    T.VoxelScale_um = 1.0f;
    T.EnableImageNoise = false;
    T.ImageNoiseAmount = 1;
    T.PreBlurNoisePasses = 0;
    T.PostBlurNoisePasses = 0;
    T.EnableGaussianBlur = false;
    T.GaussianBlurSigma = 1.0f;
    T.EnableInterferencePattern = false;
    T.AdjustContrast = false;
}

}; // namespace TestUtil
}; // namespace NES
}; // namespace BG
