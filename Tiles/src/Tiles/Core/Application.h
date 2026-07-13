#pragma once

#include "Window.h"
#include "Layer.h"

#include "Input/InputState.h"
#include "../Utils/Timer.h"

#include <string>
#include <vector>
#include <memory>

namespace Tiles
{
	// Startup configuration for the Application
	struct ApplicationSettings
	{
		WindowSettings Window;

		// Working-directory-relative path of the persisted window-state file.
		std::string SettingsFile = "settings.json";
	};

	class Application
	{
	public:
		// Builds the window, loads GL, and initializes the ImGui backends.
		Application(const ApplicationSettings& settings = ApplicationSettings());
		
		// Tears down the ImGui/GL backends and window; persists window geometry.
		virtual ~Application();

		// Runs client setup (OnCreate); call once before Run.
		void Create();

		// Detaches all layers and runs client teardown (OnDestroy).
		void Destroy();
		
		// Drives the main loop: updates layers, renders the ImGui dockspace and
		// UI, and presents each frame until the window closes or Shutdown is called.
		void Run();

		// Toggles between borderless-fullscreen on the primary monitor and the
		// last windowed position/size, per the current Fullscreen spec flag.
		void SetWindowFullscreen();

		// Requests the run loop to exit at the end of the current frame.
		void Shutdown() { m_Running = false; }

		// Gets the instance of this application
		[[nodiscard]] static Application& GetInstance();

		// Gets the native window handle of this application
		[[nodiscard]] GLFWwindow* GetWindowHandle() const { return m_Window ? m_Window->GetNativeWindow() : nullptr; }

		// The owned window. Valid for the lifetime of the running application.
		[[nodiscard]] Window& GetWindow() const { return *m_Window; }

		// Frame-coherent keyboard/mouse state, fed by the event stream each frame.
		[[nodiscard]] Input::InputState& GetInputState() { return m_InputState; }

	protected:
		// Client side create
		virtual void OnCreate() = 0;

		// Client side destroy
		virtual void OnDestroy() = 0;

		// Constructs a layer of type T in place, takes ownership, and attaches it.
		template<typename T, typename... Args>
		void PushLayer(Args&&... args)
		{
			static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
			m_LayerStack.emplace_back(std::make_unique<T>(std::forward<Args>(args)...))->OnAttach();
		}

	private:
		// Receives window/input events from the Window and dispatches them to
		// layers (top-down, stopping once an event is marked handled).
		void OnEvent(Event& event);

		// Captures the current window geometry (size/position/maximized/
		// fullscreen) and writes it to the settings file for the next run.
		void SaveSettings();

		// Loads the settings file and applies the window geometry (size/position/
		// maximized/fullscreen) to the window.
		void LoadSettings();

	private:
		std::unique_ptr<Window> m_Window;
		Input::InputState m_InputState;
		bool m_Running = true;
		std::vector<std::unique_ptr<Layer>> m_LayerStack;
		ApplicationSettings m_Settings;
		float m_TimeStep = 0.0f;
		Timer m_FrameTimer;
	};

	// Factory implemented by the client to build the concrete Application.
	// Called by the entry point; the returned instance is owned by Main.
	Application* CreateApplication(int argc, char** argv);
}
