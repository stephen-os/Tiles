#pragma once

#include "EditorIds.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Tiles { class Session; class Workspace; class Texture; class TextureAtlas; }

namespace Tiles::Editor
{
    // Facade handed to every panel and popup. It lets a view reach the shared
    // document state and drive *other* views -- open a popup, toggle a panel, fire
    // an action -- without holding or knowing about them, which is what keeps
    // panels and popups decoupled from one another.
    class EditorHost
    {
    public:
        virtual ~EditorHost() = default;

        // The active document's session state (project, brush, command history).
        virtual Session& Doc() = 0;

        // The app-level workspace: the recent-projects list and the project
        // lifecycle (new / open / save) across open documents.
        virtual Workspace& Space() = 0;

        // Closes the document at index: a clean one closes immediately; a dirty one
        // is switched to and routed through the unsaved-changes prompt.
        virtual void RequestCloseDocument(size_t index) = 0;

        // Closes the active document without prompting (the prompt already ran or the
        // document was clean): evicts its atlas textures, then drops it.
        virtual void CloseActiveConfirmed() = 0;

        // The GL texture backing a document atlas, built and cached on demand;
        // null if the atlas has no image. Lets views render atlases without the
        // document (or the atlas) depending on the graphics layer.
        virtual std::shared_ptr<Tiles::Texture> GetAtlasTexture(const Tiles::TextureAtlas& atlas) = 0;

        // Removes the atlas at index from the active document, dropping its cached
        // GL texture in the same step so a stale cache entry can't outlive -- or be
        // reused by a later atlas allocated at -- the removed atlas's address.
        virtual void RemoveAtlas(size_t index) = 0;

        virtual void OpenPopup(PopupId id) = 0;
        virtual void ClosePopup(PopupId id) = 0;

        virtual void ShowPanel(PanelId id, bool open) = 0;
        virtual void TogglePanel(PanelId id) = 0;
        virtual bool IsPanelOpen(PanelId id) const = 0;
        // Panels offered in the View menu, in registration order, as (id, label).
        virtual const std::vector<std::pair<PanelId, std::string>>& ToggleablePanels() const = 0;

        virtual void Invoke(ActionId id) = 0;
        // Human-readable shortcut label for an action (e.g. "Ctrl+Shift+S"), for
        // display next to a menu item; empty when the action has no binding.
        virtual std::string ShortcutLabel(ActionId id) const = 0;

        // Raises the shared error/notification popup with the given message.
        virtual void Notify(const std::string& message) = 0;
    };
}
