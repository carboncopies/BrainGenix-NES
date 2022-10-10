//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <PyBind11SpotLightClass.h>



PYBIND11_EMBEDDED_MODULE(SpotLight, m) {
    m.doc() = "Allows the script to manipulate the Spot light that it's attached to.";
}