#include "ActionRegistry.h"

namespace Tiles::Editor
{
    namespace
    {
        std::string KeyName(KeyCode key)
        {
            auto value = static_cast<uint16_t>(key);
            if (value >= static_cast<uint16_t>(KeyCode::A) && value <= static_cast<uint16_t>(KeyCode::Z))
                return std::string(1, static_cast<char>('A' + (value - static_cast<uint16_t>(KeyCode::A))));
            return "?";
        }

        std::string ShortcutToString(const Shortcut& shortcut)
        {
            std::string label;
            if (HasMod(shortcut.Mods, KeyMods::Control)) label += "Ctrl+";
            if (HasMod(shortcut.Mods, KeyMods::Shift))   label += "Shift+";
            if (HasMod(shortcut.Mods, KeyMods::Alt))     label += "Alt+";
            label += KeyName(shortcut.Key);
            return label;
        }
    }

    void ActionRegistry::Register(ActionId id, std::function<void()> run, Shortcut shortcut)
    {
        m_Actions[id] = Action{ std::move(run), shortcut };
    }

    bool ActionRegistry::Invoke(ActionId id)
    {
        auto it = m_Actions.find(id);
        if (it == m_Actions.end() || !it->second.Run)
            return false;

        it->second.Run();
        return true;
    }

    bool ActionRegistry::InvokeFor(const Shortcut& shortcut)
    {
        if (!shortcut.IsBound())
            return false;

        for (auto& [id, action] : m_Actions)
        {
            if (action.Key.IsBound() && action.Key == shortcut && action.Run)
            {
                action.Run();
                return true;
            }
        }
        return false;
    }

    std::string ActionRegistry::GetShortcutLabel(ActionId id) const
    {
        auto it = m_Actions.find(id);
        if (it == m_Actions.end() || !it->second.Key.IsBound())
            return {};

        return ShortcutToString(it->second.Key);
    }
}
