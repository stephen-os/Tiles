#include "Tiles.h"

#include "EntryPoint.h"

#include "Panels/PanelManager.h"
#include "Panels/PanelLayerSelection.h"
#include "Panels/PanelToolSelection.h"
#include "Panels/PanelTextureSelection.h"
#include "Panels/PanelBrushPreview.h"
#include "Panels/PanelBrushAttributes.h"
#include "Panels/PanelMenuBar.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelDebug.h"

class TilesEditorLayer : public Tiles::Layer
{
public:
	TilesEditorLayer() : Layer("Tiles Editor Layer") {}

	void OnAttach() override
	{
		// One Context is shared by every panel so they all read and mutate the
		// same project, brush, working layer, and command history.
		std::shared_ptr<Tiles::Context> context = Tiles::Context::Create();

		m_PanelManager.RegisterPanel<Tiles::PanelLayerSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelToolSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelTextureSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelBrushPreview>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelBrushAttributes>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelMenuBar>(context);
		m_PanelManager.RegisterPanel<Tiles::PanelViewport>(context);
#ifdef TILES_DEBUG
		m_PanelManager.RegisterPanel<Tiles::PanelDebug>(context);
#endif
	}

	void OnDetach() override
	{
		m_PanelManager.Clear();
	}

	void OnUpdate(float timestep) override
	{
		m_PanelManager.Update();
	}

	void OnUIRender() override
	{
		m_PanelManager.Render();
	}

	void OnEvent() override
	{

	}

private:
	Tiles::PanelManager m_PanelManager;
};

class TilesEditor : public Tiles::Application
{
public:
	TilesEditor(const Tiles::ApplicationSpecification& spec)
		: Application(spec)
	{
	}

	void OnCreate() override
	{
		PushLayer<TilesEditorLayer>();
	}
	void OnDestroy() override
	{
	}
};

Tiles::Application* Tiles::CreateApplication(int argc, char** argv)
{
	Tiles::ApplicationSpecification spec;
	spec.Name = "Tiles";
	spec.Icon = "res/assets/bucket.png";
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Use2DRenderer = true;
	spec.Maximized = true;

	Tiles::Application* app = new TilesEditor(spec);

	return app;
}

