/// @file texture.cpp
/// @author Shane
/// @date Created: 2026-02-19
/// @brief Implementation of OpenGL texture class with comprehensive texture
///        management functionality including loading from files and raw data,
///        parameter setting, and proper resource cleanup.

#include "texture.hpp"
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace cridgeon {
    
    Texture::Texture() 
        : texture_id(0)
        , texture_type(Type::TEXTURE_2D)
        , internal_format(Format::RGBA)
        , width(0)
        , height(0) {
    }

    Texture::~Texture() {
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
            texture_id = 0;
        }
    }

    Texture::Texture(Texture&& other) noexcept
        : texture_id(other.texture_id)
        , texture_type(other.texture_type)
        , internal_format(other.internal_format)
        , width(other.width)
        , height(other.height) {
        other.texture_id = 0;
        other.width = 0;
        other.height = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            // Clean up current texture
            if (texture_id != 0) {
                glDeleteTextures(1, &texture_id);
            }
            
            // Move from other
            texture_id = other.texture_id;
            texture_type = other.texture_type;
            internal_format = other.internal_format;
            width = other.width;
            height = other.height;
            
            // Reset other
            other.texture_id = 0;
            other.width = 0;
            other.height = 0;
        }
        return *this;
    }

    bool Texture::create(int width, int height, Format format, Type type) {
        if (width <= 0 || height <= 0) {
            std::cerr << "Error: Invalid texture dimensions: " << width << "x" << height << std::endl;
            return false;
        }

        // Clean up existing texture if any
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
        }

        glGenTextures(1, &texture_id);
        if (texture_id == 0) {
            std::cerr << "Error: Failed to generate texture" << std::endl;
            return false;
        }

        this->width = width;
        this->height = height;
        this->texture_type = type;
        this->internal_format = format;

        GLenum gl_target = typeToGL(type);
        GLenum gl_format = formatToGL(format);
        
        glBindTexture(gl_target, texture_id);

        // Create empty texture with specified format
        if (type == Type::TEXTURE_2D) {
            glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, 
                        gl_format == GL_DEPTH_COMPONENT ? GL_DEPTH_COMPONENT : GL_RGBA, 
                        GL_UNSIGNED_BYTE, nullptr);
        }

        // Set default parameters
        glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(gl_target, 0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error: OpenGL error during texture creation: " << error << std::endl;
            glDeleteTextures(1, &texture_id);
            texture_id = 0;
            return false;
        }

        return true;
    }

    bool Texture::loadFromFile(const std::string& file_path, bool flip_vertically) {
        // Set stb_image to flip loaded image's on the y-axis if requested
        stbi_set_flip_vertically_on_load(flip_vertically);

        int channels;
        unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cerr << "Error: Failed to load texture from file: " << file_path << std::endl;
            std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
            return false;
        }

        // Determine format based on channels
        Format format;
        switch (channels) {
            case 1: format = Format::RGB; break;  // Treat grayscale as RGB
            case 3: format = Format::RGB; break;
            case 4: format = Format::RGBA; break;
            default:
                std::cerr << "Error: Unsupported number of channels: " << channels << std::endl;
                stbi_image_free(data);
                return false;
        }

        bool result = loadFromData(data, width, height, format);
        stbi_image_free(data);

        if (!result) {
            std::cerr << "Error: Failed to load texture data from file: " << file_path << std::endl;
        }

        return result;
    }

    bool Texture::loadFromData(const unsigned char* data, int width, int height, Format format) {
        if (!data || width <= 0 || height <= 0) {
            std::cerr << "Error: Invalid texture data or dimensions" << std::endl;
            return false;
        }

        // Clean up existing texture if any
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
        }

        glGenTextures(1, &texture_id);
        if (texture_id == 0) {
            std::cerr << "Error: Failed to generate texture" << std::endl;
            return false;
        }

        this->width = width;
        this->height = height;
        this->texture_type = Type::TEXTURE_2D;
        this->internal_format = format;

        glBindTexture(GL_TEXTURE_2D, texture_id);

        GLenum gl_internal_format = formatToGL(format);
        GLenum gl_format = (format == Format::RGBA) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, 
                    gl_format, GL_UNSIGNED_BYTE, data);

        // Set default parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error: OpenGL error during texture loading: " << error << std::endl;
            glDeleteTextures(1, &texture_id);
            texture_id = 0;
            return false;
        }

        return true;
    }

    void Texture::bind(unsigned int texture_unit) const {
        if (texture_id == 0) {
            std::cerr << "Warning: Attempting to bind invalid texture" << std::endl;
            return;
        }

        if (texture_unit > 31) {
            std::cerr << "Warning: Texture unit " << texture_unit << " may not be supported" << std::endl;
        }

        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(typeToGL(texture_type), texture_id);
    }

    void Texture::unbind(unsigned int texture_unit) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::setFilter(Filter min_filter, Filter mag_filter) {
        if (texture_id == 0) {
            std::cerr << "Warning: Attempting to set filter on invalid texture" << std::endl;
            return;
        }

        GLenum gl_target = typeToGL(texture_type);
        glBindTexture(gl_target, texture_id);
        glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, filterToGL(min_filter));
        glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, filterToGL(mag_filter));
        glBindTexture(gl_target, 0);
    }

    void Texture::setWrap(Wrap wrap_s, Wrap wrap_t) {
        if (texture_id == 0) {
            std::cerr << "Warning: Attempting to set wrap on invalid texture" << std::endl;
            return;
        }

        GLenum gl_target = typeToGL(texture_type);
        glBindTexture(gl_target, texture_id);
        glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, wrapToGL(wrap_s));
        glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, wrapToGL(wrap_t));
        glBindTexture(gl_target, 0);
    }

    void Texture::generateMipmaps() {
        if (texture_id == 0) {
            std::cerr << "Warning: Attempting to generate mipmaps on invalid texture" << std::endl;
            return;
        }

        GLenum gl_target = typeToGL(texture_type);
        glBindTexture(gl_target, texture_id);
        glGenerateMipmap(gl_target);
        glBindTexture(gl_target, 0);
    }

    GLenum Texture::formatToGL(Format format) const {
        switch (format) {
            case Format::RGB:          return GL_RGB;
            case Format::RGBA:         return GL_RGBA;
            case Format::DEPTH:        return GL_DEPTH_COMPONENT;
            case Format::DEPTH_STENCIL: return GL_DEPTH_STENCIL;
            default:                   return GL_RGBA;
        }
    }

    GLenum Texture::filterToGL(Filter filter) const {
        switch (filter) {
            case Filter::NEAREST:               return GL_NEAREST;
            case Filter::LINEAR:                return GL_LINEAR;
            case Filter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            case Filter::LINEAR_MIPMAP_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
            case Filter::NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
            case Filter::LINEAR_MIPMAP_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
            default:                            return GL_LINEAR;
        }
    }

    GLenum Texture::wrapToGL(Wrap wrap) const {
        switch (wrap) {
            case Wrap::REPEAT:          return GL_REPEAT;
            case Wrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case Wrap::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
            case Wrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            default:                    return GL_REPEAT;
        }
    }

    GLenum Texture::typeToGL(Type type) const {
        switch (type) {
            case Type::TEXTURE_2D:      return GL_TEXTURE_2D;
            case Type::TEXTURE_CUBE_MAP: return GL_TEXTURE_CUBE_MAP;
            default:                    return GL_TEXTURE_2D;
        }
    }

}