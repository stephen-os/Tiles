#include "Tiles.h"

#include "Core/Event.h"
#include "Core/Log.h"

#include "EntryPoint.h"

#include "App/EditorHost.h"
#include "App/ActionRegistry.h"

#include "Panels/PanelManager.h"
#include "Panels/PanelLayerSelection.h"
#include "Panels/PanelToolSelection.h"
#include "Panels/PanelTextureSelection.h"
#include "Panels/PanelBrushPreview.h"
#include "Panels/PanelBrushAttributes.h"
#include "Panels/PanelMenuBar.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelDebug.h"

#include "Popups/PopupNewProject.h"
#include "Popups/PopupAbout.h"
#include "Popups/PopupError.h"
#include "Popups/PopupSaveAs.h"
#include "Popups/PopupOpenProject.h"
#include "Popups/PopupRenderMatrix.h"

#include "EditorTheme.h"

#include <memory>
#include <string>

using namespace Tiles;
using namespace Tiles::Editor;

// The editor's single application layer. It owns the shared Context, the panel/
// popup registry, and the action map, and implements EditorHost so any panel or
// popup can reach the document, open another view, or fire an action through the
// same facade. Global keyboard shortcuts arrive via OnEvent and are resolved
// against the action map here.
class TilesEditorLayer : public Layer, public EditorHost
{
public:
	TilesEditorLayer() : Layer("Tiles Editor Layer") {}

	void OnAttach() override
	{
		// The layer solely owns one Context; every panel and popup borrows it
		// through EditorHost::Doc(), so they all read and mutate the same project,
		// brush, working layer, and command history.
		m_Context = std::make_unique<Context>();

		m_Panels.SetHost(*this);
		RegisterPanels();
		RegisterPopups();
		RegisterActions();
	}

	void OnDetach() override
	{
		m_Panels.Clear();
	}

	void OnUpdate(float timestep) override
	{
		m_Panels.Update();
	}

	void OnUIRender() override
	{
		m_Panels.Render();
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) { return OnKeyPressed(e); });
	}

	// --- EditorHost ---------------------------------------------------------

	Context& Doc() override { return *m_Context; }

	void OpenPopup(PopupId id) override { m_Panels.OpenPopup(id); }
	void ClosePopup(PopupId id) override { m_Panels.ClosePopup(id); }

	void ShowPanel(PanelId id, bool open) override { m_Panels.ShowPanel(id, open); }
	void TogglePanel(PanelId id) override { m_Panels.TogglePanel(id); }
	bool IsPanelOpen(PanelId id) const override { return m_Panels.IsPanelOpen(id); }
	const std::vector<std::pair<PanelId, std::string>>& ToggleablePanels() const override { return m_Panels.ToggleablePanels(); }

	void Invoke(ActionId id) override { m_Actions.Invoke(id); }
	std::string ShortcutLabel(ActionId id) const override { return m_Actions.GetShortcutLabel(id); }

	void Notify(const std::string& message) override
	{
		if (auto* popup = m_Panels.GetPopupAs<PopupError>(PopupId::Error))
		{
			popup->SetMessage(message);
			popup->Show();
		}
	}

