//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the GeometryCollection struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <variant>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Box.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Geometries {


enum GeometryVariantOrder {
    GEOMETRY_SPHERE=0,
    GEOMETRY_CYLINDER,
    GEOMETRY_BOX
};

/**
 * @brief This struct contains the various geometries used in the simulation. It has a vector containing each of the geometries.
 * For now, the "id" of the geometry, is simply its index minus an offset (cylinders are -1000000, boxes -2000000, etc.)
 * 
 */
struct GeometryCollection {

    // std::vector<Sphere> Spheres; /**Vector of spheres owned by the simulation.*/
    // std::vector<Cylinder> Cylinders; /**Vector of cylinders owned by the simulation*/
    // std::vector<Box> Boxes; /**Vector of Boxes owned by the simulation*/

    std::vector<std::variant<Sphere, Cylinder, Box>> Geometries; //! Vector of variants, each being a geometry, note that the type order here MUST match the above enum.
    //std::vector<Geometry> Geometries_; // We could instead hold them in here and make use of inheritance and static_cast... (comment by Randal)

    void append(Geometry & geom) {
        //Geometries_.push_back(geom);
        switch (geom.GeometryShape) {
        case GeometrySphere: {
            Geometries.push_back(*static_cast<Sphere*>(&geom));
            break;
        }
        case GeometryCylinder:  {
            Geometries.push_back(*static_cast<Cylinder*>(&geom));
            break;
        }
        case GeometryBox:  {
            Geometries.push_back(*static_cast<Box*>(&geom));
            break;
        }
        }
    }

    size_t Size() const { return Geometries.size(); }

    size_t NextAvailableID() { return Geometries.size(); }

    Sphere & AddSphere(const Vec3D & _Center_um, float _Radius_um) {
        Geometries::Sphere S(_Center_um, _Radius_um);
        S.ID = NextAvailableID();
        Geometries.push_back(S);
        return GetSphere(S.ID);
    }
    Cylinder & AddCylinder(float _End0Radius_um, const Vec3D & _End0Pos_um, float _End1Radius_um, const Vec3D & _End1Pos_um) {
        Geometries::Cylinder C(_End0Radius_um, _End0Pos_um, _End1Radius_um, _End1Pos_um);
        C.ID = NextAvailableID();
        Geometries.push_back(C);
        return GetCylinder(C.ID);
    }
    Box & AddBox(const Vec3D & _Center_um, const Vec3D & _Dims_um, const Vec3D & _Rotations_rad) {
        Geometries::Box B(_Center_um, _Dims_um, _Rotations_rad);
        B.ID = NextAvailableID();
        Geometries.push_back(B);
        return GetBox(B.ID);
    }
    Box & AddBox(const Vec3D & _Center_um, const Vec3D & _Dims_um) {
        Geometries::Box B(_Center_um, _Dims_um);
        B.ID = NextAvailableID();
        Geometries.push_back(B);
        return GetBox(B.ID);
    }
    Box & AddBox() {
        Geometries::Box B;
        B.ID = NextAvailableID();
        Geometries.push_back(B);
        return GetBox(B.ID);
    }

    GeometryShapeEnum GetShapeType(size_t idx) const {
        return std::visit([](auto&& arg){ return arg.GeometryShape; }, Geometries.at(idx));
    }
    bool IsSphere(size_t idx) const { return GetShapeType(idx) == GeometrySphere; }
    bool IsCylinder(size_t idx) const { return GetShapeType(idx) == GeometryCylinder; }
    bool IsBox(size_t idx) const { return GetShapeType(idx) == GeometryBox; }

    Box & GetBox(size_t idx) {
        return std::get<Box>(Geometries.at(idx));
    }
    Sphere & GetSphere(size_t idx) {
        return std::get<Sphere>(Geometries.at(idx));
    }
    Cylinder & GetCylinder(size_t idx) {
        return std::get<Cylinder>(Geometries.at(idx));
    }

    Geometry * GetGeometry(size_t idx) {
        if (idx >= Geometries.size()) return nullptr;

        if (IsSphere(idx)) return &GetSphere(idx);
        if (IsCylinder(idx)) return &GetCylinder(idx);
        if (IsBox(idx)) return &GetBox(idx);
        return nullptr;
    }
};

}; // Close Namespace Geometries
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
