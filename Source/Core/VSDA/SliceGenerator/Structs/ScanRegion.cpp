#include <VSDA/SliceGenerator/Structs/ScanRegion.h>



namespace BG {
namespace NES {
namespace Simulator {



std::string ScanRegion::Dimensions() {

    std::string Out = "";
    Out += std::to_string(Point1X_um - Point2X_um) + "X, ";
    Out += std::to_string(Point1Y_um - Point2Y_um) + "Y, ";
    Out += std::to_string(Point1Z_um - Point2Z_um) + "Z";

    return Out;
}

std::string ScanRegion::ToString() {
    std::string Out = "";
    Out += std::to_string(Point1X_um) + "X1, ";
    Out += std::to_string(Point1Y_um) + "Y1, ";
    Out += std::to_string(Point1Z_um) + "Z1 | ";

    Out += std::to_string(Point2X_um) + "X2 ";
    Out += std::to_string(Point2Y_um) + "Y2, ";
    Out += std::to_string(Point2Z_um) + "Z2";

    return Out;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
