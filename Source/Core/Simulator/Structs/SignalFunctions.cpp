
#include <cassert>
#include <algorithm>
#include <Simulator/Structs/SignalFunctions.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace SignalFunctions {


//! Double exponential expression used in calculation of post-synaptic potential contributions.
float DoubleExponentExpr(float amp, float tauRise, float tauDecay, float tDiff) {
    assert(tauRise>=0.0 && tauDecay>=0.0);

    if (tDiff<0) return 0.0;
    return amp * ( exp(-tDiff/tauRise) + exp(-tDiff/tauDecay) );
}

// One-dimensional convolution.
std::vector<float> Convolve1D(std::vector<float> signal, std::vector<float> kernel) {
    std::vector<float> result{};
    
    std::reverse(kernel.begin(), kernel.end());
    
    int start = 1-static_cast<int>(kernel.size());
    int end = signal.size();

    for (int i=start; i<end; ++i) {
        
        auto signalBeginIt = signal.begin() + std::max(0, i);
        auto signalEndIt = signal.begin() + std::min(
                                i + kernel.size(), signal.size()) + 1;
        auto kernelBeginIt = kernel.begin() + std::max(-i, 0);
        auto kernelEndIt = kernel.begin() + signal.size() + 1;
        if (i > signal.size() - kernel.size())
           kernelEndIt -= i;
        auto signalIt = signalBeginIt, kernelIt = kernelBeginIt;
        float sum{};

        // Dot product
        while ((signalIt < signalEndIt) && (kernelIt < kernelEndIt)) {
            sum += (*signalIt) * (*kernelIt);
            kernelIt++;
            signalIt++;
        }
        result.emplace_back(sum);
    }
    return result;
}}; // Close Namespace 
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
