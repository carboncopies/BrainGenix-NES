#include <Simulator/Structs/BoundingBox.h>



namespace BG {
namespace NES {
namespace Simulator {


std::string BoundingBox::Dimensions() {

    std::string Out = "";
    Out += std::to_string(bb_point2[0] - bb_point1[0]) + "X, ";
    Out += std::to_string(bb_point2[1] - bb_point1[1]) + "Y, ";
    Out += std::to_string(bb_point2[2] - bb_point1[2]) + "Z";

    return Out;
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
