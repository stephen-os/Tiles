#pragma once

#include <vector>
#include <memory>

#include "Panel.h"

namespace Tiles
{
	class PanelManager
	{
	public:
		PanelManager() = default;
		~PanelManager() = default;

		template<typename TPanel, typename... Args>
		void RegisterPanel(Args&&... args)
		{
			static_assert(std::is_base_of_v<Panel, TPanel>,"PanelType must inherit from Panel");
			m_Panels.push_back(std::make_unique<TPanel>(std::forward<Args>(args)...));
		}

		void Clear();

		void Render();
		void Update();

	private:
		std::vector<std::unique_ptr<Panel>> m_Panels;
	};
}


