#include <Simulator/Distributions/Generic.h>



namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

int Generic::UniformRandomInt() {
    // Uniform distribution between 0 and postive max int.
    static std::uniform_int_distribution<int> _stdUniformDist; // Note this is static! Created only once, initialized only on first call.

    return _stdUniformDist(Gen_);

}

float Generic::UniformRandomFloat() {
    // Uniform between 0.0 and 1.0.
    static std::uniform_real_distribution<float> _stdUniformDist;

    return _stdUniformDist(Gen_);
}

float Generic::NormalRandomFloat() {
    // mean 0.0, stdev 1.0
    static std::normal_distribution<float> _stdNormalDist;

    return _stdNormalDist(Gen_);
}

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
