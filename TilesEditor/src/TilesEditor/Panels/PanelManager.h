#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <type_traits>

#include "Panel.h"
#include "../Popups/Popup.h"
#include "../App/EditorIds.h"

namespace Tiles::Editor
{
	// Owns every panel and popup, keyed by id, and drives them each frame. Panels
	// update/render in registration order and are skipped while closed; popups
	// render at top level afterward, so a popup is never tied to the scope of the
	// panel that opened it. This is also the lookup/lifecycle surface that
	// EditorHost forwards to (toggle a panel, open a popup, find a view by id).
	class PanelManager
	{
	public:
		PanelManager() = default;
		~PanelManager() = default;

		// The host handed to every panel/popup as it is constructed. Must be set
		// once before any registration.
		void SetHost(EditorHost& host) { m_Host = &host; }

		// Constructs a panel in place (host injected as its first ctor argument)
		// and takes ownership. Registration order is the per-frame Update/Render
		// order.
		// @param id    Identity for lookup and View-menu toggles.
		// @param title Menu label shown in the View menu.
		// @param args  Extra arguments forwarded after the host to the constructor.
		template<typename TPanel, typename... Args>
		TPanel& RegisterPanel(PanelId id, std::string title, Args&&... args)
		{
			static_assert(std::is_base_of_v<Panel, TPanel>, "TPanel must inherit from Panel");
			auto panel = std::make_unique<TPanel>(*m_Host, std::forward<Args>(args)...);
			TPanel& ref = *panel;
			m_PanelIndex[id] = m_Panels.size();
			m_Panels.push_back({ id, std::move(title), std::move(panel) });
			// The main menu bar is always on, so it is not offered as a toggle.
			if (id != PanelId::MenuBar)
				m_ToggleList.emplace_back(id, m_Panels.back().Title);
			return ref;
		}

		// Constructs a popup in place (host injected as its first ctor argument)
		// and takes ownership.
		template<typename TPopup, typename... Args>
		TPopup& RegisterPopup(PopupId id, Args&&... args)
		{
			static_assert(std::is_base_of_v<Popup, TPopup>, "TPopup must inherit from Popup");
			auto popup = std::make_unique<TPopup>(*m_Host, std::forward<Args>(args)...);
			TPopup& ref = *popup;
			m_PopupIndex[id] = m_Popups.size();
			m_Popups.push_back({ id, std::move(popup) });
			return ref;
		}

		Panel* GetPanel(PanelId id);
		Popup* GetPopup(PopupId id);
		template<typename T> T* GetPanelAs(PanelId id) { return static_cast<T*>(GetPanel(id)); }
		template<typename T> T* GetPopupAs(PopupId id) { return static_cast<T*>(GetPopup(id)); }

		bool HasPanel(PanelId id) const { return m_PanelIndex.find(id) != m_PanelIndex.end(); }

		void ShowPanel(PanelId id, bool open);
		void TogglePanel(PanelId id);
		bool IsPanelOpen(PanelId id) const;

		void OpenPopup(PopupId id);
		void ClosePopup(PopupId id);

		// The panels in registration order, for building the View menu. Each pair
		// is (id, title).
		const std::vector<std::pair<PanelId, std::string>>& ToggleablePanels() const { return m_ToggleList; }

		void Render();
		void Update();
		void Clear();

	private:
		struct PanelEntry { PanelId Id; std::string Title; std::unique_ptr<Panel> Ptr; };
		struct PopupEntry { PopupId Id; std::unique_ptr<Popup> Ptr; };

		std::vector<PanelEntry> m_Panels;
		std::vector<PopupEntry> m_Popups;
		std::unordered_map<PanelId, size_t> m_PanelIndex;
		std::unordered_map<PopupId, size_t> m_PopupIndex;
		std::vector<std::pair<PanelId, std::string>> m_ToggleList;

		EditorHost* m_Host = nullptr;
	};
}
