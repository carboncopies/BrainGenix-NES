#include <Simulator/Structs/SignalFunctions.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {

//! Double exponential expression used in calculation of post-synaptic potential
//! contributions.
float DoubleExponentExpr(float amp, float tauRise, float tauDecay, float tDiff) {
    assert(tauRise >= 0.0 && tauDecay >= 0.0);

    if (tDiff < 0) return 0.0;

    return amp * (-exp(-tDiff / tauRise) + exp(-tDiff / tauDecay));
}

// One-dimensional convolution.
// For the sake of efficiency, the convolved vector must already have been
// prepared to the size: signal.size()+kernel.size()-1.
// Also, for the sake of efficiency, we store and provide the already reversed
// kernel instead of reversing it here each time.
bool Convolve1D(const std::deque<float> & signal, const std::vector<float> & reversed_kernel, std::vector<float> & convolved) {
    //std::reverse(kernel.begin(), kernel.end());

    int start = 1 - static_cast<int>(reversed_kernel.size());
    int end = signal.size();
    unsigned int convolved_idx = 0;

    for (int i = start; i < end; ++i) {

        int jBegin = std::max(0, i);
        int jEnd = std::min(i + reversed_kernel.size(), signal.size());
        int kBegin = std::max(-i, 0);
        int kEnd = signal.size() + 1;
        if (i > (signal.size() - reversed_kernel.size())) kEnd -= i;

        // Dot product
        float sum = 0.0;
        int j = jBegin, k = kBegin;
        while ((j < jEnd) && (k < kEnd)) {
            sum += signal[j] * reversed_kernel[k];
            j++;
            k++;
        }

        convolved[convolved_idx++] = sum;
    }
    return true;
}

}; // namespace SignalFunctions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
