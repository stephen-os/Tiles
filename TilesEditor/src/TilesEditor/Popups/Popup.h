#pragma once

#include "Session/Context.h"

#include "../App/EditorHost.h"

namespace Tiles::Editor
{
	// Base class for dialog-style popups. Tracks a visibility flag toggled via
	// Show/Hide/Toggle; Render() and Update() dispatch to the derived OnRender/
	// OnUpdate only while visible, so callers may call them unconditionally each
	// frame. Popups are registered with and rendered by PanelManager at top level,
	// so they are never owned by the panel that opens them, and they reach shared
	// state through the EditorHost facade rather than holding it.
	class Popup
	{
	public:
		Popup(EditorHost& host) : m_Host(host) {}
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

		EditorHost& Host() const { return m_Host; }
		// Shorthand for the shared document/session state.
		Context& Ctx() const { return m_Host.Doc(); }

	protected:
		bool m_IsVisible = false;

		EditorHost& m_Host;
	};
}
