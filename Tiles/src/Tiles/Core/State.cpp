#include "State.h"

#include <iostream>

namespace Tiles
{

    void State::PushLayer(size_t index, Layer& layer, StateType type)
    {
        if (index > m_Layers->GetSize())
        {
            std::cerr << "Error: Invalid layer index for PushLayer.\n";
            return;
        }

        Action action;
        action.Type = type;
        action.Index = index;
        action.Layer = Layer(layer);

        m_UndoStack.push(action);
        Trim();
    }

    void State::PushTile(size_t y, size_t x, Tile& tile)
    {
        if (x >= m_Layers->GetWidth() || y >= m_Layers->GetHeight())
        {
            std::cerr << "Error: Invalid tile coordinates for PushTile.\n";
            return;
        }
       
        Action action;
        action.Type = StateType::Tile_Replace;
        action.Y = y;
        action.X = x;
        action.Tile = Tile(tile);

        m_UndoStack.push(action);
        Trim();
    }

    void State::Trim()
    {
        while (m_UndoStack.size() > MAX_STACK)
        {
            std::stack<Action> tempStack;
            while (m_UndoStack.size() > 1)
            {
                tempStack.push(m_UndoStack.top());
                m_UndoStack.pop();
            }
            m_UndoStack.pop();

            // Restore the stack
            while (!tempStack.empty())
            {
                m_UndoStack.push(tempStack.top());
                tempStack.pop();
            }
        }
    }

    void State::Undo()
    {
        if (m_UndoStack.empty())
           return;
        
        Action action = m_UndoStack.top();
        m_UndoStack.pop();

        Action redoAction = action;

        switch (action.Type)
        {
        case StateType::Layer_Insert:
            m_Layers->RemoveLayer(action.Index);
            redoAction.Type = StateType::Layer_Delete;
            break;

        case StateType::Layer_Delete:
            m_Layers->InsertLayer(action.Index, action.Layer);
            redoAction.Type = StateType::Layer_Insert;
            break;

        case StateType::Layer_Replace:
            redoAction.Layer = m_Layers->GetLayer(action.Index);
            m_Layers->GetLayer(action.Index) = Layer(action.Layer);
            break;

        case StateType::Tile_Replace:
            // This line is auffle but it works
            // probably expand functions in Layers
            redoAction.Tile = m_Layers->GetLayer(m_Layers->GetActiveLayer()).GetTile(action.Y, action.X);
            m_Layers->SetTile(action.Y, action.X, action.Tile);
            break;
        }

        m_RedoStack.push(redoAction);
    }

    void State::Redo()
    {
        if (m_RedoStack.empty())
            return;

        Action action = m_RedoStack.top();
        m_RedoStack.pop();

        Action undoAction = action;

        switch (action.Type)
        {
        case StateType::Layer_Insert:
            m_Layers->RemoveLayer(action.Index);
            undoAction.Type = StateType::Layer_Delete;
            break;

        case StateType::Layer_Delete:
            m_Layers->InsertLayer(action.Index, action.Layer);
            undoAction.Type = StateType::Layer_Insert;
            break;

        case StateType::Layer_Replace:
            undoAction.Layer = m_Layers->GetLayer(action.Index);
            m_Layers->GetLayer(action.Index) = Layer(action.Layer);
            break;

        case StateType::Tile_Replace:
            undoAction.Tile = m_Layers->GetLayer(m_Layers->GetActiveLayer()).GetTile(action.Y, action.X);
            m_Layers->SetTile(action.Y, action.X, action.Tile);
            break;
        }

        m_UndoStack.push(undoAction);
    }

}