#include "Popup.h"

namespace Tiles
{
    Popup::Popup(std::shared_ptr<Context> context) : m_Context(context){}

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