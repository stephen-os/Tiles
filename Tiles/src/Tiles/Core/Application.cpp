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
#include "ApplicationSettingsSerializer.h"

#include "../Graphics/Renderer2D.h"

namespace Tiles
{
    static Tiles::Application* s_Instance = nullptr;

    // Window state is persisted here, in the working directory alongside imgui.ini.
    static constexpr const char* SETTINGS_FILE = "settings.json";

    Application& Application::GetInstance() { return *s_Instance; }

    static void GLFWErrorCallback(int error, const char* description)
    {
        TILES_ENGINE_ERROR("[GLFW ERROR] {}: {}", error, description);
    }

    Application::Application(const ApplicationSettings& settings)
    {
		s_Instance = this;
        m_Settings = settings;

        TILES_LOGGER_INIT();
        TILES_ENGINE_INFO("Starting Tiles Application: {}", m_Settings.Name);

        // Restore persisted window geometry over the code-provided defaults; a
        // first run (no file yet) leaves the defaults in place.
        ApplicationSettingsSerializer::Load(SETTINGS_FILE, m_Settings);

        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit())
        {
            TILES_ENGINE_ERROR("GLFW failed to initialize.");
            return;
        }

        // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Settings.Width, m_Settings.Height, m_Settings.Name.c_str(), NULL, NULL);
        if (!m_Window)
        {
            TILES_ENGINE_ERROR("Failed to create GLFW window.");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);

        // Custom GLFW functions not available in standard GLFW
        // glfwSetWindowTitleBarColor(m_Window, 45, 45, 45);
        // glfwSetWindowTitleBarTextColor(m_Window, 255, 153, 51);

        // A missing or undecodable icon is non-fatal: log it and fall back to the
        // window system's default icon (nothing set).
        if (!m_Settings.Icon.empty())
        {
            if (!std::filesystem::exists(m_Settings.Icon))
            {
                TILES_ENGINE_WARN("Application: Window icon '{}' not found; using the default.", m_Settings.Icon);
            }
            else
            {
                GLFWimage icon;
                icon.pixels = stbi_load(m_Settings.Icon.c_str(), &icon.width, &icon.height, 0, 4);
                if (icon.pixels)
                {
                    glfwSetWindowIcon(m_Window, 1, &icon);
                    stbi_image_free(icon.pixels);
                }
                else
                {
                    TILES_ENGINE_WARN("Application: Failed to decode window icon '{}'; using the default.", m_Settings.Icon);
                }
            }
        }

        int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
        TILES_ASSERT(status, "[OpenGL Context] Failed to initialize GLAD.");

        const char* version = (const char*)glGetString(GL_VERSION);
        TILES_ASSERT(version, "[OpenGL Context] Failed to retrieve OpenGL version.");
        TILES_ENGINE_INFO("OpenGL Version: {}", version);

        if (m_Settings.Use2DRenderer)
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

        // Restore the windowed position before maximizing/fullscreening, so the
        // window returns here when the user later un-maximizes.
        glfwSetWindowPos(m_Window, m_Settings.PositionX, m_Settings.PositionY);

		// Maximize Window
        if (m_Settings.Maximized)
        {
            glfwMaximizeWindow(m_Window);
        }

        // Fullscreen
        if (m_Settings.Fullscreen)
        {
            SetWindowFullscreen();
        }
    }


    Application::~Application()
    {
		s_Instance = nullptr;

        // A null window means construction bailed out before the graphics and
        // ImGui backends were initialized, so their teardown must be skipped.
        if (m_Window)
        {
            SaveSettings();

            if (m_Settings.Use2DRenderer)
                Renderer2D::Shutdown();

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();

        TILES_LOGGER_SHUTDOWN();
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
            TILES_ENGINE_ERROR("Application::Run: Window was not created; aborting run loop.");
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
			    TILES_ENGINE_INFO("Application: Dockspace ID: {}", dockspace_id);
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
        if (m_Settings.Fullscreen)
        {
            glfwGetWindowPos(m_Window, &m_Settings.PositionX, &m_Settings.PositionY);
            glfwGetWindowSize(m_Window, (int*)&m_Settings.Width, (int*)&m_Settings.Height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            TILES_ASSERT(monitor, "Failed to get primary monitor.");
            if (!monitor) return;

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            TILES_ASSERT(mode, "Failed to get monitor video mode.");

            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, m_Settings.PositionX, m_Settings.PositionY,
                m_Settings.Width, m_Settings.Height, 0);
        }
    }

    void Application::SaveSettings()
    {
        if (m_Window)
        {
            m_Settings.Maximized = glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE;

            // Persist the floating geometry, not the maximized/fullscreen size, so
            // the window restores to where it last was in windowed mode.
            if (!m_Settings.Maximized && !m_Settings.Fullscreen)
            {
                glfwGetWindowPos(m_Window, &m_Settings.PositionX, &m_Settings.PositionY);

                int width = 0, height = 0;
                glfwGetWindowSize(m_Window, &width, &height);
                m_Settings.Width = static_cast<uint32_t>(width);
                m_Settings.Height = static_cast<uint32_t>(height);
            }
        }

        ApplicationSettingsSerializer::Save(SETTINGS_FILE, m_Settings);
    }

}
