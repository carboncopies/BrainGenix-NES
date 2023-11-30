#include <Simulator/Structs/SynTrQuantalRelease.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

//! Constructors
SynTrQuantalRelease::SynTrQuantalRelease(){};
SynTrQuantalRelease::SynTrQuantalRelease(int _NumQuantaRelease_T,
                                         float _ReleaseProb)
    : NumQuantaRelease_T(_NumQuantaRelease_T) {
    assert(0.0 <= _ReleaseProb && _ReleaseProb <= 1.0);
    this->ReleaseProb = _ReleaseProb;
};

float SynTrQuantalRelease::QuantalContent_m() {
    return this->NumQuantaRelease_T * this->ReleaseProb;
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
