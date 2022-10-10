// In-Memory Image Loading Test

#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "FreeImage.h"



struct ImageFileObject {

    // Declare Member Variables
    BYTE *MemoryBuffer;
    struct stat Buffer;
    int Result;


    // Load File Into Mem
    bool LoadImage(const char* FilePath) { // Loads Image Into Memory Buffer, Returns True On Success, False On Failure

        // Get File Stats
        Result = stat(FilePath, &Buffer);
        if (Result == 0) {

            MemoryBuffer = (BYTE*)malloc(Buffer.st_size * sizeof(BYTE));
            if (MemoryBuffer) {

                FILE *Stream = fopen(FilePath, "rb");
                if (Stream) {

                    // Read File Data
                    fread(MemoryBuffer, sizeof(BYTE), Buffer.st_size, Stream);
                    fclose(Stream);

                    return true;

                }
            }
        }

        // Return Fail
        return false;

    }


};


void TestLoad(const char* Path) {

    // Load Image Path
    ImageFileObject Obj;
    Obj.LoadImage(Path);

    // Load Image Into Memory
    std::cout<<"Loading Image Into FreeImage Memory Object (FIMEMORY*)\n";
    FIMEMORY* InMemoryData = FreeImage_OpenMemory(Obj.MemoryBuffer, Obj.Buffer.st_size);
    
    // Identifying Image Format
    std::cout<<"Getting Image Format From FIMEMORY Object\n";
    FREE_IMAGE_FORMAT Format = FreeImage_GetFileTypeFromMemory(InMemoryData);
    std::cout<<"Identified Format To Be: "<<Format<<std::endl;

    // Decode Image
    std::cout<<"Decoding Image\n";
    FIBITMAP* Image = FreeImage_LoadFromMemory(Format, InMemoryData);

    // List Image Information
    std::cout<<"Image Is "<<FreeImage_GetWidth(Image)<<"px Wide\n";
    std::cout<<"Image Is "<<FreeImage_GetHeight(Image)<<"px Tall\n";
    std::cout<<"Image Has "<<FreeImage_GetLine(Image) / FreeImage_GetWidth(Image)<<" Color Channels\n";
    
    // Delete Image From Memory
    FreeImage_CloseMemory(InMemoryData);
    free(Obj.MemoryBuffer);

}

int main() {

    // Init FreeImage
    FreeImage_Initialise();

    // Load Test.png
    std::cout<<"Loading Image: 'Assets/Test.png' From Disk\n";
    TestLoad("Assets/Test.png");
    
    // Load Test.png
    std::cout<<"Loading Image: 'Assets/Test.jpg' From Disk\n";
    TestLoad("Assets/Test.jpg");



    // De-Init FreeImage
    FreeImage_DeInitialise();

}
