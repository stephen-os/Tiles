#pragma once

#include "Event.h"

#include <string>
#include <functional>

struct GLFWwindow;

namespace Tiles
{
	struct WindowSettings
	{
		std::string Title = "Tiles Application";
		std::string IconPath;
		uint32_t Width = 1600;
		uint32_t Height = 900;
		int32_t PositionX = 100;
		int32_t PositionY = 100;
		bool Fullscreen = false;
		bool Maximized = false;
		bool Centered = false;
		bool Resizable = true;
		bool Decorated = true;
		bool VSync = true;
	};

	// Owns a GLFW window and its OpenGL context, translates GLFW callbacks into
	// Tiles Events, and exposes the window's state and geometry to the application.
	class Window
	{
	public:
		// Creates the (hidden) window and its GL context from the given settings.
		Window(const WindowSettings& settings);

		// Destroys the GLFW window; GLFW itself is torn down by TerminateGLFW.
		~Window();

		// GLFW stores a back-pointer to this Window (glfwSetWindowUserPointer), so
		// the object is pinned in memory: it must not be copied or moved.
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		// Pumps the OS event queue; call once per frame.
		void Update();

		// Presents the frame by swapping the front and back buffers.
		void SwapBuffers();

		// True once the OS has requested the window be closed.
		[[nodiscard]] bool ShouldClose() const;

		// Registers the sink that receives translated window/input Events.
		void SetEventCallback(const std::function<void(Event&)>& callback) { m_EventCallback = callback; }

		// Enables or disables vertical sync.
		void SetVSync(bool enabled);

		// Toggles borderless-fullscreen on the primary monitor, restoring the
		// stored windowed geometry when leaving fullscreen.
		void SetFullscreen(bool fullscreen);

		// Moves the window's top-left to (x, y) in screen coordinates.
		void SetPosition(int32_t x, int32_t y);

		// Maximizes the window.
		void Maximize();

		// Minimizes (iconifies) the window.
		void Minimize();

		// Restores the window from a maximized/minimized state.
		void Restore();

		// Makes the (initially hidden) window visible.
		void Show();

		// Centers the window on the primary monitor.
		void CenterOnMonitor();

		// Sets the titlebar background color (Windows 11+ only; no-op elsewhere).
		void SetTitlebarColor(uint8_t r, uint8_t g, uint8_t b);

		// Sets the titlebar text color (Windows 11+ only; no-op elsewhere).
		void SetTitlebarTextColor(uint8_t r, uint8_t g, uint8_t b);

		// Current framebuffer width in pixels.
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }

		// Current framebuffer height in pixels.
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }

		// Writes the window's top-left screen position into x, y.
		void GetPosition(int32_t& x, int32_t& y) const;

		// Whether vertical sync is currently enabled.
		[[nodiscard]] bool IsVSync() const { return m_VSync; }

		// Whether the window is currently fullscreen.
		[[nodiscard]] bool IsFullscreen() const { return m_Fullscreen; }

		// Whether the window is currently maximized.
		[[nodiscard]] bool IsMaximized() const;

		// Whether the window is currently minimized (iconified).
		[[nodiscard]] bool IsMinimized() const;

		// The underlying GLFW window handle (null if creation failed).
		[[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }

		// Terminates GLFW; call once after the last Window has been destroyed.
		static void TerminateGLFW();

	private:
		// Installs the GLFW callbacks that translate input/window events to Events.
		void SetupCallbacks();

		// Loads and applies the window icon from an image file.
		void SetIcon(const std::string& iconPath);

	private:
		GLFWwindow* m_Window = nullptr;
		std::function<void(Event&)> m_EventCallback;

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
