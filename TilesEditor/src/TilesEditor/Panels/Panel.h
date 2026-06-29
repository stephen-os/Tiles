#pragma once

#include "../Core/Context.h"

#include "../Lumina.h"

namespace Tiles
{
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