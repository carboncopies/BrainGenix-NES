//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BrainRegion class, which defines the
    characteristics of a brain region, such as geometric shape and
    physiological content.

    Additional Notes: None
    Date Created: 2023-10-18
*/

#pragma once

#include <memory>

#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Structs/NeuralCircuit.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BrainRegions {

#define RegionLabelLEN 64

struct RegionBase {
    int ID = -1;                       /**ID of the Region*/
    int CircuitID = -1;

    char RegionLabel[RegionLabelLEN] = { 0 };

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nRegionLabel: " << RegionLabel;
        return ss.str();
    }

    void safeset_RegionLabel(const char* s) {
        strncpy(RegionLabel, s, RegionLabelLEN-1);
        RegionLabel[RegionLabelLEN-1] = '\0';
    }
};

/**
 * @brief This class defines the characteristics of a brain region, such as
 * geometric shape and physiological content.
 *
 */
class BrainRegion: public RegionBase {
  public:
    BrainRegion() {}
    BrainRegion(const RegionBase& _Base): RegionBase(_Base) {}
    
    void SetName(const std::string& _Name) { safeset_RegionLabel(_Name.c_str()); }
    std::string Name() const { return RegionLabel; };

    Geometries::Geometry * Shape{nullptr}; // Regular pointers, because the objects are maintained in Simulation.Collection.
    //std::shared_ptr<CoreStructs::NeuralCircuit> Content{};

    virtual void Show(float lineWidth) {};
};

}; // namespace BrainRegions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
