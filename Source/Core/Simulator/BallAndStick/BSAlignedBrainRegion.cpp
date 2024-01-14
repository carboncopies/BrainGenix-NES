#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Constructors
BSAlignedBrainRegion::BSAlignedBrainRegion(){};

BSAlignedBrainRegion::BSAlignedBrainRegion(size_t _ID, Geometries::Box * _Shape, std::shared_ptr<BallAndStick::BSAlignedNC> _Content) {
    this->ID = _ID;
    this->Shape = _Shape;
    _Content->InitCells(_Shape);
    this->Content = _Content;
};

//! Display the BrainRegion
void BSAlignedBrainRegion::Show(float linewidth) { return; }

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
