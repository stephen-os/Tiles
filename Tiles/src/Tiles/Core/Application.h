#pragma once

#include "imgui.h"

#include "Window.h"

#include "Layer.h"
#include "ApplicationSettings.h"
#include "../Utils/Timer.h"

#include <string>
#include <vector>
#include <memory>

namespace Tiles
{
	class Application
	{
	public:
		/// Application constructor
		Application(const ApplicationSettings& settings = ApplicationSettings());
		
		/// Application destructor
		virtual ~Application();

		/// Runs client setup (OnCreate); call once before Run.
		void Create();

		/// Detaches all layers and runs client teardown (OnDestroy).
		void Destroy();
		
		/// Drives the main loop: updates layers, renders the ImGui dockspace and
		/// UI, and presents each frame until the window closes or Shutdown is called.
		void Run();

		/// Toggles between borderless-fullscreen on the primary monitor and the
		/// last windowed position/size, per the current Fullscreen spec flag.
		void SetWindowFullscreen();

		/// Requests the run loop to exit at the end of the current frame.
		void Shutdown() { m_Running = false; };

		/// Gets the instance of this application
		static Application& GetInstance();

		/// Gets the native window handle of this application
		GLFWwindow* GetWindowHandle() const { return m_Window ? m_Window->GetNativeWindow() : nullptr; };

	protected:
		/// Client side create
		virtual void OnCreate() = 0;

		/// Client side destroy
		virtual void OnDestroy() = 0;

		/// Constructs a layer of type T in place, takes ownership, and attaches it.
		/// @tparam T Layer subclass to instantiate.
		/// @param args Arguments forwarded to T's constructor.
		template<typename T, typename... Args>
		void PushLayer(Args&&... args)
		{
			static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
			m_LayerStack.emplace_back(std::make_unique<T>(std::forward<Args>(args)...))->OnAttach();
		}

	private:
		/// Captures the current window geometry (size/position/maximized/
		/// fullscreen) and writes it to the settings file for the next run.
		void SaveSettings();

	private:
		std::unique_ptr<Window> m_Window;					/// Owns the OS window + GL context
		bool m_Running = true;
		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		ApplicationSettings m_Settings;
		float m_TimeStep = 0.0f;
		Timer m_FrameTimer;
	};

	/// Factory implemented by the client to build the concrete Application.
	/// Called by the entry point; the returned instance is owned by Main.
	Application* CreateApplication(int argc, char** argv);
}
