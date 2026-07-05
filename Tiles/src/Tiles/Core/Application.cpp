#include <glad/gl.h>
#include <GLFW/glfw3.h>

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
    /// Static singleton
    static Tiles::Application* s_Instance = nullptr;
    
    /// Window state is persisted here, in the working directory alongside imgui.ini.
    /// TODO: This filename should be in the ApplicationSettings file
    static constexpr const char* SETTINGS_FILE = "settings.json";

    /// Static accessor
    Application& Application::GetInstance() { return *s_Instance; }

    /// Constructor
    Application::Application(const ApplicationSettings& settings)
    {
		s_Instance = this;
        m_Settings = settings;

        TILES_LOGGER_INIT();
        TILES_ENGINE_INFO("Starting Tiles Application: {}", m_Settings.Name);

        // Restore persisted window geometry over the code-provided defaults; a
        // first run (no file yet) leaves the defaults in place.
        ApplicationSettingsSerializer::Load(SETTINGS_FILE, m_Settings);

        // The Window owns GLFW init, the OS window, its icon, and the OpenGL
        // context the app renders into. It is created hidden and shown after setup.
        WindowSpec spec;
        spec.Title = m_Settings.Name;
        spec.Width = m_Settings.Width;
        spec.Height = m_Settings.Height;
        spec.IconPath = m_Settings.Icon;
        spec.VSync = true;

        m_Window = std::make_unique<Window>(spec);
        if (!m_Window->GetNativeWindow())
        {
            TILES_ENGINE_ERROR("Failed to create the application window.");
            return;
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

        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Route OS window/input events through the application to the layers.
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        // Restore the windowed position before maximizing/fullscreening, so the
        // window returns here when the user later un-maximizes.
        m_Window->SetPosition(m_Settings.PositionX, m_Settings.PositionY);

        if (m_Settings.Maximized)
            m_Window->Maximize();

        if (m_Settings.Fullscreen)
            m_Window->SetFullscreen(true);

        // Created hidden; reveal it now that GL and ImGui are initialized.
        m_Window->Show();
    }

    /// Application destructor
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

            m_Window.reset();
        }

        Window::TerminateGLFW();

        TILES_LOGGER_SHUTDOWN();
    }

    /// Application entry point
    void Application::Create()
    {
        OnCreate();
    }
    
    /// Application exit point
    void Application::Destroy()
    {
        for (auto& layer : m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();

        OnDestroy();
    }

    /// Application run loop
    void Application::Run()
    {
        if (!m_Window || !m_Window->GetNativeWindow())
        {
            TILES_ENGINE_ERROR("Application::Run: Window was not created; aborting run loop.");
            return;
        }

        while (!m_Window->ShouldClose() && m_Running)
        {
            m_TimeStep = m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            m_Window->Update();

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
                m_Window->SwapBuffers();
        }
    }

    /// Dispatches an event from the window to the app and its layers.
    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&)
        {
            m_Running = false;
            return true;
        });

        // Deliver top-down; a layer that marks the event handled stops it from
        // reaching layers beneath.
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (event.IsHandled())
                break;
            (*it)->OnEvent(event);
        }
    }

    /// Applies the current Fullscreen setting to the window.
    void Application::SetWindowFullscreen()
    {
        if (m_Window)
            m_Window->SetFullscreen(m_Settings.Fullscreen);
    }

    /// Saves the application settings
    /// TODO: this should be moved to ApplicationSettingsSerializer
    /// TODO: this should be called on shutdown
    void Application::SaveSettings()
    {
        if (m_Window)
        {
            m_Settings.Maximized = m_Window->IsMaximized();

            // Persist the floating geometry, not the maximized/fullscreen size, so
            // the window restores to where it last was in windowed mode.
            if (!m_Settings.Maximized && !m_Settings.Fullscreen)
            {
                m_Window->GetPosition(m_Settings.PositionX, m_Settings.PositionY);
                m_Settings.Width = m_Window->GetWidth();
                m_Settings.Height = m_Window->GetHeight();
            }
        }

        ApplicationSettingsSerializer::Save(SETTINGS_FILE, m_Settings);
    }

}
