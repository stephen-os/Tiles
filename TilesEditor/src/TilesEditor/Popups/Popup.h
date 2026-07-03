#pragma once
#include <memory>

#include "Core/Context.h"

namespace Tiles::Editor
{
	/// Base class for dialog-style popups owned by a panel. Tracks a visibility
	/// flag toggled via Show/Hide/Toggle; Render() and Update() dispatch to the
	/// derived OnRender/OnUpdate only while visible, so callers may call them
	/// unconditionally each frame.
	class Popup
	{
	public:
		Popup(std::shared_ptr<Context> context);
		virtual ~Popup() = default;
		
		void Render();
		void Update();
		
		void Show() { m_IsVisible = true; }
		void Hide() { m_IsVisible = false; }
		void Toggle() { m_IsVisible ? Hide() : Show(); }
		bool IsVisible() const { return m_IsVisible; }

	protected: 
		virtual void OnRender() = 0;
		virtual void OnUpdate() = 0; 

	protected:
		bool m_IsVisible = false;

		std::shared_ptr<Context> m_Context = nullptr;
	};
}
