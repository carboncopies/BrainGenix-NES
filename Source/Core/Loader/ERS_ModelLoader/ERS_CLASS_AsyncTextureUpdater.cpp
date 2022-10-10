//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_AsyncTextureUpdater.h>


ERS_CLASS_AsyncTextureUpdater::ERS_CLASS_AsyncTextureUpdater(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_AssetStreamingSystemResourceMonitor* ResourceMonitor, GLFWwindow* Window, unsigned int Threads) {

    SystemUtils_ = SystemUtils;
    MainThreadWindowContext_ = Window;
    ResourceMonitor_ = ResourceMonitor;
    SystemUtils_->Logger_->Log("Initializing Automatic Texture Loading Subsystem", 5);

    // If Threads Is Left To Autodetect, Use That
    if (Threads <= 0) {

        // Check If Config Has Param
        if ((*SystemUtils_->LocalSystemConfiguration_)["TextureLoaderThreadCount"]) {
            SystemUtils_->Logger_->Log("Using Config File To Set Number Of Texture Loader Threads", 4);
            Threads = (*SystemUtils_->LocalSystemConfiguration_)["TextureLoaderThreadCount"].as<int>();
        } else {
            SystemUtils_->Logger_->Log("Autodetecting Number Of Threads To Use", 4);
            Threads = std::thread::hardware_concurrency() - 2;
            if (Threads < 2) {
                Threads = 2;
                SystemUtils_->Logger_->Log("Less Than Two CPUs Detected, Will Use Two Threads Regardless, However Frame Drops May Happen", 6);
            }
        }
    }

    SetNumLoaderThreads(Threads);
    SetNumStreamerThreads(1);
    SetupPusherThreads();
    SetupLoaderThreads();

}
ERS_CLASS_AsyncTextureUpdater::~ERS_CLASS_AsyncTextureUpdater() {

    SystemUtils_->Logger_->Log("Automatic Texture Loading Subsystem Shutdown Invoked", 6);


    TeardownPusherThreads();
    TeardownLoaderThreads();

    // Cleanup
    SystemUtils_->Logger_->Log("Cleaning Up OpenGL/GLFW", 6);
    glfwTerminate();

}


