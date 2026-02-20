#include "rendering_system.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <iostream>

namespace cridgeon
{

    bool RenderingSystem::takeContext(bool noHang)
    {
        if (this->initialized_ && this->window_) {
            if (noHang) {
                if (context_mutex_.try_lock()) {
                    glfwMakeContextCurrent((GLFWwindow*)window_);
                    return true;
                } else {
                    return false;
                }
            } else {
                context_mutex_.lock();
                glfwMakeContextCurrent((GLFWwindow*)window_);
                return true;
            }
        }
        return false;   
    }

    bool RenderingSystem::releaseContext()
    {
        if (this->initialized_ && this->window_) {
            glfwMakeContextCurrent(nullptr);
            context_mutex_.unlock();
            return true;
        }
        return false;
    }

    RenderingSystem::RenderingSystem()
        : window_width_(0), window_height_(0), window_title_(""),
          window_(nullptr), clear_color_{0.05f, 0.05f, 0.08f, 1.0f}, glsl_version_("#version 130"),
          initialized_(false) {
    }
    
    RenderingSystem& RenderingSystem::getInstance() {
        static RenderingSystem instance;
        return instance;
    }
    
    RenderingSystem::~RenderingSystem() {
        shutdown();
    }
    
    bool RenderingSystem::initialize(int window_width, int window_height, const std::string& window_title) {
        if (initialized_) {
            return true;
        }
        
        window_width_ = window_width;
        window_height_ = window_height;
        window_title_ = window_title;
        
        if (!setupGLFW()) {
            std::cerr << "Failed to setup GLFW" << std::endl;
            return false;
        }
        
        initialized_ = true;
        return true;
    }
    
    void RenderingSystem::glfwErrorCallback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }
    
    bool RenderingSystem::setupGLFW() {
        // Setup GLFW
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
    
        // GL 3.0 + GLSL 130
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
        // Create window with graphics context
        window_ = glfwCreateWindow(window_width_, window_height_, window_title_.c_str(), nullptr, nullptr);
        if (window_ == nullptr) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent((GLFWwindow*)window_);
        glfwSwapInterval(1); // Enable vsync
    
        // Initialize GLAD to load OpenGL functions
        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return false;
        }
        
        // Enable blending for alpha transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        return true;
    }
    
    bool RenderingSystem::shouldContinue() {
        if (!initialized_ || !window_) {
            return false;
        }
        
        return !glfwWindowShouldClose((GLFWwindow*)window_);
    }
    
    void RenderingSystem::beginFrame() {
        if (!initialized_) return;
        takeContext();
        
        // Poll and handle events
        glfwPollEvents();
    
        // Check for window resize
        int display_w, display_h;
        glfwGetFramebufferSize((GLFWwindow*)window_, &display_w, &display_h);
        if (display_w != window_width_ || display_h != window_height_) {
            window_width_ = display_w;
            window_height_ = display_h;
        }
    
        // Clear the framebuffer and render ImGui to it
        glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void RenderingSystem::endFrame() {
        if (!initialized_) return;
        glfwSwapBuffers((GLFWwindow*)window_);
        releaseContext();
    }
    
    void RenderingSystem::setClearColor(const float col[4]) {
        for (int i = 0; i < 4; ++i) {
            clear_color_[i] = col[i];
        }
    }
    
    void RenderingSystem::shutdown() {
        if (!initialized_) return;
    
        if (window_) {
            glfwDestroyWindow((GLFWwindow*)window_);
            window_ = nullptr;
        }
        glfwTerminate();
        
        initialized_ = false;
    }
} // namespace cridgeon