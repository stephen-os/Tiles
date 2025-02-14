#include "State.h"

#include <iostream>

void State::PushLayer(size_t index, Layer& layer, StateType type)
{
    if (!m_Layers || index > m_Layers->GetSize())
    {
        std::cerr << "Error: Invalid layer index for PushLayer.\n";
        return;
    }

    std::cout << "PushLayer(" << index << ")\n";

    Action action;
    action.Type = type;
    action.Index = index;
    action.Layer = Layer(layer);

    m_StateStack.push(action);
    Trim();
}

void State::PushTile(size_t y, size_t x, Tile& tile)
{
    if (!m_Layers || x >= m_Layers->GetWidth() || y >= m_Layers->GetHeight())
    {
        std::cerr << "Error: Invalid tile coordinates for PushTile.\n";
        return;
    }

    std::cout << "PushTile(" << y << ", " << x << ")\n";

    Action action;
    action.Type = StateType::Tile_Replace;
    action.Y = y;
    action.X = x;
    action.Tile = Tile(tile);

    m_StateStack.push(action);
    Trim();
}

void State::Trim()
{
    while (m_StateStack.size() > MAX_STACK)
    {
        std::stack<Action> tempStack;
        while (m_StateStack.size() > 1)
        {
            tempStack.push(m_StateStack.top());
            m_StateStack.pop();
        }
        m_StateStack.pop();  // Remove the oldest action

        // Restore the stack
        while (!tempStack.empty())
        {
            m_StateStack.push(tempStack.top());
            tempStack.pop();
        }
    }
}

void State::Undo()
{
    if (m_StateStack.empty())
    {
        std::cerr << "Undo stack is empty.\n";
        return;
    }

    Action action = m_StateStack.top();
    m_StateStack.pop();

    if (!m_Layers)
    {
        std::cerr << "Error: No layers available for undo.\n";
        return;
    }

    switch (action.Type)
    {
    case StateType::Layer_Insert:
        m_Layers->RemoveLayer(action.Index);
        break;

    case StateType::Layer_Delete:
        m_Layers->InsertLayer(action.Index, action.Layer);
        break;

    case StateType::Layer_Replace:
        m_Layers->GetLayer(action.Index) = action.Layer;
        break;

    case StateType::Tile_Replace:
        m_Layers->SetTile(action.Y, action.X, action.Tile);
        break;
    }

    std::cout << "Undo completed.\n";
}
