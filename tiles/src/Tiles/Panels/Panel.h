#pragma once

#include "../Core/Context.h"

namespace Tiles
{
    class Panel
    {
    public:
        Panel() = default;
        virtual ~Panel() = default;

        // Called every frame to update the panel's context reference
        virtual void SetContext(Context* context) { m_Context = context; }

        // Called to render the panel
        virtual void Render() = 0;
        virtual void Update() = 0;

    protected:
        // Helper to check if we have a valid context
        bool HasContext() const { return m_Context != nullptr; }

        // Protected access to context
        Context* m_Context = nullptr;
    };
}