private:
	// Routes a key press to the action whose shortcut matches. Auto-repeat is
	// ignored so command shortcuts fire once per press.
	bool OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		Shortcut shortcut{ e.GetKey(), e.GetMods() };

		// Ctrl+Shift+Z is a second binding for redo alongside Ctrl+Y; the action
		// map holds one shortcut per action, so this alias is resolved here.
		if (shortcut.Key == KeyCode::Z && shortcut.Mods == (KeyMods::Control | KeyMods::Shift))
		{
			m_Actions.Invoke(ActionId::Redo);
			return true;
		}

		return m_Actions.InvokeFor(shortcut);
	}

	void RegisterPanels()
	{
		m_Panels.RegisterPanel<PanelMenuBar>(PanelId::MenuBar, "Menu Bar");
		m_Panels.RegisterPanel<PanelViewport>(PanelId::Viewport, "Viewport");
		m_Panels.RegisterPanel<PanelLayerSelection>(PanelId::LayerSelection, "Layer Selection");
		m_Panels.RegisterPanel<PanelToolSelection>(PanelId::ToolSelection, "Tools");
		m_Panels.RegisterPanel<PanelTextureSelection>(PanelId::TextureSelection, "Texture Selection");
		m_Panels.RegisterPanel<PanelBrushPreview>(PanelId::BrushPreview, "Brush Preview");
		m_Panels.RegisterPanel<PanelBrushAttributes>(PanelId::BrushAttributes, "Brush Attributes");
#ifdef TILES_DEBUG
		m_Panels.RegisterPanel<PanelDebug>(PanelId::Debug, "Debug Panel");
#endif
	}

	void RegisterPopups()
	{
		m_Panels.RegisterPopup<PopupNewProject>(PopupId::NewProject);
		m_Panels.RegisterPopup<PopupAbout>(PopupId::About);
		m_Panels.RegisterPopup<PopupError>(PopupId::Error);
		m_Panels.RegisterPopup<PopupSaveAs>(PopupId::SaveAs);
		m_Panels.RegisterPopup<PopupOpenProject>(PopupId::OpenProject);
		m_Panels.RegisterPopup<PopupRenderMatrix>(PopupId::Export);
	}

	void RegisterActions()
	{
		m_Actions.Register(ActionId::NewProject,
			[this] { OpenPopup(PopupId::NewProject); },
			{ KeyCode::N, KeyMods::Control });

		m_Actions.Register(ActionId::OpenProject,
			[this] { OpenPopup(PopupId::OpenProject); },
			{ KeyCode::O, KeyMods::Control });

		m_Actions.Register(ActionId::Save,
			[this] { SaveProject(); },
			{ KeyCode::S, KeyMods::Control });

		m_Actions.Register(ActionId::SaveAs,
			[this] { if (m_Context->HasProject()) OpenPopup(PopupId::SaveAs); },
			{ KeyCode::S, KeyMods::Control | KeyMods::Shift });

		m_Actions.Register(ActionId::Export,
			[this] { if (m_Context->HasProject()) OpenPopup(PopupId::Export); },
			{ KeyCode::E, KeyMods::Control });

		m_Actions.Register(ActionId::Undo,
			[this] { if (m_Context->CanUndo()) m_Context->Undo(); },
			{ KeyCode::Z, KeyMods::Control });

		m_Actions.Register(ActionId::Redo,
			[this] { if (m_Context->CanRedo()) m_Context->Redo(); },
			{ KeyCode::Y, KeyMods::Control });

		m_Actions.Register(ActionId::ClearHistory,
			[this] { if (m_Context->HasProject()) m_Context->ClearHistory(); });
	}

	// Saves in place, falling back to the Save-As dialog for a never-saved
	// project; a failed save surfaces through the shared error popup.
	void SaveProject()
	{
		if (!m_Context->HasProject())
			return;

		auto project = m_Context->GetProject();
		if (project->IsNew() && project->HasUnsavedChanges())
		{
			OpenPopup(PopupId::SaveAs);
		}
		else
		{
			ProjectResult result = m_Context->SaveProject();
			if (!result.Success)
				Notify(result.Message);
		}
	}

	std::unique_ptr<Context> m_Context;
	PanelManager m_Panels;
	ActionRegistry m_Actions;
};

class TilesEditor : public Application
{
public:
	TilesEditor(const ApplicationSettings& spec)
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
	spec.Window.Title = "Tiles";
	spec.Window.IconPath = "res/assets/bucket.png";
	spec.Window.Width = 1920;
	spec.Window.Height = 1080;
	spec.Use2DRenderer = true;
	spec.Window.Maximized = true;

	Tiles::Application* app = new TilesEditor(spec);

	return app;
}
