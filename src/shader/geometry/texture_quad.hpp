/// @file texture_quad.hpp
/// @author Shane
/// @date Created: 2026-02-19
/// @date Updated: 2026-02-19
/// @brief Renders textured quads with position, dimensions, and subtexture support.
///        Provides functionality to render any portion of a texture to any
///        screen region using OpenGL.

#ifndef CRIDGEON_SHADER_TEXTURE_QUAD_HPP
#define CRIDGEON_SHADER_TEXTURE_QUAD_HPP

namespace cridgeon {
namespace Render {
    /// @brief Renders a textured quad to the screen.
    /// @param textureID The OpenGL texture ID to render.
    /// @param x The x position on screen.
    /// @param y The y position on screen.
    /// @param w The width of the rendered quad.
    /// @param h The height of the rendered quad.
    /// @param subX The x offset in the texture (0.0 to 1.0).
    /// @param subY The y offset in the texture (0.0 to 1.0).
    /// @param subW The width of the subtexture region (0.0 to 1.0).
    /// @param subH The height of the subtexture region (0.0 to 1.0).
    /// @param r Red tint component (0.0 to 1.0).
    /// @param g Green tint component (0.0 to 1.0).
    /// @param b Blue tint component (0.0 to 1.0).
    /// @param a Alpha component (0.0 to 1.0).
    void textureQuad(unsigned int textureID, 
                     float x, float y, float w, float h,
                     float subX = 0.0f, float subY = 0.0f, 
                     float subW = 1.0f, float subH = 1.0f,
                     float r = 1.0f, float g = 1.0f, 
                     float b = 1.0f, float a = 1.0f);
    
    /// @brief Clean up texture quad rendering resources.
    void _destroyTextureQuad();
} // namespace Render
} // namespace cridgeon

#endif // CRIDGEON_SHADER_TEXTURE_QUAD_HPP