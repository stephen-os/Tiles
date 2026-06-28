#pragma once

#include "Core/Base.h"
#include "Core/Context.h"

namespace Tiles
{
	class Popup
	{
	public:
		Popup(Ref<Context> context);
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

		Ref<Context> m_Context = nullptr;
	};
}
