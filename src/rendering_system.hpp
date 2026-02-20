#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>

#include <mutex>

namespace cridgeon
{
    
    class RenderingSystem {
    public:
        // Singleton access
        static RenderingSystem& getInstance();
        
        // Delete copy constructor and assignment operator
        RenderingSystem(const RenderingSystem&) = delete;
        RenderingSystem& operator=(const RenderingSystem&) = delete;
        
        ~RenderingSystem();
    
        // Initialize the rendering system with window parameters
        bool initialize(int window_width, int window_height, 
                       const std::string& window_title = "Kalman Test - ImGui Demo");
        
        // Main rendering loop - returns true while window should remain open
        bool shouldContinue();
        
        // Begin frame - call before rendering
        void beginFrame();
        
        // End frame and swap buffers
        void endFrame();
        
        // Set background color
        void setClearColor(const float col[4]);
        
        // Get window dimensions
        int getWindowWidth() const { return window_width_; }
        int getWindowHeight() const { return window_height_; }
        
        // Cleanup
        void shutdown();
    
        const char* getGLSLVersion() const { return glsl_version_; }

        bool takeContext(bool noHang = false);
        bool releaseContext();
    
    private:
        // Private constructor for singleton
        RenderingSystem();
        
        // GLFW callback
        static void glfwErrorCallback(int error, const char* description);
        
        // Setup functions
        bool setupGLFW();
        
    private:
        int window_width_;
        int window_height_;
        std::string window_title_;
        
        void* window_;
        float clear_color_[4];
        const char* glsl_version_;
        
        bool initialized_;

        std::mutex context_mutex_;
    };
} // namespace cridgeon
