#pragma once

#include "Core/KeyCodes.h"

namespace Tiles::Editor
{
    // The editor names input-code types (from Tiles::Input) directly in its
    // shortcuts and actions; surface the ones it uses so they need no qualifying.
    using Input::KeyCode;
    using Input::KeyMods;
    using Input::Shortcut;
    using Input::HasMod;

    // Panel id's
    enum class PanelId
    {
		Viewport,           // The main canvas where the user paints tiles
		LayerSelection,     // The panel that allows the user to select and manage layers
		ToolSelection,      // The panel that allows the user to select the active tool (brush, eraser, fill)
		TextureSelection,   // The panel that allows the user to select the active texture for painting
		BrushPreview,       // The panel that shows a preview of the current brush
		BrushAttributes,    // The panel that allows the user to adjust brush attributes (size, shape, etc.)
		MenuBar,            // The top menu bar with File, Edit, View, Help menus
		Debug               // The debug panel that shows internal state for development purposes
    };

	// Popup id's 
    enum class PopupId
    {
		NewProject,         // Popup for creating a new project
		About,              // Popup that shows information about the application
		Error,              // Popup that shows error messages
		SaveAs,             // Popup for saving the current project under a new name
		OpenProject,        // Popup for opening an existing project
		Export              // Popup for exporting the current project (e.g., to a render matrix)
    };

	// Action id's
    enum class ActionId
    {
		NewProject,         // Action to create a new project
		OpenProject,        // Action to open an existing project
		Save,               // Action to save the current project
		SaveAs,             // Action to save the current project under a new name
		Export,             // Action to export the current project (e.g., to a render matrix)
		Undo,               // Action to undo the last change
		Redo,               // Action to redo the last undone change
		ClearHistory		// Action to clear the undo/redo history
    };
}
