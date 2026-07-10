#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Application.h"

#include <vector>

#include <spdlog/spdlog.h>

#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Log.h"
#include "Assert.h"
#include "ApplicationSettingsSerializer.h"

#include "../Graphics/Renderer2D.h"

namespace Tiles
{
	// Static singleton
	static Tiles::Application* s_Instance = nullptr;

	// Window state is persisted here, in the working directory alongside imgui.ini.
	// TODO: This filename should be in the ApplicationSettings file
	static constexpr const char* SETTINGS_FILE = "settings.json";

	// Static accessor
	Application& Application::GetInstance() { return *s_Instance; }

	// Builds the window, loads GL, initializes the ImGui backends, and restores
	// persisted window geometry over the code-provided defaults.
	Application::Application(const ApplicationSettings& settings)
	{
		s_Instance = this;
		m_Settings = settings;

		TILES_LOGGER_INIT();
		TILES_ENGINE_INFO("Starting Tiles Application: {}", m_Settings.Window.Title);

		// Restore persisted window geometry over the code-provided defaults; a
		// first run (no file yet) leaves the defaults in place.
		ApplicationSettingsSerializer::Load(SETTINGS_FILE, m_Settings);

		// The Window owns GLFW init, the OS window, its icon, and the OpenGL
		// context the app renders into. It is created hidden — applying the
		// persisted geometry and maximize/fullscreen state — and shown after setup.
		m_Window = std::make_unique<Window>(m_Settings.Window);
		if (!m_Window->GetNativeWindow())
		{
			TILES_ENGINE_ERROR("Failed to create the application window.");
			return;
		}

		int status = gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress));
		TILES_ASSERT(status, "[OpenGL Context] Failed to initialize GLAD.");

		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		TILES_ASSERT(version, "[OpenGL Context] Failed to retrieve OpenGL version.");
		TILES_ENGINE_INFO("OpenGL Version: {}", version);

		if (m_Settings.Use2DRenderer)
			Renderer2D::Init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
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
		const char* glslVersion = "#version 130";
		ImGui_ImplOpenGL3_Init(glslVersion);

		// Route OS window/input events through the application to the layers.
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

		// Created hidden; reveal it now that GL and ImGui are initialized.
		m_Window->Show();
	}

	// Persists window geometry, then tears down the ImGui/GL backends and window.
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

	// Runs client setup (OnCreate) once before the run loop.
	void Application::Create()
	{
		OnCreate();
	}

	// Detaches all layers, then runs client teardown (OnDestroy).
	void Application::Destroy()
	{
		for (auto& layer : m_LayerStack)
			layer->OnDetach();

		m_LayerStack.clear();

		OnDestroy();
	}

	// Application run loop
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

			// Advance last frame's press/release edges to steady state before the
			// window pumps this frame's events into the input state (in OnEvent).
			m_InputState.NewFrame();
			m_Window->Update();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGuiWindowFlags dockspaceFlags =
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

			ImGui::Begin("DockSpace", nullptr, dockspaceFlags);

			// Create the dockspace
			ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

			ImGui::End();
			ImGui::PopStyleVar(3);

			// Let each layer emit its UI into the dockspace.
			for (auto& layer : m_LayerStack)
				layer->OnUIRender();

			// Render ImGui
			ImGui::Render();

			ImDrawData* mainDrawData = ImGui::GetDrawData();
			const bool mainIsMinimized = (mainDrawData->DisplaySize.x <= 0.0f || mainDrawData->DisplaySize.y <= 0.0f);

			if (!mainIsMinimized)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}

			// Handle ImGui viewport if enabled
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backupCurrentContext);
			}

			if (!mainIsMinimized)
				m_Window->SwapBuffers();
		}
	}

	// Dispatches an event from the window to the app and its layers.
	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&)
		{
			m_Running = false;
			return true;
		});

		// Feed the input state before layers run, and without consuming the event,
		// so held-state stays correct even if a layer marks the event handled.
		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) { m_InputState.OnKeyPressed(e.GetKey(), e.IsRepeat()); return false; });
		dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e) { m_InputState.OnKeyReleased(e.GetKey()); return false; });
		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) { m_InputState.OnMouseButtonPressed(e.GetButton()); return false; });
		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) { m_InputState.OnMouseButtonReleased(e.GetButton()); return false; });
		dispatcher.Dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent&) { m_InputState.Reset(); return false; });

		// Deliver top-down; a layer that marks the event handled stops it from
		// reaching layers beneath.
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event.IsHandled())
				break;
			(*it)->OnEvent(event);
		}
	}

	// Applies the current Fullscreen setting to the window.
	void Application::SetWindowFullscreen()
	{
		if (m_Window)
			m_Window->SetFullscreen(m_Settings.Window.Fullscreen);
	}

	// Saves the application settings
	// TODO: this should be moved to ApplicationSettingsSerializer
	void Application::SaveSettings()
	{
		if (m_Window)
		{
			m_Settings.Window.Maximized = m_Window->IsMaximized();

			// Persist the floating geometry, not the maximized/fullscreen size, so
			// the window restores to where it last was in windowed mode.
			if (!m_Settings.Window.Maximized && !m_Settings.Window.Fullscreen)
			{
				m_Window->GetPosition(m_Settings.Window.PositionX, m_Settings.Window.PositionY);
				m_Settings.Window.Width = m_Window->GetWidth();
				m_Settings.Window.Height = m_Window->GetHeight();
			}
		}

		ApplicationSettingsSerializer::Save(SETTINGS_FILE, m_Settings);
	}

}
