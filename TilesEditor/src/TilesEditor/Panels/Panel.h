#pragma once

#include "Core/Context.h"

#include "Tiles.h"

namespace Tiles
{
    /// Base class for editor panels. Each panel holds a shared reference to the
    /// editor Context (the single source of project/brush/command state) and is
    /// driven once per frame by PanelManager: Update() first, then Render().
    class Panel
    {
    public:
        Panel(std::shared_ptr<Context> context) : m_Context(context) {}
        virtual ~Panel() = default;

        virtual void Render() = 0;
        virtual void Update() = 0;

    protected:
		std::shared_ptr<Context> m_Context= nullptr;
    };
}