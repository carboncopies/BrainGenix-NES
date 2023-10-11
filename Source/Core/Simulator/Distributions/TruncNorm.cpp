#include <Simulator/Distributions/TruncNorm.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

//! Constructors
TruncNorm::TruncNorm(float _a, float _b) : a(_a), b(_b) { };

//! Generates a random sample from the distribution of size numSamples.
std::vector<float> TruncNorm::RandomSample(float loc, float scale, size_t numSamples) {
    return std::vector<float>();
};

//! Probability distribution function
std::vector<float> TruncNorm::PDF(std::vector<float> x, float loc, float scale) {
    return std::vector<float>();
};

float TruncNorm::PDF(float x, float loc, float scale) {
    return 0.0;
};

//! Cumulative distribution function
std::vector<float> TruncNorm::CDF(std::vector<float> x, float loc, float scale) {
    return std::vector<float>();
};

float TruncNorm::CDF(float x, float loc, float scale) {
    return 0.0;
};

//! Percent point function.
std::vector<float> TruncNorm::PPF(std::vector<float> x, float loc, float scale) {
    return std::vector<float>();
};

float TruncNorm::PPF(float x, float loc, float scale) {
    return 0.0;
};

//! Mean, variance, skewness and kurtosis
std::tuple<float, float, float, float> TruncNorm::Stats(float loc, float scale) {
    return std::tuple<float, float, float, float>();
};

//! Mean
float TruncNorm::Mean(float loc, float scale) {
    return 0.0;
};

//! Standard deviation
float TruncNorm::Std(float loc, float scale) {
    return 0.0;
};

}; // Close Namespace Distributions
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
