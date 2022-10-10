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

        enum ShadowUpdateMode {

            ERS_SHADOW_UPDATE_MODE_DISABLED,
            ERS_SHADOW_UPDATE_MODE_RANDOM,
            ERS_SHADOW_UPDATE_MODE_CONSECUTIVE,
            ERS_SHADOW_UPDATE_MODE_DISTANCE_PRIORITIZED,
            ERS_SHADOW_UPDATE_MODE_ALL

        };

    }

}