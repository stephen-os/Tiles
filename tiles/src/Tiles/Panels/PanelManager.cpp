#include "PanelManager.h"

namespace Tiles
{
	void PanelManager::Render()
	{
		for (auto& panel : m_Panels)
		{
			panel->Render();
		}
	}

	void PanelManager::Update()
	{
		for (auto& panel : m_Panels)
		{
			panel->Update();
		}
	}

	void PanelManager::Clear()
	{
		m_Panels.clear(); 
	}

	void PanelManager::SetContext(Context* context)
	{
		for (auto& panel : m_Panels)
		{
			panel->SetContext(context);
		}
	}
}