#pragma once

#include "Event.h"

#include <string>
#include <functional>

struct GLFWwindow;

namespace Tiles
{
	struct WindowSpec
	{
		std::string Title = "Tiles Application";
		std::string IconPath;
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool Fullscreen = false;
		bool Maximized = false;
		bool Centered = false;
		bool Resizable = true;
		bool Decorated = true;
		bool VSync = true;
	};

	/// GLFW window owning an OpenGL context. Forwards OS input/window events to a
	/// callback (unset for now -- event wiring lands in a later change).
	class Window
	{
	public:
		using EventCallback = std::function<void(Event&)>;

		Window(const WindowSpec& spec);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void Update();               // polls OS events
		void SwapBuffers();          // presents the GL back buffer
		[[nodiscard]] bool ShouldClose() const;

		void SetEventCallback(const EventCallback& callback) { m_EventCallback = callback; }

		// Runtime operations
		void SetVSync(bool enabled);
		void SetFullscreen(bool fullscreen);
		void SetPosition(int32_t x, int32_t y);
		void Maximize();
		void Minimize();
		void Restore();
		void Show();
		void CenterOnMonitor();

		// Titlebar theming (Windows only)
		void SetTitlebarColor(uint8_t r, uint8_t g, uint8_t b);
		void SetTitlebarTextColor(uint8_t r, uint8_t g, uint8_t b);

		// State queries
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }
		void GetPosition(int32_t& x, int32_t& y) const;
		[[nodiscard]] bool IsVSync() const { return m_VSync; }
		[[nodiscard]] bool IsFullscreen() const { return m_Fullscreen; }
		[[nodiscard]] bool IsMaximized() const;
		[[nodiscard]] bool IsMinimized() const;

		[[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }

		static void TerminateGLFW();

	private:
		void SetupCallbacks();
		void SetIcon(const std::string& iconPath);

		GLFWwindow* m_Window = nullptr;
		EventCallback m_EventCallback;

		// Cached state
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		int32_t m_WindowedX = 100;
		int32_t m_WindowedY = 100;
		uint32_t m_WindowedWidth = 0;
		uint32_t m_WindowedHeight = 0;
		bool m_VSync = true;
		bool m_Fullscreen = false;
	};
}
