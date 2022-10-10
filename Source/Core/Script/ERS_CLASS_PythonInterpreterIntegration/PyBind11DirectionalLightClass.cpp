//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <PyBind11DirectionalLightClass.h>



PYBIND11_EMBEDDED_MODULE(DirectionalLight, m) {
    m.doc() = "Allows the script to manipulate the Directional light that it's attached to.";
}