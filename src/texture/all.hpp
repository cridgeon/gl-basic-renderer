/// @file all.hpp
/// @author Shane
/// @date Created: 2026-02-19
/// @brief Includes all texture-related headers and provides utility functions
///        for texture management across the gl-basic-renderer library.

#pragma once

#include "texture.hpp"

namespace cridgeon {
    /// @brief Cleanup function for texture resources.
    ///        Currently no global texture cleanup needed as textures are RAII.
    inline void destroyAllTextures() {
        // Individual textures handle their own cleanup via RAII
        // This function exists for consistency with other modules
    }

    /// @brief Example function demonstrating texture usage.
    ///        Defined in texture_example.cpp
    void texture_usage_example();
}