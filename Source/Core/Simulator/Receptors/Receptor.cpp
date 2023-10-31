#include <Simulator/Receptors/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Receptors {

float Receptor::PostSynCurrent_pA() {
    this->ISyn_pA = this->GSyn_pS * (this->Vm_mV - this->ESyn_mV);
    return this->ISyn_pA;
};

float Receptor::Conductance_pS() {
    float diff = this->Vm_mV - this->ESyn_mV;

    if (diff == 0.0)
        throw std::overflow_error("Cannot divide by zero.");
    this->GSyn_pS = this->ISyn_pA / diff;
    return this->GSyn_pS;
};

float Receptor::GSynT_pS(float t_ms, Receptor::PSPType pspType) {
    float calculated{};

    switch (pspType) {
    case PSPType::DOUBLE_EXPONENT:
        calculated = this->GSynT_pS_DoubleExponential(t_ms);
        break;
    case PSPType::MXH:
        calculated = this->GSynT_pS_MXH(t_ms);
        break;
    case PSPType::SIMPLE_EXPONENT:
        calculated = this->GSynT_pS_DecayZeroRiseTime(t_ms);
        break;
    case PSPType::ALPHA_FUNCTION:
        calculated = this->GSynT_pS_RiseDecay(t_ms);
        break;
    default:
        break;
    }
    return calculated;
};

std::vector<float> Receptor::GSynT_pS(std::vector<float> t_ms,
                                      Receptor::PSPType pspType) {
    std::vector<float> calculated = {};

    switch (pspType) {
    case PSPType::DOUBLE_EXPONENT:
        calculated = this->GSynT_pS_DoubleExponential(t_ms);
        break;
    case PSPType::MXH:
        calculated = this->GSynT_pS_MXH(t_ms);
        break;
    case PSPType::SIMPLE_EXPONENT:
        calculated = this->GSynT_pS_DecayZeroRiseTime(t_ms);
        break;
    case PSPType::ALPHA_FUNCTION:
        calculated = this->GSynT_pS_RiseDecay(t_ms);
        break;
    default:
        break;
    }
    return calculated;
};

//! Modeled with a simple exponential.
float Receptor::GSynT_pS_DecayZeroRiseTime(float t_ms) {
    assert(t_ms >= 0.0);

    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    return this->GPeak_pS * exp(-t_ms / this->TauD_ms);
};

std::vector<float>
Receptor::GSynT_pS_DecayZeroRiseTime(std::vector<float> t_ms) {
    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    std::vector<float> calculated = {};

    for (float t_msVal : t_ms) {
        assert(t_msVal >= 0.0);
        calculated.push_back(this->GPeak_pS * exp(-t_msVal / this->TauD_ms));
    }
    return calculated;
};

//! Modeled with an alpha function.
float Receptor::GSynT_pS_RiseDecay(float t_ms) {
    assert(t_ms >= 0.0);

    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    return this->GPeak_pS * t_ms / this->TauR_ms *
           exp(1.0 - t_ms / this->TauR_ms);
};

std::vector<float> Receptor::GSynT_pS_RiseDecay(std::vector<float> t_ms) {
    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    std::vector<float> calculated = {};
    for (float t_msVal : t_ms) {
        assert(t_msVal >= 0.0);
        calculated.emplace_back(this->GPeak_pS * t_msVal / this->TauR_ms *
                                exp(1.0 - t_msVal / this->TauR_ms));
    }
    return calculated;
};

//! Modeled with a double exponential.
float Receptor::GSynT_pS_DoubleExponential(float t_ms) {
    assert(t_ms >= 0.0);
    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    if (this->ANorm == 0.0)
        throw std::overflow_error("Cannot divide by zero (ANorm).");
    return this->GPeak_pS *
           (-exp(-t_ms / this->TauR_ms) + exp(-t_ms / this->TauD_ms)) /
           this->ANorm;
};

std::vector<float>
Receptor::GSynT_pS_DoubleExponential(std::vector<float> t_ms) {
    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    if (this->ANorm == 0.0)
        throw std::overflow_error("Cannot divide by zero (ANorm).");

    std::vector<float> calculated = {};
    for (float t_msVal : t_ms) {
        assert(t_msVal >= 0.0);
        calculated.emplace_back(
            this->GPeak_pS *
            (-exp(-t_msVal / this->TauR_ms) + exp(-t_msVal / this->TauD_ms)) /
            this->ANorm);
    }
    return calculated;
};

//! Modeled with a multiexponential function with m^xh formalism to fit
//! more complex waveforms.
float Receptor::GSynT_pS_MXH(float t_ms) {

    assert(t_ms >= 0.0);
    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    if (this->TauD2_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD2_ms).");
    if (this->TauD3_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD3_ms).");
    if (this->ANorm == 0.0)
        throw std::overflow_error("Cannot divide by zero (ANorm).");

    return this->GPeak_pS / this->ANorm *
           pow(this->X, 1.0 - exp(-t_ms / this->TauR_ms)) * this->D1 *
           (exp(-t_ms / this->TauD_ms) + exp(-t_ms / this->TauD2_ms) +
            exp(-t_ms / this->TauD3_ms));
};

std::vector<float> Receptor::GSynT_pS_MXH(std::vector<float> t_ms) {
    if (this->TauR_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauR_ms).");
    if (this->TauD_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD_ms).");
    if (this->TauD2_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD2_ms).");
    if (this->TauD3_ms == 0.0)
        throw std::overflow_error("Cannot divide by zero (TauD3_ms).");
    if (this->ANorm == 0.0)
        throw std::overflow_error("Cannot divide by zero (ANorm).");

    std::vector<float> calculated = {};
    for (float t_msVal : t_ms) {
        assert(t_msVal >= 0.0);
        calculated.emplace_back(
            this->GPeak_pS / this->ANorm *
            pow(this->X, 1.0 - exp(-t_msVal / this->TauR_ms)) * this->D1 *
            (exp(-t_msVal / this->TauD_ms) + exp(-t_msVal / this->TauD2_ms) +
             exp(-t_msVal / this->TauD3_ms)));
    }
    return calculated;
};

//! For analytical solution see Roth & van Rossum (2009).
float Receptor::FindANorm(bool verbose) {

    float aNormBottom = 0.0, aNormTop = 2.0;
    std::vector<float> t_ms = {};

    for (size_t i = 0; i < _FIND_ANORM_NUM_TIMESTEPS; ++i)
        t_ms.emplace_back(i * 100.0 /
                          static_cast<float>(_FIND_ANORM_NUM_TIMESTEPS));

    for (size_t i = 0; i < _FIND_ANORM_MAX_ITERS; ++i) {
        std::vector<float> GSynT_pS = this->GSynT_pS_DoubleExponential(t_ms);
        float GDiff = *(std::max_element(GSynT_pS.begin(), GSynT_pS.end())) -
                      this->GPeak_pS;

        this->GDiffs.emplace_back(GDiff);
        this->ANorms.emplace_back(this->ANorm);

        if (abs(GDiff) < _FIND_ANORM_CONVERGENCE_TOL)
            return this->ANorm;

        if (GDiff > 0.0) {
            aNormBottom = this->ANorm;
            this->ANorm = (aNormTop + this->ANorm) / 2.0;
            continue;
        }
        aNormTop = this->ANorm;
        this->ANorm = (aNormBottom + this->ANorm) / 2.0;
    }

    // Log message
    std::cout << "Did not converge in " << _FIND_ANORM_MAX_ITERS
              << " iterations.\n";
    return this->ANorm;
};

}; // namespace Receptors
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
