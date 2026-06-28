#pragma once

#include "imgui.h"

#include <GLFW/glfw3.h>

#include "Layer.h"
#include "../Utils/Timer.h"

#include <string>
#include <vector>
#include <memory>

namespace Tiles
{

	struct ApplicationSpecification
	{
		std::string Name = "Tiles App";
		std::string Icon = "";

		uint32_t Width = 1600;
		uint32_t Height = 900;
		int32_t PositionX = 100;
		int32_t PositionY = 100;

		bool Use2DRenderer = false;
		bool Use3DRenderer = false;

		bool Fullscreen = false;
		bool Maximized = false;
		bool Theme = true;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		virtual ~Application();

		void Run();

		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		// From Walnut
		template<typename T>
		void PushLayer()
		{
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void SetWindowFullscreen();

		void ApplyTilesTheme();

		void Shutdown() { m_Running = false; };

		static Application& GetInstance();
		GLFWwindow* GetWindowHandle() const { return m_Window; };

	private:
		GLFWwindow* m_Window = nullptr;

		bool m_Running = true;

		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		ApplicationSpecification m_Specifications;

		float m_TimeStep = 0.0f;
		Timer m_FrameTimer;

		bool m_IsDragging = false;
		ImVec2 m_DragOffset = { 0.0f, 0.0f };
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);
}
