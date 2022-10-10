//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <PyBind11ModelClass.h>



PYBIND11_EMBEDDED_MODULE(Model, m) {
    m.doc() = "Allows the script to manipulate the model that it's attached to.";
}