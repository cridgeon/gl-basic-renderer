/// @file texture.hpp
/// @author Shane
/// @date Created: 2026-02-19
/// @brief OpenGL texture class for handling texture creation, loading, and 
///        destruction. Supports 2D textures with common image formats and
///        provides utilities for texture binding and parameter management.

#pragma once

#include <string>
#include <glad/gl.h>

namespace cridgeon {
    class Texture {
    public:
        enum class Format {
            RGB,
            RGBA,
            DEPTH,
            DEPTH_STENCIL
        };
        
        enum class Type {
            TEXTURE_2D,
            TEXTURE_CUBE_MAP
        };
        
        enum class Filter {
            NEAREST,
            LINEAR,
            NEAREST_MIPMAP_NEAREST,
            LINEAR_MIPMAP_NEAREST,
            NEAREST_MIPMAP_LINEAR,
            LINEAR_MIPMAP_LINEAR
        };
        
        enum class Wrap {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER
        };

        Texture();
        ~Texture();

        // Disable copy constructor and assignment operator
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Enable move constructor and assignment operator
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        /// @brief Creates an empty texture with specified dimensions and format.
        /// @param width The width of the texture in pixels.
        /// @param height The height of the texture in pixels.
        /// @param format The internal format of the texture.
        /// @param type The type of texture to create.
        /// @returns True if texture creation succeeded, false otherwise.
        bool create(int width, int height, Format format = Format::RGBA, 
                   Type type = Type::TEXTURE_2D);

        /// @brief Loads texture data from an image file.
        /// @param file_path The path to the image file to load.
        /// @param flip_vertically Whether to flip the image vertically during load.
        /// @returns True if texture loading succeeded, false otherwise.
        bool loadFromFile(const std::string& file_path, bool flip_vertically = true);

        /// @brief Loads texture data from raw pixel data.
        /// @param data Pointer to the raw pixel data.
        /// @param width The width of the texture in pixels.
        /// @param height The height of the texture in pixels.
        /// @param format The format of the input data.
        /// @returns True if texture loading succeeded, false otherwise.
        bool loadFromData(const unsigned char* data, int width, int height, 
                         Format format = Format::RGBA);

        /// @brief Binds the texture to the specified texture unit.
        /// @param texture_unit The texture unit to bind to (0-31).
        void bind(unsigned int texture_unit = 0) const;

        /// @brief Unbinds any texture from the specified texture unit.
        /// @param texture_unit The texture unit to unbind from.
        static void unbind(unsigned int texture_unit = 0);

        /// @brief Sets the texture filtering parameters.
        /// @param min_filter The minification filter to use.
        /// @param mag_filter The magnification filter to use.
        void setFilter(Filter min_filter, Filter mag_filter);

        /// @brief Sets the texture wrapping parameters.
        /// @param wrap_s The wrap mode for the S coordinate (horizontal).
        /// @param wrap_t The wrap mode for the T coordinate (vertical).
        void setWrap(Wrap wrap_s, Wrap wrap_t);

        /// @brief Generates mipmaps for the texture.
        void generateMipmaps();

        /// @brief Gets the OpenGL texture ID.
        /// @returns The OpenGL texture ID, or 0 if invalid.
        unsigned int getID() const { return texture_id; }

        /// @brief Gets the texture width.
        /// @returns The width in pixels, or 0 if not loaded.
        int getWidth() const { return width; }

        /// @brief Gets the texture height.
        /// @returns The height in pixels, or 0 if not loaded.
        int getHeight() const { return height; }

        /// @brief Gets the texture's internal format.
        /// @returns The internal format of the texture.
        Format getFormat() const { return internal_format; }

        /// @brief Gets the number of channels for the texture's format.
        /// @returns Number of channels (3 for RGB, 4 for RGBA, 1 for depth).
        int getChannelCount() const;

        /// @brief Checks if the texture is valid and loaded.
        /// @returns True if the texture has been successfully created/loaded.
        bool isValid() const { return texture_id != 0; }

        /// @brief Reads the texture pixel data into a supplied buffer.
        /// @param data Pointer to the buffer where pixel data will be written.
        ///             Buffer must be large enough to hold width * height * getChannelCount() bytes.
        /// @param format The format to read the pixel data in. If not specified,
        ///               uses the texture's internal format.
        /// @returns True if the read succeeded, false otherwise.
        bool readPixels(unsigned char* data, Format format) const;
        bool readPixels(unsigned char* data) const;

        /// @brief Saves the texture to an image file.
        /// @param file_path The path to save the image file. Extension determines format
        ///                  (.png, .bmp, .tga, .jpg supported).
        /// @param flip_vertically Whether to flip the image vertically when saving
        ///                        (default true to match standard image orientation).
        /// @param quality JPEG quality (1-100) when saving as .jpg (default 90).
        /// @returns True if save succeeded, false otherwise.
        bool saveToFile(const std::string& file_path, bool flip_vertically = true, 
                       int quality = 90) const;

    private:
        unsigned int texture_id;     // OpenGL texture ID
        Type texture_type;           // Type of texture (2D, cube map, etc.)
        Format internal_format;      // Internal texture format
        int width;                   // Texture width in pixels
        int height;                  // Texture height in pixels

        /// @brief Converts internal Format enum to OpenGL format constant.
        /// @param format The format to convert.
        /// @returns The corresponding OpenGL format constant.
        GLenum formatToGL(Format format) const;

        /// @brief Converts internal Filter enum to OpenGL filter constant.
        /// @param filter The filter to convert.
        /// @returns The corresponding OpenGL filter constant.
        GLenum filterToGL(Filter filter) const;

        /// @brief Converts internal Wrap enum to OpenGL wrap constant.
        /// @param wrap The wrap mode to convert.
        /// @returns The corresponding OpenGL wrap constant.
        GLenum wrapToGL(Wrap wrap) const;

        /// @brief Converts internal Type enum to OpenGL target constant.
        /// @param type The type to convert.
        /// @returns The corresponding OpenGL target constant.
        GLenum typeToGL(Type type) const;
    };
}