// Texture Streaming Helpers
void ERS_CLASS_AsyncTextureUpdater::FreeRAMAllocation(ERS_STRUCT_TextureLevel &Level) {
    if (Level.AllocatedRAMBudget) {
        ResourceMonitor_->DeallocateTextureRAMFromBudget(Level.LevelMemorySizeBytes);
        Level.AllocatedRAMBudget = false;
    }
}
void ERS_CLASS_AsyncTextureUpdater::FreeVRAMAllocation(ERS_STRUCT_TextureLevel &Level) {
    if (Level.AllocatedVRAMBudget) {
        ResourceMonitor_->DeallocateTextureVRAMFromBudget(Level.LevelMemorySizeBytes);
        Level.AllocatedVRAMBudget = false;
    }
}
bool ERS_CLASS_AsyncTextureUpdater::LoadImageDataRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable) {

    // Check If Requested Level Exists
    long long unsigned int MemoryFree = SystemUtils_->RendererSettings_->RAMBudget_ - SystemUtils_->RendererSettings_->CurrentRAMUsage_;
    if (Level < 0) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Load Negative Texture Level", 8, LogEnable);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    } else if (Level > (int)Texture->TextureLevels.size()) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Load Nonexistant Texture Level", 8, LogEnable);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    } else if (!ResourceMonitor_->TextureFitsInRAMBudget(Texture->TextureLevels[Level].LevelMemorySizeBytes)) {
        SystemUtils_->Logger_->Log("Cannot Load Texture Into Memory, Will Not Fit In RAM Budget", 6);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    } else if (MemoryFree < MinRAMCutoff_) {
        SystemUtils_->Logger_->Log("Not Enough Free RAM To Load Texture", 9);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }

    // Check If Level Already Loaded
    if (Texture->TextureLevels[Level].LevelBitmap != nullptr) {
        return false;
    }


    // If Needed, Allocate Memory From Budget
    if (!Texture->TextureLevels[Level].AllocatedRAMBudget) {
        ResourceMonitor_->AllocateTextureRAMFromBudget(Texture->TextureLevels[Level].LevelMemorySizeBytes);
        Texture->TextureLevels[Level].AllocatedRAMBudget = true;
    }

    // Load Image Data
    ERS_STRUCT_IOData ImageData;
    long LevelAssetID = Texture->TextureLevels[Level].LevelTextureAssetID;
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(LevelAssetID, &ImageData);

    // Decode Image
    FIMEMORY* FIImageData = FreeImage_OpenMemory(ImageData.Data.get(), ImageData.Size_B);
    FREE_IMAGE_FORMAT Format = FreeImage_GetFileTypeFromMemory(FIImageData);
    FIBITMAP* RawImage = FreeImage_LoadFromMemory(Format, FIImageData);


    // Detect Width/Height/memsize
    int Width = FreeImage_GetWidth(RawImage);
    int Height = FreeImage_GetHeight(RawImage);
    if (Width <= 0) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Width Is <1", 8, LogEnable);
        FreeImage_Unload(RawImage);
        FreeImage_CloseMemory(FIImageData);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }
    if (Height <= 0) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Height Is <1", 8, LogEnable);
        FreeImage_Unload(RawImage);
        FreeImage_CloseMemory(FIImageData);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }


    // Force The Image Into A Constant Number Of Channels
    FIBITMAP* Image = nullptr;
    int Channels = FreeImage_GetLine(RawImage) / FreeImage_GetWidth(RawImage);
    if (Channels == 1) {
        Image = FreeImage_ConvertTo8Bits(RawImage);
    } else if (Channels == 2) {
        Image = FreeImage_ConvertTo16Bits555(RawImage);
    } else if (Channels == 3) {
        Image = FreeImage_ConvertTo24Bits(RawImage);
    } else if (Channels == 4) {
        Image = FreeImage_ConvertTo32Bits(RawImage);
    }
    FreeImage_Unload(RawImage);
    FreeImage_CloseMemory(FIImageData);

    std::pair<int, int> TargetWidthHeight = Texture->TextureLevels[Level].LevelResolution;
    if ((TargetWidthHeight.first != Width) && (TargetWidthHeight.first != -1)) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Width Does Not Match Metadata Target", 8, LogEnable);
        FreeImage_Unload(Image);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }
    if ((TargetWidthHeight.second != Height) && (TargetWidthHeight.second != -1)) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Height Does Not Match Metadata Target", 8, LogEnable);
        FreeImage_Unload(Image);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }

    // Detect NumChannels
    if ((Channels < 1) || (Channels > 4)) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Invalid Number Of Channels '" + std::to_string(Channels) + "'", 8, LogEnable);
        FreeImage_Unload(Image);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }
    if ((Texture->TextureLevels[Level].LevelChannel != Channels) && (Texture->TextureLevels[Level].LevelChannel != -1)) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(LevelAssetID)
        + "' Number Channels '" + std::to_string(Channels) + "' Does Not Match Metadata Target '"
        + std::to_string(Texture->TextureLevels[Level].LevelChannel) + "'", 8, LogEnable);
        FreeImage_Unload(Image);
        FreeRAMAllocation(Texture->TextureLevels[Level]);
        return false;
    }

    // Finally After Passing Sanity Checks, Populate Info.
    Texture->TextureLevels[Level].LevelBitmap = Image;
    Texture->TextureLevels[Level].LevelLoadedInRAM = true;

    return true;

}
bool ERS_CLASS_AsyncTextureUpdater::UnloadImageDataRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable) {

    // Check If Requested Level Exists
    if (Level < 0) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Negative Texture Level", 8, LogEnable);
        return false;
    } else if (Level > (int)Texture->TextureLevels.size()) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Nonexistant Texture Level", 8, LogEnable);
        return false;
    }

    // Check If Level Already Not Loaded
    if (Texture->TextureLevels[Level].LevelBitmap == nullptr) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Not Loaded Image", 8, LogEnable);
        return false;
    }

    // Ensure Texture Is Free
    if (Texture->IsBeingUsed) {
        return false;
    }
    Texture->IsBeingUsed = true;

    FreeRAMAllocation(Texture->TextureLevels[Level]);

    // Update Data
    FreeImage_Unload(Texture->TextureLevels[Level].LevelBitmap);
    Texture->TextureLevels[Level].LevelBitmap = nullptr;
    Texture->TextureLevels[Level].LevelLoadedInRAM = false;
    Texture->IsBeingUsed = false;

    return true;
}
bool ERS_CLASS_AsyncTextureUpdater::LoadImageDataVRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable) {

    // Ensure Texture Is Free
    if (Texture->IsBeingUsed) {
        return false;
    }
    Texture->IsBeingUsed = true;

    // Check If Requested Level Exists
    long long unsigned int MemoryFree = SystemUtils_->RendererSettings_->VRAMBudget_ - SystemUtils_->RendererSettings_->CurrentVRAMUsage_;
    if (Level < 0) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Load Negative Texture Level Into VRAM", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    } else if (Level > (int)Texture->TextureLevels.size()) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Load Nonexistant Texture Level Into VRAM", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    } else if (!ResourceMonitor_->TextureFitsInVRAMBudget(Texture->TextureLevels[Level].LevelMemorySizeBytes)) {
        SystemUtils_->Logger_->Log("Cannot Load Texture Into Memory, Will Not Fit In VRAM Budget", 6);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    } else if ((Texture->TextureLevels[Level].LevelTextureOpenGLID != 0)) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Load Already Loaded Image Into VRAM", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    } else if (MemoryFree < MinVRAMCutoff_) {
        SystemUtils_->Logger_->Log("Not Enough Free VRAM To Push Texture", 9);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    } else if (MemoryFree < MinRAMCutoff_) {
        SystemUtils_->Logger_->Log("Not Enough Free RAM To Load Texture", 9);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }


    // Allocate Budget
    if (!Texture->TextureLevels[Level].AllocatedVRAMBudget) {
        ResourceMonitor_->AllocateTextureVRAMFromBudget(Texture->TextureLevels[Level].LevelMemorySizeBytes);
        Texture->TextureLevels[Level].AllocatedVRAMBudget = true;
    }

    // Get Image Metadata, Perform Checks
    int MaxLevel = Texture->TextureLevels.size() - 1;
    int CorrectedIndex = Level;

    // Check that Pointer Is Valid
    if (Texture->TextureLevels[CorrectedIndex].LevelBitmap == nullptr) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Image Data Is Null", 9, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }

    int MaxWidth = Texture->TextureLevels[CorrectedIndex].LevelResolution.first;
    int MaxHeight = Texture->TextureLevels[CorrectedIndex].LevelResolution.second;
    int ImageSize = FreeImage_GetMemorySize(Texture->TextureLevels[CorrectedIndex].LevelBitmap);//Texture->LevelMemorySizeBytes[MaxLevel - Level];
    int Channels = Texture->TextureLevels[CorrectedIndex].LevelChannel;
    if (Channels > 4) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Channel Count >4", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    if (Channels < 1) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Channel Count <1", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    if (!Texture->TextureLevels[Level].LevelLoadedInRAM) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Not All Prior Levels Are Loaded Into RAM", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    if (MaxLevel < 0) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' No Levels To Load", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    if (MaxWidth < 1) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Width is 0", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false; 
    }
    if (MaxHeight < 1) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Height is 0", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    if (ImageSize < 1) {
        SystemUtils_->Logger_->Log(std::string("Error Loading Texture '") + Texture->Path
        + "', Level '" + std::to_string(Level) + "' With ID '" + std::to_string(Texture->TextureLevels[CorrectedIndex].LevelTextureAssetID)
        + "' Image Byte Array Has Size Of 0", 8, LogEnable);
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }

    // Setup PBO
    unsigned int PBOID;
    glGenBuffers(1, &PBOID);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOID);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, ImageSize, 0, GL_STREAM_DRAW);


    // Generate OpenGL Texture ID
    unsigned int OpenGLTextureID;
    glGenTextures(1, &OpenGLTextureID);
    glBindTexture(GL_TEXTURE_2D, OpenGLTextureID);

    // Identify Required Texture Format
    GLint TextureInternFormat;
    GLenum TextureExternFormat;
    if (Channels == 4) {
        TextureInternFormat = GL_RGBA;
        TextureExternFormat = GL_RGBA;
    } else if (Channels == 3) {
        TextureInternFormat = GL_RGB;
        TextureExternFormat = GL_RGB;
    } else if (Channels == 2) {
        TextureInternFormat = GL_RG;
        TextureExternFormat = GL_RG;
    } else if (Channels == 1) {
        TextureInternFormat = GL_RED;
        TextureExternFormat = GL_RED;
    } else {
        FreeVRAMAllocation(Texture->TextureLevels[Level]);
        Texture->IsBeingUsed = false;
        return false;
    }
    
    // Generate Texture
    glTexImage2D(GL_TEXTURE_2D, 0, TextureInternFormat, MaxWidth, MaxHeight, 0, TextureExternFormat, GL_UNSIGNED_BYTE, 0);

    // Set Texture Properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int Width = Texture->TextureLevels[Level].LevelResolution.first;
    int Height = Texture->TextureLevels[Level].LevelResolution.second;

    unsigned char* LevelImageBytes = (unsigned char*)FreeImage_GetBits(Texture->TextureLevels[Level].LevelBitmap);
    int ImageLevelSize = Width*Height*Channels*sizeof(unsigned char);

    //glBufferData(GL_PIXEL_UNPACK_BUFFER, LevelImageSize, 0, GL_STREAM_DRAW);
    GLubyte* PBOPointer = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    if (PBOPointer != nullptr) {
        ::memcpy(PBOPointer, LevelImageBytes, ImageLevelSize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    } else {
        SystemUtils_->Logger_->Log("Error Mapping PBO, glMapBuffer Returned Nullptr", 8);
    }
    glFinish();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, TextureExternFormat, GL_UNSIGNED_BYTE, 0);
    

    // Cleanup Buffers, Wait For Everything To Finish
    glFinish();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDeleteBuffers(1, &PBOID);

    // Update Struct
    Texture->TextureLevels[CorrectedIndex].LevelTextureOpenGLID = OpenGLTextureID;
    Texture->TextureLevels[CorrectedIndex].LevelLoadedInVRAM = true;
    Texture->IsBeingUsed = false;
    return true;
}
bool ERS_CLASS_AsyncTextureUpdater::UnloadImageDataVRAM(ERS_STRUCT_Texture* Texture, int Level, bool LogEnable) {
    
    // Check If Requested Level Exists
    if (Level < 0) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Negative Texture Level Out Of VRAM", 8, LogEnable);
        return false;
    } else if (Level > (int)Texture->TextureLevels.size()) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Nonexistant Texture Level Out Of VRAM", 8, LogEnable);
        return false;
    }

    // Check If Level Already Unloaded
    if ((Texture->TextureLevels[Level].LevelTextureOpenGLID == 0)) {
        SystemUtils_->Logger_->Log("Texture Updater Tried To Unload Already Loaded Image Out Of VRAM", 8, LogEnable);
        return false;
    }

    // Delete Texture
    glDeleteTextures(1, &Texture->TextureLevels[Level].LevelTextureOpenGLID);

    // Update Struct
    // long MemorySize = Texture->TextureLevels[Level].LevelMemorySizeBytes;
    FreeVRAMAllocation(Texture->TextureLevels[Level]);
    Texture->TextureLevels[Level].LevelTextureOpenGLID = 0;
    Texture->TextureLevels[Level].LevelLoadedInVRAM = false;

    return true;

}
void ERS_CLASS_AsyncTextureUpdater::SetLevelRAM(ERS_STRUCT_Model* Model, bool LogEnable) {

        // NOTE: RAM UPDATES MUST BE CONSECUATIVE (you must have every level loaded consecuitively, eg: if you have level 3 loaded, you must also have 0,1,2 as well)
        // Also apparenly I don't know how to spell...
        // Load New Levels Into Memory
        if (Model->TextureLevelInRAM_ < Model->TargetTextureLevelRAM) {
            for (int LevelToLoad = std::max(Model->TextureLevelInRAM_, 0); LevelToLoad <= Model->TargetTextureLevelRAM; LevelToLoad++) {
                for (unsigned int TextureIndex = 0; TextureIndex < Model->Textures_.size(); TextureIndex++) {
                    LoadImageDataRAM(&Model->Textures_[TextureIndex], LevelToLoad, LogEnable);
                }
                Model->TextureLevelInRAM_ = LevelToLoad;

            }
        }

        // Unload Old 
        else if (Model->TextureLevelInRAM_ > Model->TargetTextureLevelRAM) {
            for (int LevelToUnload = Model->TextureLevelInRAM_; LevelToUnload > Model->TargetTextureLevelRAM; LevelToUnload--) {
                for (unsigned int TextureIndex = 0; TextureIndex < Model->Textures_.size(); TextureIndex++) {
                    UnloadImageDataRAM(&Model->Textures_[TextureIndex], LevelToUnload, LogEnable);
                }
                Model->TextureLevelInRAM_ = LevelToUnload - 1;
            }
        }


}
void ERS_CLASS_AsyncTextureUpdater::SetLevelVRAM(ERS_STRUCT_Model* Model, bool LogEnable) {

        // Abort If Level Not Loaded Yet
        if (Model->TextureLevelInVRAM_ < Model->TargetTextureLevelVRAM) {

            // Load This VRAM Level For All Textures
            int LevelToLoad = Model->TargetTextureLevelVRAM;
            for (unsigned int TextureIndex = 0; TextureIndex < Model->Textures_.size(); TextureIndex++) {

                // Requested Level
                LoadImageDataVRAM(&Model->Textures_[TextureIndex], LevelToLoad, LogEnable);
                Model->Textures_[TextureIndex].BestAvailableOpenGLID = Model->Textures_[TextureIndex].TextureLevels[LevelToLoad].LevelTextureOpenGLID;
                Model->Textures_[TextureIndex].HasAnyLevelReady = true;

            }
            Model->TextureLevelInVRAM_ = LevelToLoad;
        }

        else if (Model->TextureLevelInVRAM_ > Model->TargetTextureLevelVRAM) {

            // Here, we unload *all* levels that are above the target (if they're loaded)
            for (int LevelToUnload = Model->TextureLevelInVRAM_; LevelToUnload > Model->TargetTextureLevelVRAM; LevelToUnload--) {
                for (unsigned int TextureIndex = 0; TextureIndex < Model->Textures_.size(); TextureIndex++) {
                    
                    // Make Sure We Don't Unload Level 0
                    int PriorLevel = LevelToUnload - 1;
                    if (PriorLevel >= 0) {

                        // Ensure Prior Level Is Loaded
                        if (!Model->Textures_[TextureIndex].TextureLevels[PriorLevel].LevelLoadedInVRAM) {
                            LoadImageDataVRAM(&Model->Textures_[TextureIndex], PriorLevel, LogEnable);
                        }

                        // Check If Level Already Loaded, Otherwise, Don't Try To Unload It
                        if (Model->Textures_[TextureIndex].TextureLevels[LevelToUnload].LevelLoadedInVRAM) {
                            UnloadImageDataVRAM(&Model->Textures_[TextureIndex], LevelToUnload, LogEnable);
                        }

                        // Find New Best OpenGL ID
                        for (unsigned int i = 0; i < Model->Textures_[TextureIndex].TextureLevels.size(); i++) {
                            if (Model->Textures_[TextureIndex].TextureLevels[i].LevelLoadedInVRAM) {
                                Model->Textures_[TextureIndex].BestAvailableOpenGLID = Model->Textures_[TextureIndex].TextureLevels[i].LevelTextureOpenGLID;
                                Model->Textures_[TextureIndex].HasAnyLevelReady = true;
                            }
                        }

                    }
                    

                }

            }

            // Detect New Level Loaded In Memory
            if (Model->Textures_.size() > 0) {
                for (unsigned int i = 0; i < Model->Textures_[0].TextureLevels.size(); i++) {
                    if (Model->Textures_[0].TextureLevels[i].LevelLoadedInVRAM) {
                        Model->TextureLevelInVRAM_ = i;
                    }
                }
            }
        }


}
void ERS_CLASS_AsyncTextureUpdater::ProcessLoadWorkItem(ERS_STRUCT_Model* Model) {
    
    // Identify Type Of Work To Be Done
    int TargetRAMLevel = Model->TargetTextureLevelRAM;

    // Perform RAM Updates
    if (Model->TextureLevelInRAM_!= TargetRAMLevel) {
        SetLevelRAM(Model, true);
    }

}
void ERS_CLASS_AsyncTextureUpdater::ProcessPushWorkItem(ERS_STRUCT_Model* Model) {
    
    // Identify Type Of Work To Be Done
    int TargetVRAMLevel = Model->TargetTextureLevelVRAM;

    // Perform VRAM Updates
    if (Model->TextureLevelInVRAM_ != TargetVRAMLevel) {
        SetLevelVRAM(Model, true);
    }

}
void ERS_CLASS_AsyncTextureUpdater::ProcessVRAMUpdate(int Index, ERS_STRUCT_Scene* Scene) {

    BlockPusherThreads_.lock();

    bool CanAdd = true;

    // Check If Queue Full
    if (PushWorkItems_.size() >= (unsigned int)WorkQueueLimit_) {
        CanAdd = false;
    }

    // Skip Checking If Already In Queue If We Can't Add
    if (CanAdd) {
        for (unsigned int x = 0; x < PushWorkItems_.size(); x++) {
            if (PushWorkItems_[x] == Scene->Models[Index]) {
                CanAdd = false;
                break;
            }
        }
    }


    // Memory Check
    if (Scene->Models[Index]->TargetTextureLevelVRAM > Scene->Models[Index]->TextureLevelInVRAM_) {
        if (SystemUtils_->ERS_HardwareInformation_->GetHWInfo().Dynamic_.PhysicalMemoryFree < SystemUtils_->RendererSettings_->CriticalLowRAMBytes) {
            CanAdd = false;
        }
    }


    // If We Can Actually Add It, Do So
    if (CanAdd) {

        if (PrioritizeQueueByVisualImpact_) {
            int HighestTargetLevel = Scene->Models[Index]->TargetTextureLevelVRAM;

            if (HighestTargetLevel > -1) {
                float Priority = ((double)HighestTargetLevel + 0.0001f) / Scene->Models[Index]->MaxTextureLevel_;
                int InsertLocationIndex = PushWorkItems_.size() * Priority;
                InsertLocationIndex = std::min(InsertLocationIndex, (int)PushWorkItems_.size());
                InsertLocationIndex = std::max(InsertLocationIndex, 0);
                PushWorkItems_.insert(PushWorkItems_.end() - InsertLocationIndex, Scene->Models[Index]);
            }
        } else {
            PushWorkItems_.push_back(Scene->Models[Index]);
        }
    }

    BlockPusherThreads_.unlock();

}
void ERS_CLASS_AsyncTextureUpdater::ProcessRAMUpdate(int Index, ERS_STRUCT_Scene* Scene) {

    BlockLoaderThreads_.lock();

    bool CanAdd = true;

    // Check If Queue Full
    if (LoadWorkItems_.size() >= (unsigned int)WorkQueueLimit_) {
        CanAdd = false;
    }

    // Skip Checking If Already In Queue If We Can't Add
    if (CanAdd) {
        for (unsigned int x = 0; x < LoadWorkItems_.size(); x++) {
            if (LoadWorkItems_[x] == Scene->Models[Index]) {
                CanAdd = false;
                break;
            }
        }
    }

    // Memory Check
    if (Scene->Models[Index]->TargetTextureLevelRAM > Scene->Models[Index]->TextureLevelInRAM_) {
        if (SystemUtils_->ERS_HardwareInformation_->GetHWInfo().Dynamic_.PhysicalMemoryFree < SystemUtils_->RendererSettings_->CriticalLowRAMBytes) {
            CanAdd = false;
        }
    }

    // If We Can Actually Add It, Do So
    if (CanAdd) {

        if (PrioritizeQueueByVisualImpact_) {
            int HighestTargetLevel = Scene->Models[Index]->TargetTextureLevelVRAM;

            if (HighestTargetLevel > -1) {
                float Priority = ((double)HighestTargetLevel + 0.0001f) / Scene->Models[Index]->MaxTextureLevel_;
                int InsertLocationIndex = LoadWorkItems_.size() * Priority;
                InsertLocationIndex = std::min(InsertLocationIndex, (int)LoadWorkItems_.size());
                InsertLocationIndex = std::max(InsertLocationIndex, 0);
                LoadWorkItems_.insert(LoadWorkItems_.end() - InsertLocationIndex, Scene->Models[Index]);
            }

        } else {
            LoadWorkItems_.push_back(Scene->Models[Index]);
        }
    }

    BlockLoaderThreads_.unlock();

}

