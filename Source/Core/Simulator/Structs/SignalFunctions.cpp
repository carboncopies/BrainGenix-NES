#include <Simulator/Structs/SignalFunctions.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {

//! Double exponential expression used in calculation of post-synaptic potential
//! contributions.
float DoubleExponentExpr(float amp, float tauRise, float tauDecay,
                         float tDiff) {
    assert(tauRise >= 0.0 && tauDecay >= 0.0);

    if (tDiff < 0)
        return 0.0;
    return amp * (exp(-tDiff / tauRise) + exp(-tDiff / tauDecay));
}

// One-dimensional convolution.
std::vector<float> Convolve1D(std::vector<float> signal,
                              std::vector<float> kernel) {
    std::vector<float> result{};

    std::reverse(kernel.begin(), kernel.end());

    int start = 1 - static_cast<int>(kernel.size());
    int end = signal.size();

    for (int i = start; i < end; ++i) {

        int jBegin = std::max(0, i);
        int jEnd = std::min(i + kernel.size(), signal.size());
        int kBegin = std::max(-i, 0);
        int kEnd = signal.size() + 1;
        if (i > signal.size() - kernel.size())
            kEnd -= i;
        int j = jBegin, k = kBegin;
        float sum = 0.0;

        // Dot product
        while ((j < jEnd) && (k < kEnd)) {
            sum += signal[j] * kernel[k];
            j++;
            k++;
        }
        result.emplace_back(sum);
    }
    return result;
}

}; // namespace SignalFunctions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
