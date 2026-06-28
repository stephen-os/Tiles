#pragma once

#include "../Core/Context.h"

#include "Lumina/Lumina.h"

namespace Tiles
{
    class Panel
    {
    public:
        Panel(Ref<Context> context) : m_Context(context) {}
        virtual ~Panel() = default;

        virtual void Render() = 0;
        virtual void Update() = 0;

    protected:
		Ref<Context> m_Context= nullptr;
    };
}