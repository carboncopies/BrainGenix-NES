
#include <cassert>
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

}; // Close Namespace 
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
