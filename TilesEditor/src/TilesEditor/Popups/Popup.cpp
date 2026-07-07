#include "Popup.h"

namespace Tiles::Editor
{
	void Popup::Render()
	{
		if (!m_IsVisible)
			return;

		OnRender();
	}

	void Popup::Update()
	{
		if (!m_IsVisible)
			return;

		OnUpdate();
	}
}
