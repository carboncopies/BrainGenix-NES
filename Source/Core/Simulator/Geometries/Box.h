//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Box struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

#include <cassert>
#include <vector>

// Internal Libraries (BG convention: use <> instead of "")
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
 * @brief Struct representing a cuboidal box geometry.
 *
 */
struct Box : Geometry {

    Vec3D Dims_um{5.0, 10.0, 10.0}; //! Dimensions of the box.
    Vec3D Rotations_rad{0.0, 0.0, 0.0}; //! Rotation of the box (euler angles) in radians.

    //! Constructors
    Box();
    Box(const Vec3D & _Center_um, const Vec3D & _Dims_um);
    Box(const Vec3D & _Center_um, const Vec3D & _Dims_um, const Vec3D & _Rotations_rad);

    //! Renders the box in 3D.
    void Show();

    //! Returns the volume of the box in micrometer^3.
    float Volume_um3();

    //! Returns 1 of n equally sliced subpartitions of the Box
    //! shape, when lined up from left to right along the width.
    std::vector<std::vector<float>> EqualSliceBounds(int nSlices, int slice);

    //! Returns a vector of dimensions of the Box.
    std::vector<float> Sides();


    //! Returns the bounding box
    virtual BoundingBox GetBoundingBox(VSDA::WorldInfo& _WorldInfo);
    virtual bool IsPointInShape(Vec3D _Position_um, VSDA::WorldInfo& _WorldInfo); // not used - bad don't use this it does not do rotation or work at all!!!
    virtual bool IsInsideRegion(BoundingBox _Region, VSDA::WorldInfo& _WorldInfo);

    //! Returns a point cloud that can be used to fill voxels representing the cylinder.
    // std::vector<Vec3D> GetPointCloud(float _VoxelScale);






    /**
     * @brief Writes the shape at the current size into the voxel array.
     * 
     * @param _VoxelScale 
     * @param _Array 
     */
    // void WriteToVoxelArray(VoxelArray* _Array, VSDA::WorldInfo& _WorldInfo);
    void WriteToVoxelArray(VSDA::Calcium::VoxelArray* _Array, VSDA::Calcium::VoxelType _VoxelInfo, VSDA::WorldInfo& _WorldInfo);


};

}; // namespace Geometries
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
