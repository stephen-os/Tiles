#include "Tiles.h"

#include "Core/Event.h"
#include "Core/Logger.h"

#include "Session/Workspace.h"

#include "EntryPoint.h"

#include "App/EditorHost.h"
#include "App/ActionRegistry.h"

#include "Rendering/AtlasTextureCache.h"

#include "Panels/PanelManager.h"
#include "Panels/PanelLayerSelection.h"
#include "Panels/PanelToolSelection.h"
#include "Panels/PanelTextureSelection.h"
#include "Panels/PanelBrushPreview.h"
#include "Panels/PanelRenderPreview.h"
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
#include "Popups/PopupConfirmClose.h"
#include "Popups/PopupConfirmQuit.h"

#include "UI/Theme.h"
#include "UI/Fonts.h"

#include <memory>
#include <string>

using namespace Tiles;
using namespace Tiles::Editor;

// The editor's single application layer. It owns the shared Session, the panel/
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
		// The layer owns the Workspace: the app-level recent list plus the active
		// editing Session every panel and popup borrows through EditorHost::Doc().
		m_Workspace = std::make_unique<Workspace>();

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
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&) { return OnWindowClose(); });
	}

	// --- EditorHost ---------------------------------------------------------

	Session& Doc() override { return m_Workspace->Active(); }

	Workspace& Space() override { return *m_Workspace; }

	void RequestCloseDocument(size_t index) override
	{
		// Show the document being closed, then close it (clean) or prompt (dirty).
		m_Workspace->SwitchTo(index);
		if (Doc().IsDirty())
			OpenPopup(PopupId::ConfirmClose);
		else
			CloseActiveConfirmed();
	}

	void CloseActiveConfirmed() override
	{
		// Drop the closing document's atlas textures before it (and they) are freed.
		if (Doc().HasProject())
			for (const auto& atlas : Doc().GetProject()->GetTextureAtlases())
				if (atlas)
					m_AtlasTextures.Evict(*atlas);

		m_Workspace->CloseDocument(m_Workspace->ActiveIndex());
	}

	std::shared_ptr<Tiles::Texture> GetAtlasTexture(const Tiles::TextureAtlas& atlas) override { return m_AtlasTextures.Get(atlas); }

	void RemoveAtlas(size_t index) override
	{
		if (!Doc().HasProject())
			return;

		auto project = Doc().GetProject();
		if (index >= project->GetTextureAtlasCount())
			return;

		// Evict the cached texture before the atlas is destroyed, so its raw-pointer
		// key can't dangle -- or alias a future atlas allocated at the same address.
		if (auto atlas = project->GetTextureAtlas(index))
			m_AtlasTextures.Evict(*atlas);

		project->RemoveTextureAtlas(index);
	}

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

	// Vetoes a window close while any document has unsaved changes, raising the
	// quit-confirmation popup instead; otherwise lets the app close.
	bool OnWindowClose()
	{
		if (m_Workspace->UnsavedDocumentCount() > 0)
		{
			OpenPopup(PopupId::ConfirmQuit);
			return true;   // handled = veto the quit
		}

		return false;
	}

	void RegisterPanels()
	{
		m_Panels.RegisterPanel<PanelMenuBar>(PanelId::MenuBar, "Menu Bar");
		m_Panels.RegisterPanel<PanelViewport>(PanelId::Viewport, "Viewport");
		m_Panels.RegisterPanel<PanelLayerSelection>(PanelId::LayerSelection, "Layer Selection");
		m_Panels.RegisterPanel<PanelToolSelection>(PanelId::ToolSelection, "Tools");
		m_Panels.RegisterPanel<PanelTextureSelection>(PanelId::TextureSelection, "Texture Selection");
		m_Panels.RegisterPanel<PanelBrushPreview>(PanelId::BrushPreview, "Brush Preview");
		m_Panels.RegisterPanel<PanelRenderPreview>(PanelId::RenderPreview, "Render Preview");
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
		m_Panels.RegisterPopup<PopupConfirmClose>(PopupId::ConfirmClose);
		m_Panels.RegisterPopup<PopupConfirmQuit>(PopupId::ConfirmQuit);
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
			[this] { if (Doc().HasProject()) OpenPopup(PopupId::SaveAs); },
			{ KeyCode::S, KeyMods::Control | KeyMods::Shift });

		m_Actions.Register(ActionId::Export,
			[this] { if (Doc().HasProject()) OpenPopup(PopupId::Export); },
			{ KeyCode::E, KeyMods::Control });

		m_Actions.Register(ActionId::Undo,
			[this] { if (Doc().CanUndo()) Doc().Undo(); },
			{ KeyCode::Z, KeyMods::Control });

		m_Actions.Register(ActionId::Redo,
			[this] { if (Doc().CanRedo()) Doc().Redo(); },
			{ KeyCode::Y, KeyMods::Control });

		m_Actions.Register(ActionId::ClearHistory,
			[this] { if (Doc().HasProject()) Doc().ClearHistory(); });
	}

	// Saves in place, falling back to the Save-As dialog for a never-saved
	// project; a failed save surfaces through the shared error popup.
	void SaveProject()
	{
		if (!Doc().HasProject())
			return;

		auto project = Doc().GetProject();
		if (project->IsNew() && project->HasUnsavedChanges())
		{
			OpenPopup(PopupId::SaveAs);
		}
		else
		{
			auto result = m_Workspace->SaveProject();
			if (!result)
				Notify(result.error().message);
		}
	}

	std::unique_ptr<Workspace> m_Workspace;
	PanelManager m_Panels;
	ActionRegistry m_Actions;

	// GPU textures for open documents' atlases; an entry is evicted when its
	// document closes (see CloseActiveConfirmed).
	AtlasTextureCache m_AtlasTextures;
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
		// Install the shared Tiles::UI theme + fonts.
		Tiles::UI::InitFonts();
		Tiles::UI::Apply(Tiles::UI::Theme{});
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
	spec.Window.Maximized = true;

	Tiles::Application* app = new TilesEditor(spec);

	return app;
}
