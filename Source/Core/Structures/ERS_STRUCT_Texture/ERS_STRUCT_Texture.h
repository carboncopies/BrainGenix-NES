//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <FreeImage.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_TextureLevel.h>

// Structure To Hold 2D ERS Texture
struct ERS_STRUCT_Texture {

    // New Loading System Info
    std::vector<ERS_STRUCT_TextureLevel> TextureLevels; /**<Levels in this texture*/

    unsigned int BestAvailableOpenGLID = 0; /**<The current best ID we have loaded*/
    bool HasAnyLevelReady = false; /**<Tells the mesh renderer if we have any levels ready for it*/
    bool IsBeingUsed = false; /**<Prevents modifications when this is set to true. For Example, would stop the ram data being unloaded while the gpu is trying to access it.*/

    int TextureLevelInRAM_ = 0; /**<Determines the current texture level in RAM*/
    int TextureLevelInVRAM_ = 0; /**<Determines the current texture level in RAM*/

    int NumberChannels; /**<Shows the number of color channels in this texture*/

    // Set ImageData For Deferred OpenGL Loading
    // float Channels;
    // float Width;
    // float Height;
    // FIBITMAP* ImageData;
    // unsigned char* ImageBytes;
    // bool FreeImageBackend = true;
    bool HasImageData = false;

    // Set Metadata
    std::string Type;
    std::string Path;

};
