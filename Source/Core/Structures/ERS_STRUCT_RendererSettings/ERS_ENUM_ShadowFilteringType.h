//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


namespace ERS {

    namespace Renderer {

        enum ShadowFilteringType {

            ERS_SHADOW_FILTERING_DISABLED,
            ERS_SHADOW_FILTERING_PCF,
            ERS_SHADOW_FILTERING_POISSON_SAMPLING,
            ERS_SHADOW_FILTERING_STRATIFIED_POISSON_SAMPLING

        };

    }

}