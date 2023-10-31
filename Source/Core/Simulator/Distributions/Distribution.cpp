#include <Simulator/Distributions/Distribution.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

//! Constructor
Distribution::Distribution() {
    uint32_t seed;
    std::random_device rd;
    if (rd.entropy())
        seed = rd();
    else
        seed = std::chrono::high_resolution_clock::now()
                   .time_since_epoch()
                   .count();
    this->_Gen.seed(seed);
};

//! Sets the random number generator seed.
void Distribution::SetSeed(uint32_t _Seed) { this->_Gen.seed(_Seed); };

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
