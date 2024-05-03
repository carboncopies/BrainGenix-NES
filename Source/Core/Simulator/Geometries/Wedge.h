//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the wedge struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

#include <cmath>
#include <vector>
#include <tuple>

#include <glm/mat3x3.hpp>

#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/VecTools.h>

#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>

#include <VSDA/Common/Structs/WorldInfo.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {

/**
 * @brief This struct defines a wedge geometry used in creation of components of
 * virtual brain samples.
 *
 */
struct Wedge : Geometry {

    Vec3D End0Pos_um; //! Position of the first end of the wedge in
                      //! micrometers (relative to origin).
    Vec3D End1Pos_um{1.0, 0.0, 0.0}; //! Position of the second end of the wedge in
                                     //! micrometers (relative to origin).
    float End0Width_um = 1.0; //! Width in micrometers of the first end of the wedge.
    float End1Width_um = 1.0; //! Width in micrometers of the second end of the wedge.
    float End0Height_um = 1.0; //! Height in micrometers of the first end of the wedge.
    float End1Height_um = 1.0; //! Height in micrometers of the second end of the wedge.

    // Constructors
    Wedge();
    Wedge(const Vec3D & _End0Pos_um, const Vec3D & _End1Pos_um, float _End0Width_um, float _End0Height_um, float _End1Width_um, float _End1Height_um);

    //! Renders the wedge in 3D.
    //void Show();

    //! Returns the volume of the wedge in micrometer^3.
    float Volume_um3() { return 0.0; } // *** FIX THIS!

    //! Returns the distance from the origin along wedge axis at specified
    //! fraction of height distance from end 0.
    float RAtPosition_um(float position) { return 0.0; } // *** FIX THIS!


    //! Returns the bounding box
    virtual BoundingBox GetBoundingBox(VSDA::WorldInfo& _WorldInfo) { return BoundingBox(); } // ** FIX THIS!
    virtual bool IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo) { return true; } // ***FIX THIS!
    virtual bool IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo) { return true; } // ***FIX THIS!

    //! Returns a point cloud that can be used to fill voxels representing the cylinder.
    // std::vector<Vec3D> GetPointCloud(float _VoxelScale);


    //! Gets the rotation in radians with respect to the x, y and z axes
    Vec3D GetRotation_rad() const { return Vec3D(); } // *** FIX THIS!


    /**
     * @brief Writes the shape at the current size into the voxel array.
     * 
     * @param _VoxelScale 
     * @param _Array 
     */
    // void WriteToVoxelArray(VoxelArray* _Array, VSDA::WorldInfo& _WorldInfo);
    void WriteToVoxelArray(VSDA::Calcium::VoxelArray* _Array, VSDA::Calcium::VoxelType _VoxelInfo, VSDA::WorldInfo& _WorldInfo) {} // *** FIX THIS!

};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
