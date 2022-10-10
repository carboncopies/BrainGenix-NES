// In-Memory Image Writing Test

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
#include <LuciferIL/Lucifer_EnumToString.h>





struct IOData {

    std::unique_ptr<unsigned char[]> Data;
    unsigned long Size = 0;
};


void ReadIOData(IOData& IOData, std::string Path) {

    struct stat Buffer;
    int FileStatus = stat(Path.c_str(), &Buffer);
    IOData.Size = Buffer.st_size + 1;


    IOData.Data.reset(new unsigned char[Buffer.st_size + 1]);

    FILE *Stream = fopen(Path.c_str(), "rb");

    [[maybe_unused]]size_t _ = fread(IOData.Data.get(), sizeof(unsigned char), Buffer.st_size,Stream);
        
    IOData.Data.get()[Buffer.st_size] = '\0';
    fclose(Stream);


}


void WriteIOData(IOData &IOData, std::string Path) {

    FILE *Stream = fopen(Path.c_str(), "wb");
    fwrite(IOData.Data.get(), 1, sizeof(unsigned char) * IOData.Size, Stream);
    fclose(Stream);

}


int main() {


    Lucifer::Lucifer Luci;

    std::cout<<"Loading Image: 'Assets/Test.png' From Disk\n";
    IOData Data;
    Lucifer::Image Image;
    ReadIOData(Data, "Assets/Test.png");
    Lucifer::LoadingStatus Status = Luci.Load(Data.Data.get(), Data.Size, Image);


    std::cout<<"Writing Image: 'Assets/TestOutput.png' To Disk\n";
    IOData NewData;
    Luci.Write(Image, &NewData.Data, NewData.Size);
    WriteIOData(NewData, "Assets/TestOutput.png");

}
