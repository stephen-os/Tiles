#include "PanelManager.h"

namespace Tiles::Editor
{
	Panel* PanelManager::GetPanel(PanelId id)
	{
		auto it = m_PanelIndex.find(id);
		return it == m_PanelIndex.end() ? nullptr : m_Panels[it->second].Ptr.get();
	}

	Popup* PanelManager::GetPopup(PopupId id)
	{
		auto it = m_PopupIndex.find(id);
		return it == m_PopupIndex.end() ? nullptr : m_Popups[it->second].Ptr.get();
	}

	void PanelManager::ShowPanel(PanelId id, bool open)
	{
		if (Panel* panel = GetPanel(id))
			panel->SetOpen(open);
	}

	void PanelManager::TogglePanel(PanelId id)
	{
		if (Panel* panel = GetPanel(id))
			panel->SetOpen(!panel->IsOpen());
	}

	bool PanelManager::IsPanelOpen(PanelId id) const
	{
		auto it = m_PanelIndex.find(id);
		return it != m_PanelIndex.end() && m_Panels[it->second].Ptr->IsOpen();
	}

	void PanelManager::OpenPopup(PopupId id)
	{
		if (Popup* popup = GetPopup(id))
			popup->Show();
	}

	void PanelManager::ClosePopup(PopupId id)
	{
		if (Popup* popup = GetPopup(id))
			popup->Hide();
	}

	void PanelManager::Update()
	{
		for (auto& entry : m_Panels)
		{
			if (entry.Ptr->IsOpen())
				entry.Ptr->Update();
		}

		// Popups gate on their own visibility inside Update().
		for (auto& entry : m_Popups)
			entry.Ptr->Update();
	}

	void PanelManager::Render()
	{
		for (auto& entry : m_Panels)
		{
			if (entry.Ptr->IsOpen())
				entry.Ptr->Render();
		}

		// Rendered after all panels so modals sit at top level, independent of any
		// panel's window/ID scope.
		for (auto& entry : m_Popups)
			entry.Ptr->Render();
	}

	void PanelManager::Clear()
	{
		m_Panels.clear();
		m_Popups.clear();
		m_PanelIndex.clear();
		m_PopupIndex.clear();
		m_ToggleList.clear();
	}
}
