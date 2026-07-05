#pragma once

#include <string>

namespace Tiles
{
	class Event;

	/// A unit of application behavior driven by the Application run loop.
	/// Subclasses hook into the attach/detach lifecycle and the per-frame
	/// update, UI render, and event passes.
	class Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		/// @param timestep Seconds elapsed since the previous frame.
		virtual void OnUpdate(float timestep) = 0;
		virtual void OnUIRender() = 0;
		/// Handles a window/input event. Mark it handled to stop propagation to
		/// layers beneath. Default: ignore.
		virtual void OnEvent(Event& event) {}

		[[nodiscard]] const std::string& GetName() const { return m_name; }
	private:
		std::string m_name;
	};
}