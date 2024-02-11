#include <Simulator/Distributions/TruncNorm.h>

#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {
namespace Distributions {

//! Probability distribution function for the standard normal distribution.
static float _StandardNormalPDF(float x) {
    static float multiplier = 1.0 / sqrt(2 * M_PI);
    return multiplier * exp(-0.5 * x * x);
};

//! Cumulative distribution function for the standard normal distribution.
static float _StandardNormalCDF(float x) {
    static float sqrt_2 = sqrt(2.0);
    return 0.5 * (1 + erf(x / sqrt_2));
};

//! Constructors
TruncNorm::TruncNorm(float _a, float _b) {
    assert(_b > _a);
    this->a = _a;
    this->b = _b;
};

// Use this to test a distribution without assuming anything.
std::string Histogram(std::vector<float> samples, unsigned int numbins = 9) {
    float min = 10000.0;
    float max = -10000.0;
    for (auto x : samples) {
        if (x < min) min = x;
        if (x > max) max = x;
    }
    float range = max - min;
    float dx = range / numbins;
    std::vector<unsigned int> bins(numbins, 0);
    for (auto x : samples) {
        float v = (x - min) / dx;
        if (v < 0) v = 0;
        if (v > (numbins-1)) v = numbins - 1;
        bins[(unsigned int) v]++;
    }
    std::string hist("Histogram (min="+std::to_string(min)+", max="+std::to_string(max)+"):\n");
    for (auto count : bins) {
        hist += std::to_string(count) + ' ';
    }
    hist += '\n';
    return hist;
}

TruncNorm::TruncNorm(float _a, float _b, float _loc, float _scale) {
    assert(_b > _a);
    this->a = _a;
    this->b = _b;
    this->loc = _loc;
    this->scale = _scale;
};

//! Generates a random sample from the distribution of size numSamples.
std::vector<float> TruncNorm::RandomSample(size_t numSamples) {
    // Standard normal distribution
    static std::normal_distribution<float> _stdNormalDist; // Note this is static! Created only once, initialized only on first call.
    std::vector<float> randomSample;

    while (randomSample.size() < numSamples) {
        float val = this->loc + _stdNormalDist(this->_Gen) * this->scale;
        if (val < this->a || val > this->b)
            continue;
        randomSample.emplace_back(val);
    }

    return randomSample;
};

//! Probability distribution function
std::vector<float> TruncNorm::PDF(std::vector<float> x) {
    std::vector<float> pdf;
    for (float val : x)
        pdf.emplace_back(this->PDF(val));
    return pdf;
};

float TruncNorm::PDF(float x) {
    // If out of bounds, return 0
    if (x < this->a || x > this->b)
        return 0.0;

    static float z_a = (this->a - this->loc) / this->scale;
    static float z_b = (this->b - this->loc) / this->scale;
    static float Z = _StandardNormalCDF(z_b) - _StandardNormalCDF(z_a);

    float z_x = (x - this->loc) / this->scale;
    return _StandardNormalPDF(z_x) / (this->scale * Z);
};

//! Cumulative distribution function
std::vector<float> TruncNorm::CDF(std::vector<float> x) {
    std::vector<float> cdf;
    for (float val : x)
        cdf.emplace_back(this->CDF(val));
    return cdf;
};

float TruncNorm::CDF(float x) {
    // If x < a, return 0
    if (x < a)
        return 0.0;
    // If x > b, return 1
    if (x > b)
        return 1.0;

    static float z_a = (this->a - this->loc) / this->scale;
    static float z_b = (this->b - this->loc) / this->scale;
    static float Z = _StandardNormalCDF(z_b) - _StandardNormalCDF(z_a);

    float z_x = (x - this->loc) / this->scale;

    return (_StandardNormalCDF(z_x) - _StandardNormalCDF(z_a)) / Z;
};

//! Mean and standard deviation.
std::tuple<float, float> TruncNorm::Stats() {
    std::tuple<float, float> stats{this->Mean(), this->Std()};
    return stats;
};

//! Mean
float TruncNorm::Mean() {
    static float z_a = (this->a - this->loc) / this->scale;
    static float z_b = (this->b - this->loc) / this->scale;
    static float Z = _StandardNormalCDF(z_b) - _StandardNormalCDF(z_a);
    static float mean =
        this->loc +
        (_StandardNormalPDF(z_a) - _StandardNormalPDF(z_b)) * this->scale / Z;

    return mean;
};

//! Standard deviation
float TruncNorm::Std() {
    static float z_a = (this->a - this->loc) / this->scale;
    static float z_b = (this->b - this->loc) / this->scale;
    static float Z = _StandardNormalCDF(z_b) - _StandardNormalCDF(z_a);
    static float pdf_z_a = _StandardNormalPDF(z_a);
    static float pdf_z_b = _StandardNormalPDF(z_b);

    static float sigma =
        this->scale * sqrt(1 - (z_b * pdf_z_b - z_a * pdf_z_a) / Z -
                           pow((pdf_z_a - pdf_z_b) / Z, 2.0));

    return sigma;
};

}; // namespace Distributions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
