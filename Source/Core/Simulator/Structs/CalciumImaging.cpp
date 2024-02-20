#include <Simulator/Structs/CalciumImaging.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/SignalFunctions.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

// CalciumImaging::CalciumImaging(Simulator::Simulation* _Sim): Sim(_Sim) {
//     assert(_Sim != nullptr);
// };

// CalciumImaging::CalciumImaging(CalciumImaging & CaImg):
//     Sim(CaImg.Sim), Name(CaImg.Name), ID(CaImg.ID),
//     FluorescingNeurons(CaImg.FluorescingNeurons),
//     CalciumIndicator(CaImg.CalciumIndicator),
//     IndicatorRise_ms(CaImg.IndicatorRise_ms),
//     IndicatorDecay_ms(CaImg.IndicatorDecay_ms),
//     IndicatorInterval_ms(CaImg.IndicatorInterval_ms),
//     ImagingInterval_ms(CaImg.ImagingInterval_ms), 
//     VoxelSpaceSide_px(CaImg.VoxelSpaceSide_px),
//     GenerateDuringSim(CaImg.GenerateDuringSim),
//     Center_um(CaImg.Center_um),
//     Half(CaImg.Half),
//     Dx(CaImg.Dx), Dy(CaImg.Dy), Dz(CaImg.Dz)
// {

//     Init();

// }

void CalciumImaging::Init(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params) {
    // *** Voxel_um = GetVoxelSize_um();
    // *** IncludeComponents = GetVisibleComponentsList();
    // SetImageSize();
    // InstantiateVoxelSpace();
    // InitializeDepthDimming();
    // InitializeProjectionCircles();
    InitializeFluorescenceKernel(_Sim, _Params);
    InitializeFluorescingNeuronFIFOs(_Sim, _Params);
    ImagingInterval_ms = _Params.ImagingInterval_ms;
}

// void CalciumImaging::SetImageSize() {
//     // *** Do we need this, or is it taken care of by voxel code?
// }

// void CalciumImaging::InstantiateVoxelSpace() {
//     // *** Do we need this, or is it taken care of by voxel code?
// }

// void CalciumImaging::InitializeDepthDimming() {
//     // *** Do we need this, or is it taken care of by voxel code?
// }

// void CalciumImaging::InitializeProjectionCircles() {
//     // *** Do we need this, or is it taken care of by voxel code?
// }

float DelayedPulse(float amp, float tau_delay, float tau_pulse, float tdiff) {
    if (tdiff < tau_delay) return 0.0;
    if (tdiff < (tau_delay + tau_pulse)) return amp;
    return 0.0;
}

/**
 * See the example in VBP SignalFunctions.py about the following steps:
 * 1. Prepare a kernel using the kernel shape chosen (e.g. double exponential, rectangle pulse).
 * 2. Scale the kernel so that its convolution effect on an action potential is approximately normalized.
 * 3. Reverse the kernel (which would otherwise be the first step of every convolution).
 */
void CalciumImaging::InitializeFluorescenceKernel(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params) {
    FluorescenceKernel.clear();
    float t = 0.0;
    float kernel_ms = 2.0 * (_Params.IndicatorRiseTime_ms + _Params.IndicatorDecayTime_ms);
    //float pulse_samples = _Params.IndicatorDecayTime_ms;
    //float amp = 1.0 / pulse_samples;
    bool use_dblexp_kernel = true; // *** Could make this selectable.
    float v_sum = 0.0;
    while (t < kernel_ms) {
        // for a period of length rise time + decay time, produce a pulse of fixed height amp
        float k;
        if (use_dblexp_kernel) {
            k = SignalFunctions::DoubleExponentExpr(1.0, _Params.IndicatorRiseTime_ms, _Params.IndicatorDecayTime_ms, t);
        } else {
            k = DelayedPulse(1.0, _Params.IndicatorRiseTime_ms, _Params.IndicatorDecayTime_ms, t);
        }
        FluorescenceKernel.emplace_back(k);
        v_sum += k;
        t += _Sim->Dt_ms;
    }
    // Scale and reverse the kernel:
    size_t kernelsize = FluorescenceKernel.size();
    ReversedFluorescenceKernel.resize(kernelsize, 0.0);
    // for (size_t i; i < kernelsize; i++) {
    //     ReversedFluorescenceKernel[i] = FluorescenceKernel[(kernelsize-i)-1]/v_sum;
    // }
    // *** TESTING WITHOUT REVERSAL (see SignalFunctions.py)
    for (size_t i  = 0; i < kernelsize; i++) {
        ReversedFluorescenceKernel[i] = FluorescenceKernel[i]/v_sum;
    }

}

void CalciumImaging::InitializeFluorescingNeuronFIFOs(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params) {
    // *** TODO: Set different FIFO sizes for different GCaMP types.
    float FIFO_ms = 4.0 * (_Params.IndicatorRiseTime_ms + _Params.IndicatorDecayTime_ms);
    float FIFO_dt_ms = _Sim->Dt_ms;
    if (_Params.FlourescingNeuronIDs_.empty()) {
        // All neurons.
        for (auto & neuron_ptr : _Sim->Neurons) {
            static_cast<BallAndStick::BSNeuron*>(neuron_ptr.get())->SetFIFO(FIFO_ms, FIFO_dt_ms, FluorescenceKernel.size());
        }

    } else {
        // Specified neurons subset.
        for (auto & neuron_id : _Params.FlourescingNeuronIDs_) if (neuron_id < _Sim->Neurons.size()) {
            static_cast<BallAndStick::BSNeuron*>(_Sim->Neurons.at(neuron_id).get())->SetFIFO(FIFO_ms, FIFO_dt_ms, FluorescenceKernel.size());
        }
    }

}

void CalciumImaging::Record(float t_ms, Simulation* Sim, NES::VSDA::Calcium::CaMicroscopeParameters& _Params) {
    assert(t_ms >= 0.0);
    // Check if we have reached the next sample time:
    if (!TRecorded_ms.empty()) {
        if ((t_ms - TRecorded_ms.back()) < ImagingInterval_ms) return;
    }

    // Make a recording:
    TRecorded_ms.emplace_back(t_ms);

    if (_Params.FlourescingNeuronIDs_.empty()) {
        // All neurons fluoresce.
        for (size_t i = 0; i < Sim->Neurons.size(); i++) {
            std::shared_ptr<Simulator::CoreStructs::Neuron> ThisNeuron = Sim->Neurons[i];
            assert(ThisNeuron != nullptr);
            static_cast<BallAndStick::BSNeuron*>(ThisNeuron.get())->UpdateConvolvedFIFO(ReversedFluorescenceKernel);
        }

    } else {
        // For specified fluorescing neurons set.
        for (auto & neuron_id : _Params.FlourescingNeuronIDs_) if (neuron_id < Sim->Neurons.size()) {
            static_cast<BallAndStick::BSNeuron*>(Sim->Neurons.at(neuron_id).get())->UpdateConvolvedFIFO(ReversedFluorescenceKernel);
        }
    }

    // num_samples++;

    // Note: Unlike the prototype code, we don't implement generating during sim at this time.
}

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
