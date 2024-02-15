#include <Simulator/Structs/CalciumImaging.h>
#include <Simulator/Structs/Simulation.h>
#include <Simulator/BallAndStick/BSNeuron.h>

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
    ImagingInterval_ms = _Params.IndicatorDecayTime_ms + 10.0;
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

void CalciumImaging::InitializeFluorescenceKernel(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params) {
    FluorescenceKernel.clear();
    float t = 0.0;
    float kernel_ms = 2.0 * (_Params.IndicatorRiseTime_ms + _Params.IndicatorDecayTime_ms);
    float pulse_samples = _Params.IndicatorDecayTime_ms;
    float amp = 1.0 / pulse_samples;
    while (t < kernel_ms) {
        float k = DelayedPulse(amp, _Params.IndicatorRiseTime_ms, _Params.IndicatorDecayTime_ms, t);
        FluorescenceKernel.emplace_back(k);
        t += _Sim->Dt_ms;
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
        for (auto & neuron_ptr : Sim->Neurons) {
            static_cast<BallAndStick::BSNeuron*>(neuron_ptr.get())->UpdateConvolvedFIFO(FluorescenceKernel);
        }

    } else {
        // For specified fluorescing neurons set.
        for (auto & neuron_id : _Params.FlourescingNeuronIDs_) if (neuron_id < Sim->Neurons.size()) {
            static_cast<BallAndStick::BSNeuron*>(Sim->Neurons.at(neuron_id).get())->UpdateConvolvedFIFO(FluorescenceKernel);
        }
    }

    // num_samples++;

    // Note: Unlike the prototype code, we don't implement generating during sim at this time.
}

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
