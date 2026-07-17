#pragma once

#include "imgui.h"

namespace Tiles::UI
{
	// RAII guard that pushes ImGui style colors and pops exactly as many when it
	// leaves scope, replacing hand-balanced PushStyleColor / PopStyleColor(n) --
	// the standing source of push/pop count mismatches. Marked [[nodiscard]] on the
	// type so a guard created as a bare temporary (which pops immediately and does
	// nothing) warns at the call site.
	class [[nodiscard]] StyleColorScope
	{
	public:
		StyleColorScope() = default;

		// Constructs already pushing one color.
		StyleColorScope(ImGuiCol index, const ImVec4& color) { Push(index, color); }

		// Pops every color this scope pushed.
		~StyleColorScope() { if (m_Count > 0) ImGui::PopStyleColor(m_Count); }

		// A scope owns its pushes; copying or moving would double-pop.
		StyleColorScope(const StyleColorScope&) = delete;
		StyleColorScope& operator=(const StyleColorScope&) = delete;

		// Pushes one more color; chainable, all popped together at scope end.
		StyleColorScope& Push(ImGuiCol index, const ImVec4& color)
		{
			ImGui::PushStyleColor(index, color);
			++m_Count;
			return *this;
		}

	private:
		int m_Count = 0;
	};

	// As StyleColorScope, but for style vars. A distinct type because ImGui pops
	// colors and vars through different calls.
	class [[nodiscard]] StyleVarScope
	{
	public:
		StyleVarScope() = default;

		// Constructs already pushing one float- or ImVec2-valued var.
		StyleVarScope(ImGuiStyleVar index, float value) { Push(index, value); }
		StyleVarScope(ImGuiStyleVar index, const ImVec2& value) { Push(index, value); }

		// Pops every var this scope pushed.
		~StyleVarScope() { if (m_Count > 0) ImGui::PopStyleVar(m_Count); }

		// A scope owns its pushes; copying or moving would double-pop.
		StyleVarScope(const StyleVarScope&) = delete;
		StyleVarScope& operator=(const StyleVarScope&) = delete;

		// Pushes one more float-valued var; chainable.
		StyleVarScope& Push(ImGuiStyleVar index, float value)
		{
			ImGui::PushStyleVar(index, value);
			++m_Count;
			return *this;
		}

		// Pushes one more ImVec2-valued var; chainable.
		StyleVarScope& Push(ImGuiStyleVar index, const ImVec2& value)
		{
			ImGui::PushStyleVar(index, value);
			++m_Count;
			return *this;
		}

	private:
		int m_Count = 0;
	};
}
