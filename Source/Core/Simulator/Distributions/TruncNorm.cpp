#include <Simulator/Distributions/TruncNorm.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

//! Constructors
TruncNorm::TruncNorm(float _a, float _b) : a(_a), b(_b) { };
TruncNorm::TruncNorm(float _a, float _b, float _loc, float _scale) : a(_a), b(_b) {
    this->loc = _loc;
    this->scale = _scale;
};

//! Generates a random sample from the distribution of size numSamples.
std::vector<float> TruncNorm::RandomSample(size_t numSamples) {
    return std::vector<float>();
};

//! Probability distribution function
std::vector<float> TruncNorm::PDF(std::vector<float> x) {
    return std::vector<float>();
};

float TruncNorm::PDF(float x) {
    return 0.0;
};

//! Cumulative distribution function
std::vector<float> TruncNorm::CDF(std::vector<float> x) {
    return std::vector<float>();
};

float TruncNorm::CDF(float x) {
    return 0.0;
};

//! Percent point function.
std::vector<float> TruncNorm::PPF(std::vector<float> x) {
    return std::vector<float>();
};

float TruncNorm::PPF(float x) {
    return 0.0;
};

//! Mean, variance, skewness and kurtosis
std::tuple<float, float, float, float> TruncNorm::Stats() {
    return std::tuple<float, float, float, float>();
};

//! Mean
float TruncNorm::Mean() {
    return 0.0;
};

//! Standard deviation
float TruncNorm::Std() {
    return 0.0;
};

}; // Close Namespace Distributions
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
