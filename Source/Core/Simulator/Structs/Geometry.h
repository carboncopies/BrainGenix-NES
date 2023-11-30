#pragma once

namespace BG {
namespace NES {
namespace Simulator {
namespace Shapes {


struct BoundingBox {
    float bb_point1[3];
    float bb_point2[3];
};

/**
 * @brief This is inherited by all shapes.
 */
struct Geometry {
    virtual BoundingBox GetBoundingBox() = 0;
};

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