// Streaming Threads
void ERS_CLASS_AsyncTextureUpdater::SortModels(ERS_STRUCT_Scene* Scene) {

    // Iterate Over All Models
    for (unsigned int i = 0; i < Scene->Models.size(); i++) {
        
        // Check RAM Level vs Target
        int CurrentRAMLevel = Scene->Models[i]->TextureLevelInRAM_;
        int TargetRAMLevel = Scene->Models[i]->TargetTextureLevelRAM;
        bool RAMUpdate = CurrentRAMLevel!=TargetRAMLevel;

        // Check VRAM Level vs Target
        int CurrentVRAMLevel = Scene->Models[i]->TextureLevelInVRAM_;
        int TargetVRAMLevel = Scene->Models[i]->TargetTextureLevelVRAM;
        bool VRAMUpdate = CurrentVRAMLevel!=TargetVRAMLevel;

        // If There's Anything To Update, Add To Queue
        if (RAMUpdate) {
            ProcessRAMUpdate(i, Scene);
        }
        if (VRAMUpdate) {
            ProcessVRAMUpdate(i, Scene);
        }


    }

    // Reset Strings
    RAMQueueString = "";
    VRAMQueueString = "";
    RAMBeingProcessed_.clear();
    VRAMBeingProcessed_.clear();

    BlockLoaderThreads_.lock();
    for (unsigned int i = 0; i < LoadWorkItems_.size(); i++) {
        ERS_STRUCT_Model* Model = LoadWorkItems_[i].get();

        if (Model->TargetTextureLevelRAM > Model->TextureLevelInRAM_) {
            RAMQueueString += "L";
        } else if (Model->TargetTextureLevelRAM < Model->TextureLevelInRAM_) {
            RAMQueueString += "U";
        }

        RAMBeingProcessed_.push_back(Model->TexturesBeingLoaded);

    }
    BlockLoaderThreads_.unlock();



    BlockPusherThreads_.lock();
    for (unsigned int i = 0; i < PushWorkItems_.size(); i++) {
        ERS_STRUCT_Model* Model = PushWorkItems_[i].get();

        if (Model->TargetTextureLevelVRAM > Model->TextureLevelInVRAM_) {
            VRAMQueueString += "P";
        } else if (Model->TargetTextureLevelVRAM < Model->TextureLevelInVRAM_) {
            VRAMQueueString += "F";
        }

        VRAMBeingProcessed_.push_back(Model->TexturesBeingPushed);

    }
    BlockPusherThreads_.unlock();




}
void ERS_CLASS_AsyncTextureUpdater::TexturePusherThread(int Index) {

    // Name Thread
    std::string ThreadName = std::string("ERS_TPT-") + std::to_string(Index);
    SetThreadName(ThreadName);

    // Setup OpenGL Shared Context
    SystemUtils_->Logger_->Log(std::string("Texture Streaming Thead '") + std::to_string(Index) + "' Creating Shared OpenGL Context", 3);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* ThreadWindow = glfwCreateWindow(1, 1, std::to_string(Index).c_str(), NULL, MainThreadWindowContext_);
    glfwMakeContextCurrent(ThreadWindow);
    SystemUtils_->Logger_->Log(std::string("Texture Streaming Thead '") + std::to_string(Index) + "' Finished Creating OpenGL Context", 2);
    PusherThreadReady_ = true;

    while (!StopPusherThreads_) {

        // Get Work Item If It Exists
        std::shared_ptr<ERS_STRUCT_Model> WorkItem = nullptr;
        bool HasWorkItem = false;
        BlockPusherThreads_.lock();
        for (unsigned int i = 0; i < PushWorkItems_.size(); i++) {
            bool HasRAMLevel = PushWorkItems_[i]->TargetTextureLevelVRAM <= PushWorkItems_[i]->TextureLevelInRAM_;
            if (!PushWorkItems_[i]->TexturesBeingPushed && HasRAMLevel) {
                WorkItem = PushWorkItems_[i];
                HasWorkItem = true;
                PushWorkItems_.erase(PushWorkItems_.begin() + i);
                break;
            }
        }
        BlockPusherThreads_.unlock();

        // Process Item, If Item Doens't Exist, Sleep Thread
        if (HasWorkItem) {
            WorkItem->TexturesBeingPushed = true;

            // We Need All Texture Levels In RAM To Push To VRAM, Check This Is True!
            // If It's Not, We're Going To Wait Until They Are By Moving This Item To The End Of The Queue
            if (WorkItem->TextureLevelInRAM_ < WorkItem->TargetTextureLevelVRAM) {
                // if (WorkItem->TargetTextureLevelRAM < WorkItem->TargetTextureLevelVRAM) {
                //     WorkItem->TargetTextureLevelRAM = WorkItem->TargetTextureLevelVRAM;
                // }
                // SetLevelRAM(Model, LogEnable);
                // BlockPusherThreads_.lock();
                // PushWorkItems_.push_back(WorkItem);
                // BlockPusherThreads_.unlock();

            }
            else {
                ProcessPushWorkItem(WorkItem.get());
            }
            WorkItem->TexturesBeingPushed = false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    }

    // Destroy OpenGL Context
    glfwDestroyWindow(ThreadWindow);

}
void ERS_CLASS_AsyncTextureUpdater::TextureLoaderThread(int Index) {

    // Setup FreeImage
    FreeImage_Initialise();

    // Name Thread
    std::string ThreadName = std::string("ERS_TLT-") + std::to_string(Index);
    SetThreadName(ThreadName);


    while (!StopLoaderThreads_) {

        // Get Work Item If It Exists
        std::shared_ptr<ERS_STRUCT_Model> WorkItem = nullptr;
        bool HasWorkItem = false;
        BlockLoaderThreads_.lock();
        for (unsigned int i = 0; i < LoadWorkItems_.size(); i++) {
            if (!LoadWorkItems_[i]->TexturesBeingLoaded) {
                WorkItem = LoadWorkItems_[i];
                HasWorkItem = true;
                LoadWorkItems_.erase(LoadWorkItems_.begin() + i);
                break;
            }
        }
        BlockLoaderThreads_.unlock();

        // Process Item, If Item Doens't Exist, Sleep Thread
        if (HasWorkItem) {
            WorkItem->TexturesBeingLoaded = true;
            ProcessLoadWorkItem(WorkItem.get());
            WorkItem->TexturesBeingLoaded = false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    }



    // Shut Down FreeImage
    FreeImage_DeInitialise();
}
int ERS_CLASS_AsyncTextureUpdater::GetNumLoaderThreads() {
    return NumLoaderThreads_;
}
int ERS_CLASS_AsyncTextureUpdater::GetNumStreamerThreads() {
    return NumPusherThreads_;
}
void ERS_CLASS_AsyncTextureUpdater::SetNumLoaderThreads(int NumThreads) {
    NumLoaderThreads_ = NumThreads;
}
void ERS_CLASS_AsyncTextureUpdater::SetNumStreamerThreads(int NumThreads) {
    NumPusherThreads_ = NumThreads;
}
void ERS_CLASS_AsyncTextureUpdater::SetupPusherThreads() {

    // Setup Threads
    SystemUtils_->Logger_->Log("Starting GPU Worker Thread Pool", 4);
    SystemUtils_->Logger_->Log(std::string("GPU Worker Pool Will Have ") + std::to_string(NumPusherThreads_) + " Threads", 3);
    

    // For some reason windows cannot handle sharing a context if it's in use by another thread so we have to do this bullshit.
    // thanks, windows! /s
    StopPusherThreads_ = false;
    glfwMakeContextCurrent(NULL);

    for (unsigned int i = 0; i < (unsigned int)NumPusherThreads_; i++) {
        PusherThreadReady_ = false;
        TexturePusherThreads_.push_back(std::thread(&ERS_CLASS_AsyncTextureUpdater::TexturePusherThread, this, i));
        SystemUtils_->Logger_->Log(std::string("Started GPU Worker Thread '") + std::to_string(i) + "'", 2);
        while (!PusherThreadReady_) {}
    }

    glfwMakeContextCurrent(MainThreadWindowContext_);
    SystemUtils_->Logger_->Log("Setup GPU Worker Thread Pool", 3);

}
void ERS_CLASS_AsyncTextureUpdater::SetupLoaderThreads() {

    // Setup Threads
    SystemUtils_->Logger_->Log("Starting CPU Worker Thread Pool", 4);
    SystemUtils_->Logger_->Log(std::string("CPU Worker Pool Will Have ") + std::to_string(NumLoaderThreads_) + " Threads", 3);

    StopLoaderThreads_ = false;
    for (unsigned int i = 0; i < (unsigned int)NumLoaderThreads_; i++) {
        TextureLoaderThreads_.push_back(std::thread(&ERS_CLASS_AsyncTextureUpdater::TextureLoaderThread, this, i));
        SystemUtils_->Logger_->Log(std::string("Started CPU Worker Thread '") + std::to_string(i) + "'", 2);
    }
    SystemUtils_->Logger_->Log("Setup CPU Worker Thread Pool", 3);

}
void ERS_CLASS_AsyncTextureUpdater::TeardownPusherThreads() {

    // Send Shutdown Command
    SystemUtils_->Logger_->Log("Sending Stop Command To GPU Worker Thread Pool", 5);
    StopPusherThreads_ = true;
    SystemUtils_->Logger_->Log("Stop Command Sent", 3);

    // Join Threads
    SystemUtils_->Logger_->Log("Joining Texture Streaming GPU Worker Thread Pool", 5);
    for (unsigned int i = 0; i < TexturePusherThreads_.size(); i++) {
        SystemUtils_->Logger_->Log(std::string("Joining Texture Streaming GPU Worker Thread '") + std::to_string(i) + "'", 3);
        TexturePusherThreads_[i].join();
    }
    TexturePusherThreads_.clear();

    SystemUtils_->Logger_->Log("Finished Joining Texture Streaming GPU Worker Thread Pool", 4);

}
void ERS_CLASS_AsyncTextureUpdater::TeardownLoaderThreads() {

    // Send Shutdown Command
    SystemUtils_->Logger_->Log("Sending Stop Command To CPU Worker Thread Pool", 5);
    StopLoaderThreads_ = true;
    SystemUtils_->Logger_->Log("Stop Command Sent", 3);

    // Join Threads
    SystemUtils_->Logger_->Log("Joining Texture Streaming CPU Worker Thread Pool", 5);
    for (unsigned int i = 0; i < TextureLoaderThreads_.size(); i++) {
        SystemUtils_->Logger_->Log(std::string("Joining Texture Streaming CPU Worker Thread '") + std::to_string(i) + "'", 3);
        TextureLoaderThreads_[i].join();
    }
    TextureLoaderThreads_.clear();

    SystemUtils_->Logger_->Log("Finished Joining Texture Streaming CPU Worker Thread Pool", 4);

}
int ERS_CLASS_AsyncTextureUpdater::GetQueueLimit() {
    return WorkQueueLimit_;
}
void ERS_CLASS_AsyncTextureUpdater::SetQueueLimit(int QueueLimit) {
    WorkQueueLimit_ = QueueLimit;
}
bool ERS_CLASS_AsyncTextureUpdater::GetDupeQueueEntryPrevention() {
    return PreventDuplicateWorkItems_;
}
void ERS_CLASS_AsyncTextureUpdater::SetDupeQueueEntryPrevention(bool State) {
    PreventDuplicateWorkItems_ = State;
}
bool ERS_CLASS_AsyncTextureUpdater::GetQueuePrioritizationEnabled() {
    return PrioritizeQueueByVisualImpact_;
}
void ERS_CLASS_AsyncTextureUpdater::SetQueuePrioritizationEnabled(bool State) {
    PrioritizeQueueByVisualImpact_ = State;
}
void ERS_CLASS_AsyncTextureUpdater::QueuePanic() {

    // Lock Mutexes
    BlockLoaderThreads_.lock();
    BlockPusherThreads_.lock();

    // Strip Load Items
    std::vector<std::shared_ptr<ERS_STRUCT_Model>> ModelsWithUnloads;
    for (unsigned int i = 0; i < LoadWorkItems_.size(); i++) {
        if (LoadWorkItems_[i]->TargetTextureLevelRAM < LoadWorkItems_[i]->TextureLevelInRAM_) {
            ModelsWithUnloads.push_back(LoadWorkItems_[i]);
        }
    }
    LoadWorkItems_.clear();
    for (unsigned int i = 0; i < ModelsWithUnloads.size(); i++) {
        LoadWorkItems_.push_back(ModelsWithUnloads[i]);
    }

    // Strip Push Items
    std::vector<std::shared_ptr<ERS_STRUCT_Model>> PushItemsWithUnloads;
    for (unsigned int i = 0; i < PushWorkItems_.size(); i++) {
        if (PushWorkItems_[i]->TargetTextureLevelVRAM < PushWorkItems_[i]->TextureLevelInVRAM_) {
            PushItemsWithUnloads.push_back(PushWorkItems_[i]);
        }
    }
    PushWorkItems_.clear();
    for (unsigned int i = 0; i < PushItemsWithUnloads.size(); i++) {
        PushWorkItems_.push_back(PushItemsWithUnloads[i]);
    }


    // Lock Mutexes
    BlockLoaderThreads_.unlock();
    BlockPusherThreads_.unlock();

}