#pragma once

#include <string>

namespace Tiles
{
	class Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
		virtual void OnEvent() = 0;

		[[nodiscard]] const std::string& GetName() const { return m_name; }
	private:
		std::string m_name;
	};
}