#pragma once

#include "EditorIds.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace Tiles::Editor
{
    // Central table of editor actions. Each action binds an id to a callback and
    // an optional keyboard shortcut, giving the menu bar and the event router a
    // single source of truth for what a command does and how it is invoked.
    class ActionRegistry
    {
    public:
        void Register(ActionId id, std::function<void()> run, Shortcut shortcut = {});

        // Runs the action bound to id. @return true if an action ran.
        bool Invoke(ActionId id);

        // Runs the action whose shortcut matches, if any. Used by the event router.
        // @return true if a bound action consumed the shortcut.
        bool InvokeFor(const Shortcut& shortcut);

        // Human-readable shortcut label (e.g. "Ctrl+Shift+S"), or empty when the
        // action is unbound or has no shortcut.
        std::string GetShortcutLabel(ActionId id) const;

    private:
        struct Action
        {
            std::function<void()> Run;
            Shortcut Key;
        };

        std::unordered_map<ActionId, Action> m_Actions;
    };
}
