/// @file texture_quad.cpp
/// @author Shane
/// @date Created: 2026-02-19
/// @date Updated: 2026-02-19
/// @brief Implementation of textured quad rendering with OpenGL.
///        Handles initialization, rendering, and cleanup of texture quads.

#include "texture_quad.hpp"

#include "rendering_system.hpp"
#include "shader/shader.hpp"
#include "shader/utility.hpp"

namespace cridgeon {
namespace Render {

    static Shader textureQuadShader;
    static bool textureVAOInitialized = false;
    static unsigned int textureVAO = 0;
    static unsigned int textureVBO = 0;
    static unsigned int textureEBO = 0;

    /// @brief Initialize the texture quad VAO and shader if needed.
    static void initializeTextureQuad() {
        if (textureVAOInitialized) return;

        // Create and bind VAO
        glGenVertexArrays(1, &textureVAO);
        glBindVertexArray(textureVAO);

        // Define quad vertices (positions only, texture coords calculated in shader)
        float vertices[] = {
            // Position (will be transformed to rect coordinates)
            0.0f, 0.0f,  // Bottom-left
            1.0f, 0.0f,  // Bottom-right  
            1.0f, 1.0f,  // Top-right
            0.0f, 1.0f   // Top-left
        };

        // Define indices for two triangles
        unsigned int indices[] = {
            0, 1, 2,  // First triangle
            2, 3, 0   // Second triangle
        };

        // Create and set up VBO
        glGenBuffers(1, &textureVBO);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Create and set up EBO
        glGenBuffers(1, &textureEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute (only attribute needed for default.vert)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Load shader (using default.vert)
        if (!textureQuadShader.loadFromFile(
            "resources/shaders/default.vert", 
            "resources/shaders/texture.frag")) {
            std::cerr << "Failed to load texture quad shader" << std::endl;
        }

        textureVAOInitialized = true;
    }

    void textureQuad(unsigned int textureID, 
                     float x, float y, float w, float h,
                     float subX, float subY, float subW, float subH,
                     float r, float g, float b, float a) {
        
        // Initialize if needed
        initializeTextureQuad();

        if (!textureQuadShader.isValid()) {
            std::cerr << "Texture quad shader is not valid" << std::endl;
            return;
        }

        // Use shader program
        textureQuadShader.use();

        // Get rendering system for resolution
        auto& rs = RenderingSystem::getInstance();
        
        // Set uniforms
        glUniform2f(glGetUniformLocation(textureQuadShader.getID(), "resolution"), 
                    static_cast<float>(rs.getWindowWidth()), 
                    static_cast<float>(rs.getWindowHeight()));

        glUniform4f(glGetUniformLocation(textureQuadShader.getID(), "rect"), x, y, w, h);
        glUniform4f(glGetUniformLocation(textureQuadShader.getID(), "subtexture"), subX, subY, subW, subH);
        glUniform4f(glGetUniformLocation(textureQuadShader.getID(), "color"), r, g, b, a);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(textureQuadShader.getID(), "textureSampler"), 0);

        // Bind VAO and draw
        glBindVertexArray(textureVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Clean up
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void _destroyTextureQuad() {
        if (textureVAOInitialized) {
            glDeleteVertexArrays(1, &textureVAO);
            glDeleteBuffers(1, &textureVBO);
            glDeleteBuffers(1, &textureEBO);
            textureQuadShader.destroy();
            textureVAOInitialized = false;
        }
    }

} // namespace Render
} // namespace cridgeon