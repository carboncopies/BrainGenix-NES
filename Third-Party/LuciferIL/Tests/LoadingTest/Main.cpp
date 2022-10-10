// In-Memory Image Loading Test

#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <LuciferIL/Lucifer_IncludeOnce.h>
#include <LuciferIL/Lucifer.h>
#include <LuciferIL/Lucifer_LoadImageSTB.h>
#include <LuciferIL/Lucifer_LoadImageFreeImage.h>
#include <LuciferIL/Lucifer_LoadImageDevIL.h>
#include <LuciferIL/Lucifer_EnumToString.h>


struct ImageFileObject {

    // Declare Member Variables
    unsigned char *MemoryBuffer;
    struct stat Buffer;
    int Result;


    // Load File Into Mem
    bool LoadImage(const char* FilePath) { // Loads Image Into Memory Buffer, Returns True On Success, False On Failure

        // Get File Stats
        Result = stat(FilePath, &Buffer);
        if (Result == 0) {

            MemoryBuffer = (BYTE*)malloc(Buffer.st_size * sizeof(unsigned char));
            if (MemoryBuffer) {

                FILE *Stream = fopen(FilePath, "rb");
                if (Stream) {

                    // Read File Data
                    fread(MemoryBuffer, sizeof(unsigned char), Buffer.st_size, Stream);
                    fclose(Stream);

                    return true;

                }
            }
        }

        // Return Fail
        return false;

    }


};


void TestLoadFreeImage(const char* Path) {

    // Load Image Path
    ImageFileObject Obj;
    Obj.LoadImage(Path);

    // Load Image Into Memory
    std::cout<<"Loading Image Into Memory Object\n";
    Lucifer::Image Image;
    Lucifer::LoadingStatus Status = Lucifer::Lucifer_LoadFreeImage(Obj.MemoryBuffer, Obj.Buffer.st_size, Image);

    std::cout<<"Image Loading Status Is: "<<Lucifer::Lucifer_GetLoadingStatusString(Status)<<std::endl;

    // List Image Information
    std::cout<<"Image Is "<<Image.Width<<"px Wide\n";
    std::cout<<"Image Is "<<Image.Height<<"px Tall\n";
    std::cout<<"Image Has "<<Image.Channels<<" Color Channels\n";
    
    // Delete Image From buffer
    free(Obj.MemoryBuffer);

}

void TestLoadSTB(const char* Path) {

    // Load Image Path
    ImageFileObject Obj;
    Obj.LoadImage(Path);

    // Load Image Into Memory
    std::cout<<"Loading Image Into Memory Object\n";
    Lucifer::Image Image;
    Lucifer::LoadingStatus Status = Lucifer::Lucifer_LoadSTB(Obj.MemoryBuffer, Obj.Buffer.st_size, Image);

    std::cout<<"Image Loading Status Is: "<<Lucifer::Lucifer_GetLoadingStatusString(Status)<<std::endl;

    // List Image Information
    std::cout<<"Image Is "<<Image.Width<<"px Wide\n";
    std::cout<<"Image Is "<<Image.Height<<"px Tall\n";
    std::cout<<"Image Has "<<Image.Channels<<" Color Channels\n";
    
    // Delete Image From buffer
    free(Obj.MemoryBuffer);

}


void TestLoadDevIL(const char* Path) {

    // Load Image Path
    ImageFileObject Obj;
    Obj.LoadImage(Path);

    // Load Image Into Memory
    std::cout<<"Loading Image Into Memory Object\n";
    Lucifer::Image Image;
    Lucifer::LoadingStatus Status = Lucifer::Lucifer_LoadDevIL(Obj.MemoryBuffer, Obj.Buffer.st_size, Image);

    std::cout<<"Image Loading Status Is: "<<Lucifer::Lucifer_GetLoadingStatusString(Status)<<std::endl;

    // List Image Information
    std::cout<<"Image Is "<<Image.Width<<"px Wide\n";
    std::cout<<"Image Is "<<Image.Height<<"px Tall\n";
    std::cout<<"Image Has "<<Image.Channels<<" Color Channels\n";
    
    // Delete Image From buffer
    free(Obj.MemoryBuffer);

}


int main() {

    // Init FreeImage
    FreeImage_Initialise();

    // Load Test.png
    std::cout<<"Loading Image: 'Assets/Test.png' From Disk With FreeImage\n";
    TestLoadFreeImage("Assets/Test.png");
    std::cout<<"Loading Image: 'Assets/Test.png' From Disk With STB\n";
    TestLoadSTB("Assets/Test.png");
    std::cout<<"Loading Image: 'Assets/Test.png' From Disk With DevIL\n";
    TestLoadDevIL("Assets/Test.png");

    // Load Test.png
    std::cout<<"Loading Image: 'Assets/Test.jpg' From Disk With FreeImage\n";
    TestLoadFreeImage("Assets/Test.jpg");
    std::cout<<"Loading Image: 'Assets/Test.jpg' From Disk With STB\n";
    TestLoadSTB("Assets/Test.jpg");
    std::cout<<"Loading Image: 'Assets/Test.jpg' From Disk With DevIL\n";
    TestLoadDevIL("Assets/Test.jpg");

    // De-Init FreeImage
    FreeImage_DeInitialise();

}
