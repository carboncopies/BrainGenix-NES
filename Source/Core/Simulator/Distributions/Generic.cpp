#include <Simulator/Distributions/Generic.h>



namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

Generic::Generic(uint32_t _Seed) {
    Gen_.seed(_Seed);
}

int Generic::UniformRandomInt() {
    // Uniform distribution between 0 and postive max int.
    return UniformDistInt_(Gen_);
}

float Generic::UniformRandomFloat() {
    // Uniform between 0.0 and 1.0.
    return UniformDistFloat_(Gen_);
}

float Generic::NormalRandomFloat() {
    // mean 0.0, stdev 1.0
    return NormalDistFloat_(Gen_);
}

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
