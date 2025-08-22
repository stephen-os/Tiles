#include "Editor.h"

#include "imgui.h"

#include "Panels/PanelLayerSelection.h"
#include "Panels/PanelToolSelection.h"
#include "Panels/PanelTextureSelection.h"
#include "Panels/PanelBrushPreview.h"
#include "Panels/PanelBrushAttributes.h"
#include "Panels/PanelMenuBar.h"
#include "Panels/PanelViewport.h"

#include "Lumina/Core/Log.h"

#include "Core/Context.h" 

namespace Tiles
{
    void Editor::OnAttach()
    {
        Ref<Context> context = Context::Create();

        m_PanelManager.RegisterPanel<PanelLayerSelection>(context);
        m_PanelManager.RegisterPanel<PanelToolSelection>(context);
        m_PanelManager.RegisterPanel<PanelTextureSelection>(context);
        m_PanelManager.RegisterPanel<PanelBrushPreview>(context);
		m_PanelManager.RegisterPanel<PanelBrushAttributes>(context);
		m_PanelManager.RegisterPanel<PanelMenuBar>(context);
        m_PanelManager.RegisterPanel<PanelViewport>(context);
#ifdef LUMINA_DEBUG

#endif
    }

    void Editor::OnDetach()
    {
        m_PanelManager.Clear();
    }

    void Editor::OnUpdate(float timestep)
    {
        m_PanelManager.Update(); 
    }

    void Editor::OnUIRender()
    {
        m_PanelManager.Render();
    }
}