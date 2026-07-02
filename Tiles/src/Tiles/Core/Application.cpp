#include <glad/gl.h>

#include "Application.h"

#include <iostream>
#include <vector>

#include <spdlog/spdlog.h>

#include <stb/stb_image.h>

#include <fstream>
#include <filesystem>
#include <stdlib.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Log.h"
#include "Assert.h"

#include "../Graphics/Renderer2D.h"

namespace Tiles
{
    static Tiles::Application* s_Instance = nullptr;

    Application& Application::GetInstance() { return *s_Instance; }

    static void GLFWErrorCallback(int error, const char* description)
    {
        TILES_LOG_ERROR("[GLFW ERROR] {}: {}", error, description);
    }

    Application::Application(const ApplicationSpecification& applicationSpecification)
    {
		s_Instance = this;
        m_Specifications = applicationSpecification;

        Log::Init(m_Specifications.Name);
        TILES_LOG_INFO("Starting Tiles Application: {}", m_Specifications.Name);

        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit())
        {
            TILES_LOG_ERROR("GLFW failed to initialize.");
            return;
        }

        // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Specifications.Width, m_Specifications.Height, m_Specifications.Name.c_str(), NULL, NULL);
        if (!m_Window)
        {
            TILES_LOG_ERROR("Failed to create GLFW window.");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);

        // Custom GLFW functions not available in standard GLFW
        // glfwSetWindowTitleBarColor(m_Window, 45, 45, 45);
        // glfwSetWindowTitleBarTextColor(m_Window, 255, 153, 51);

        if (!m_Specifications.Icon.empty())
        {
            GLFWimage icon;
            icon.pixels = stbi_load(m_Specifications.Icon.c_str(), &icon.width, &icon.height, 0, 4);
            if (icon.pixels)
            {
                glfwSetWindowIcon(m_Window, 1, &icon);
                stbi_image_free(icon.pixels);
            }
            else
            {
                TILES_LOG_WARN("Failed to load window icon: {}", m_Specifications.Icon);
            }
        }

        int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
        TILES_ASSERT(status, "[OpenGL Context] Failed to initialize GLAD.");

        const char* version = (const char*)glGetString(GL_VERSION);
        TILES_ASSERT(version, "[OpenGL Context] Failed to retrieve OpenGL version.");
        TILES_LOG_INFO("OpenGL Version: {}", version);

        if (m_Specifications.Use2DRenderer)
            Renderer2D::Init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

		// Maximize Window
        if (m_Specifications.Maximized)
        {
            glfwMaximizeWindow(m_Window);
        }

        // Fullscreen
        if (m_Specifications.Fullscreen)
        {
            SetWindowFullscreen();
        }

        // Apply Theme
        if (m_Specifications.Theme)
            ApplyTilesTheme();
    }


    Application::~Application()
    {
		s_Instance = nullptr;

        // A null window means construction bailed out before the graphics and
        // ImGui backends were initialized, so their teardown must be skipped.
        if (m_Window)
        {
            if (m_Specifications.Use2DRenderer)
                Renderer2D::Shutdown();

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    void Application::Create()
    {
        OnCreate();
    }
    
    void Application::Destroy()
    {
        for (auto& layer : m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();

        OnDestroy();
    }

    void Application::Run()
    {
        if (!m_Window)
        {
            TILES_LOG_ERROR("Application::Run: Window was not created; aborting run loop.");
            return;
        }

        while (!glfwWindowShouldClose(m_Window) && m_Running)
        {
            m_TimeStep = m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGuiWindowFlags dockspace_flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoBackground;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("DockSpace", nullptr, dockspace_flags);

            // Create the dockspace
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
			static bool flag = true;
            if (flag)
            {
			    TILES_LOG_INFO("Application: Dockspace ID: {}", dockspace_id);
                flag = false;
            }
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            ImGui::End();
            ImGui::PopStyleVar(3);

            // Layer from TilesApp
            for (auto& layer : m_LayerStack)
                layer->OnUIRender();

            // Render ImGui
            ImGui::Render();

            ImDrawData* main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

            if (!main_is_minimized)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            // Handle ImGui viewport if enabled
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            if (!main_is_minimized)
                glfwSwapBuffers(m_Window);
        }
    }

    void Application::SetWindowFullscreen()
    {
        if (m_Specifications.Fullscreen)
        {
            glfwGetWindowPos(m_Window, &m_Specifications.PositionX, &m_Specifications.PositionY);
            glfwGetWindowSize(m_Window, (int*)&m_Specifications.Width, (int*)&m_Specifications.Height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            TILES_ASSERT(monitor, "Failed to get primary monitor.");
            if (!monitor) return;

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            TILES_ASSERT(mode, "Failed to get monitor video mode.");

            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, m_Specifications.PositionX, m_Specifications.PositionY,
                m_Specifications.Width, m_Specifications.Height, 0);
        }
    }

    void Application::ApplyTilesTheme()
    {
        ImGui::StyleColorsDark();                                               // First apply dark theme

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;                                            // Rounded corners for windows
        style.FrameRounding = 5.0f;                                             // Rounded corners for frames
        style.FramePadding = ImVec2(5.0f, 5.0f);                                // Padding within a frame
        style.ItemSpacing = ImVec2(8.0f, 6.0f);                                 // Spacing between items

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);             // Dark background
        colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);              // Frame background
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);       // Hovered frame background
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        // Active frame background
        colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);               // Button
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);        // Hovered button
        colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);         // Active button
        colors[ImGuiCol_Header] = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);               // Header (Orange)
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);        // Hovered header
        colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);         // Active header
        colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);            // Checkmark (Orange)
        colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);           // Slider grab
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);     // Active slider grab
        colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.4f, 0.1f, 1.0f);                  // Tab (Orange)
        colors[ImGuiCol_TabHovered] = ImVec4(0.9f, 0.5f, 0.2f, 1.0f);           // Hovered tab
        colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);            // Active tab
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);         // Unfocused tab
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);   // Unfocused active tab

        // Maintain compatibility with multiple viewports
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;                                        // Disable rounding for additional platform windows
            colors[ImGuiCol_WindowBg].w = 1.0f;                                 // Fully opaque background
        }
    }
}
