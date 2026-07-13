#pragma once

#include <string>

namespace Tiles
{
	class Event;

	// A unit of application behavior driven by the Application run loop.
	// Subclasses hook into the attach/detach lifecycle and the per-frame
	// update, UI render, and event passes.
	class Layer
	{
	public:
		// Names the layer for identification and debugging.
		Layer(const std::string& name = "layer");

		// Virtual so derived layers destruct correctly through a Layer pointer.
		virtual ~Layer() = default;

		// A polymorphic base held by unique_ptr; copy/move are deleted to prevent slicing.
		Layer(const Layer&) = delete;
		Layer& operator=(const Layer&) = delete;
		Layer(Layer&&) = delete;
		Layer& operator=(Layer&&) = delete;

		// Called once when the layer is attached to the stack.
		virtual void OnAttach() = 0;

		// Called once when the layer is detached from the stack.
		virtual void OnDetach() = 0;

		// Advances the layer by one frame.
		// @param timestep Seconds elapsed since the previous frame.
		virtual void OnUpdate(float timestep) = 0;

		// Renders the layer's ImGui UI for the frame.
		virtual void OnUIRender() = 0;

		// Handles a window/input event. Mark it handled to stop propagation to
		// layers beneath. Default: ignore.
		virtual void OnEvent(Event& event) {}

		// The layer's name.
		[[nodiscard]] const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
	};
}
