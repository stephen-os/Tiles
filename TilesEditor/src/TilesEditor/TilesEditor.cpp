#include "Tiles.h"

#include "Core/Event.h"
#include "Core/Log.h"

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

#include "EditorTheme.h"

class TilesEditorLayer : public Tiles::Layer
{
public:
	TilesEditorLayer() : Layer("Tiles Editor Layer") {}

	void OnAttach() override
	{
		// One Context is shared by every panel so they all read and mutate the
		// same project, brush, working layer, and command history.
		std::shared_ptr<Tiles::Context> context = Tiles::Context::Create();

		m_PanelManager.RegisterPanel<Tiles::Editor::PanelLayerSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelToolSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelTextureSelection>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelBrushPreview>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelBrushAttributes>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelMenuBar>(context);
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelViewport>(context);
#ifdef TILES_DEBUG
		m_PanelManager.RegisterPanel<Tiles::Editor::PanelDebug>(context);
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

	void OnEvent(Tiles::Event& event) override
	{
		Tiles::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Tiles::KeyPressedEvent>([this](Tiles::KeyPressedEvent& e) { return OnKeyPressed(e); });
		dispatcher.Dispatch<Tiles::MouseButtonPressedEvent>([this](Tiles::MouseButtonPressedEvent& e) { return OnMouseButtonPressed(e); });
		dispatcher.Dispatch<Tiles::MouseScrolledEvent>([this](Tiles::MouseScrolledEvent& e) { return OnMouseScrolled(e); });
	}

private:
	// SCAFFOLD: mirrors the keybinds the editor currently drives via Input polling
	// (viewport) and ImGui shortcuts (menu bar). Each case only logs its intent for
	// now; the real actions get wired here once popups/commands are made agnostic to
	// the panel they live in. NOTE: KeyPressedEvent carries no modifier state yet, so
	// the Ctrl-combos can't be told apart from the bare keys until modifiers are added
	// to the event -- both intents are documented per key.
	bool OnKeyPressed(Tiles::KeyPressedEvent& e)
	{
		switch (e.GetKey())
		{
		// Ctrl+N -> create a new project
		case Tiles::KeyCode::N:
			TILES_INFO("[Event] Key N -> New Project (Ctrl+N)");
			break;
		// Ctrl+O -> open an existing project
		case Tiles::KeyCode::O:
			TILES_INFO("[Event] Key O -> Open Project (Ctrl+O)");
			break;
		// Ctrl+E -> open the Export (Render Matrix) popup;  E -> zoom the viewport out
		case Tiles::KeyCode::E:
			TILES_INFO("[Event] Key E -> Export (Ctrl+E) / zoom out (viewport)");
			break;
		// Ctrl+S -> save the project (Save As when it is new)
		case Tiles::KeyCode::S:
			TILES_INFO("[Event] Key S -> Save (Ctrl+S) / pan camera down (viewport)");
			break;
		// Ctrl+Z -> undo;  Ctrl+Shift+Z -> redo
		case Tiles::KeyCode::Z:
			TILES_INFO("[Event] Key Z -> Undo (Ctrl+Z) / Redo (Ctrl+Shift+Z)");
			break;
		// Ctrl+Y -> redo
		case Tiles::KeyCode::Y:
			TILES_INFO("[Event] Key Y -> Redo (Ctrl+Y)");
			break;
		// W -> pan the viewport camera up
		case Tiles::KeyCode::W:
			TILES_INFO("[Event] Key W -> pan camera up (viewport)");
			break;
		// A -> pan the viewport camera left
		case Tiles::KeyCode::A:
			TILES_INFO("[Event] Key A -> pan camera left (viewport)");
			break;
		// D -> pan the viewport camera right
		case Tiles::KeyCode::D:
			TILES_INFO("[Event] Key D -> pan camera right (viewport)");
			break;
		// Q -> zoom the viewport camera in
		case Tiles::KeyCode::Q:
			TILES_INFO("[Event] Key Q -> zoom in (viewport)");
			break;
		default:
			break;
		}
		return false;   // scaffold: never consumes the event
	}

	bool OnMouseButtonPressed(Tiles::MouseButtonPressedEvent& e)
	{
		switch (e.GetButton())
		{
		// Left click -> paint the active tool (brush / eraser / fill) at the hovered cell
		case Tiles::MouseCode::Left:
			TILES_INFO("[Event] Mouse Left -> paint action (brush/erase/fill)");
			break;
		// Middle button -> begin a camera pan drag in the viewport
		case Tiles::MouseCode::Middle:
			TILES_INFO("[Event] Mouse Middle -> begin camera pan (viewport)");
			break;
		// Right button -> continue the camera pan while a drag is active
		case Tiles::MouseCode::Right:
			TILES_INFO("[Event] Mouse Right -> pan camera while dragging (viewport)");
			break;
		default:
			break;
		}
		return false;   // scaffold: never consumes the event
	}

	bool OnMouseScrolled(Tiles::MouseScrolledEvent& e)
	{
		// Mouse wheel -> zoom the viewport camera
		TILES_INFO("[Event] Mouse wheel {} -> zoom camera (viewport)", e.GetYOffset());
		return false;   // scaffold: never consumes the event
	}

	Tiles::Editor::PanelManager m_PanelManager;
};

class TilesEditor : public Tiles::Application
{
public:
	TilesEditor(const Tiles::ApplicationSettings& spec)
		: Application(spec)
	{
	}

	void OnCreate() override
	{
		Tiles::Editor::ApplyTheme();
		PushLayer<TilesEditorLayer>();
	}
	void OnDestroy() override
	{
	}
};

Tiles::Application* Tiles::CreateApplication(int argc, char** argv)
{
	Tiles::ApplicationSettings spec;
	spec.Name = "Tiles";
	spec.Icon = "res/assets/bucket.png";
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Use2DRenderer = true;
	spec.Maximized = true;

	Tiles::Application* app = new TilesEditor(spec);

	return app;
}

