/// @file texture.cpp
/// @author Charlie Ridgeon
/// @date Created: 2026-02-19
/// @brief Implementation of OpenGL texture class with comprehensive texture
///        management functionality including loading from files and raw data,
///        parameter setting, and proper resource cleanup.

#include "texture.hpp"
#include <glad/gl.h>
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>

#include <vector>
#include <algorithm>

namespace cridgeon {

    GLenum formatToGL(Texture::Format format) {
        switch (format) {
            case Texture::Format::RGB:          return GL_RGB;
            case Texture::Format::RGBA:         return GL_RGBA;
            case Texture::Format::DEPTH:        return GL_DEPTH_COMPONENT;
            case Texture::Format::DEPTH_STENCIL: return GL_DEPTH_STENCIL;
            default:                   return GL_RGBA;
        }
    }

    GLenum filterToGL(Texture::Filter filter) {
        switch (filter) {
            case Texture::Filter::NEAREST:               return GL_NEAREST;
            case Texture::Filter::LINEAR:                return GL_LINEAR;
            case Texture::Filter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            case Texture::Filter::LINEAR_MIPMAP_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
            case Texture::Filter::NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
            case Texture::Filter::LINEAR_MIPMAP_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
            default:                            return GL_LINEAR;
        }
    }

    GLenum wrapToGL(Texture::Wrap wrap) {
        switch (wrap) {
            case Texture::Wrap::REPEAT:          return GL_REPEAT;
            case Texture::Wrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case Texture::Wrap::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
            case Texture::Wrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            default:                    return GL_REPEAT;
        }
    }

    GLenum typeToGL(Texture::Type type) {
        switch (type) {
            case Texture::Type::TEXTURE_2D:      return GL_TEXTURE_2D;
            case Texture::Type::TEXTURE_CUBE_MAP: return GL_TEXTURE_CUBE_MAP;
            default:                    return GL_TEXTURE_2D;
        }
    }
    
    Texture::Texture() 
        : texture_id(0)
        , texture_type(Type::TEXTURE_2D)
        , internal_format(Format::RGBA)
        , width(0)
        , height(0) {
    }

    Texture::~Texture() {
        if (texture_id != 0) {
            std::cerr << "Warning: Texture destroyed without explicit destroy() call. Make sure you know what you're doing and destroy the GL resources manually." << std::endl;
        }
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

        // Set pixel unpack alignment to 1 to avoid row padding issues
        GLint prev_unpack_alignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_unpack_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLenum gl_internal_format = formatToGL(format);
        GLenum gl_format = (format == Format::RGBA) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, 
                    gl_format, GL_UNSIGNED_BYTE, data);

        // Restore previous alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, prev_unpack_alignment);

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

    int Texture::getChannelCount() const {
        switch (internal_format) {
            case Format::RGBA:         return 4;
            case Format::RGB:          return 3;
            case Format::DEPTH:        return 1;
            case Format::DEPTH_STENCIL: return 2;
            default:                   return 4;
        }
    }

    bool Texture::readPixels(unsigned char* data) const {
        return readPixels(data, internal_format);
    }

    bool Texture::readPixels(unsigned char* data, Format format) const {
        if (texture_id == 0) {
            std::cerr << "Error: Attempting to read pixels from invalid texture" << std::endl;
            return false;
        }

        if (data == nullptr) {
            std::cerr << "Error: Null data pointer provided to readPixels" << std::endl;
            return false;
        }

        GLenum gl_target = typeToGL(texture_type);
        GLenum gl_format = formatToGL(format);

        // Set pixel pack alignment to 1 to avoid row padding issues
        GLint prev_pack_alignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &prev_pack_alignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glBindTexture(gl_target, texture_id);
        glGetTexImage(gl_target, 0, gl_format, GL_UNSIGNED_BYTE, data);
        glBindTexture(gl_target, 0);

        // Restore previous alignment
        glPixelStorei(GL_PACK_ALIGNMENT, prev_pack_alignment);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error: OpenGL error during texture read: " << error << std::endl;
            return false;
        }

        return true;
    }

    bool Texture::saveToFile(const std::string& file_path, bool flip_vertically, 
                            int quality) const {
        if (texture_id == 0) {
            std::cerr << "Error: Attempting to save invalid texture" << std::endl;
            return false;
        }

        if (width <= 0 || height <= 0) {
            std::cerr << "Error: Invalid texture dimensions for save" << std::endl;
            return false;
        }

        // Determine channels based on internal format
        int channels = getChannelCount();
        GLenum gl_format = formatToGL(internal_format);

        // Allocate buffer for pixel data
        std::vector<unsigned char> data(width * height * channels);

        // Set pixel pack alignment to 1 to avoid row padding issues
        GLint prev_pack_alignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &prev_pack_alignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        // Read texture data from GPU
        GLenum gl_target = typeToGL(texture_type);
        glBindTexture(gl_target, texture_id);
        glGetTexImage(gl_target, 0, gl_format, GL_UNSIGNED_BYTE, data.data());
        glBindTexture(gl_target, 0);

        // Restore previous alignment
        glPixelStorei(GL_PACK_ALIGNMENT, prev_pack_alignment);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error: OpenGL error during texture read for save: " << error << std::endl;
            return false;
        }

        // Set vertical flip for stb_image_write
        stbi_flip_vertically_on_write(flip_vertically ? 1 : 0);

        // Determine format from file extension
        std::string extension = file_path.substr(file_path.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        int result = 0;
        if (extension == "png") {
            result = stbi_write_png(file_path.c_str(), width, height, channels, 
                                   data.data(), width * channels);
        } else if (extension == "bmp") {
            result = stbi_write_bmp(file_path.c_str(), width, height, channels, data.data());
        } else if (extension == "tga") {
            result = stbi_write_tga(file_path.c_str(), width, height, channels, data.data());
        } else if (extension == "jpg" || extension == "jpeg") {
            result = stbi_write_jpg(file_path.c_str(), width, height, channels, 
                                   data.data(), quality);
        } else {
            std::cerr << "Error: Unsupported image format: " << extension << std::endl;
            return false;
        }

        if (result == 0) {
            std::cerr << "Error: Failed to write image file: " << file_path << std::endl;
            return false;
        }

        return true;
    }

    void Texture::destroy()
    {
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
            texture_id = 0;
            width = 0;
            height = 0;
        }
    }

}