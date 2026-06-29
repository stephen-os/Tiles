#pragma once

#include "ICommand.h"
#include "../../Domain/Entities/TileGrid.h"
#include "../../Domain/ValueObjects/Position.h"
#include "../../Domain/ValueObjects/TileData.h"

#include <vector>

namespace Tiles::Services
{
    // Command for painting a single tile
    class PaintTileCommand : public ICommand
    {
    public:
        PaintTileCommand(Domain::TileGrid& layer, const Domain::Position& position,
                         const Domain::TileData& newTile)
            : m_Layer(layer)
            , m_Position(position)
            , m_NewTile(newTile)
            , m_OldTile(layer.GetTile(position))
        {}

        void Execute() override
        {
            m_Layer.SetTile(m_Position, m_NewTile);
        }

        void Undo() override
        {
            m_Layer.SetTile(m_Position, m_OldTile);
        }

        std::string GetDescription() const override
        {
            return "Paint Tile";
        }

        bool CanMergeWith(const ICommand& other) const override
        {
            // Can merge continuous paint strokes
            return dynamic_cast<const PaintTileCommand*>(&other) != nullptr;
        }

    private:
        Domain::TileGrid& m_Layer;
        Domain::Position m_Position;
        Domain::TileData m_NewTile;
        Domain::TileData m_OldTile;
    };

    // Command for painting multiple tiles at once (stroke)
    class PaintStrokeCommand : public ICommand
    {
    public:
        struct TileChange
        {
            Domain::Position Position;
            Domain::TileData OldTile;
            Domain::TileData NewTile;
        };

        PaintStrokeCommand(Domain::TileGrid& layer)
            : m_Layer(layer)
        {}

        void AddTile(const Domain::Position& position, const Domain::TileData& newTile)
        {
            // Check if we already have this position
            for (auto& change : m_Changes)
            {
                if (change.Position == position)
                {
                    change.NewTile = newTile;
                    return;
                }
            }

            m_Changes.push_back({
                position,
                m_Layer.GetTile(position),
                newTile
            });
        }

        void Execute() override
        {
            for (const auto& change : m_Changes)
            {
                m_Layer.SetTile(change.Position, change.NewTile);
            }
        }

        void Undo() override
        {
            for (const auto& change : m_Changes)
            {
                m_Layer.SetTile(change.Position, change.OldTile);
            }
        }

        std::string GetDescription() const override
        {
            return "Paint Stroke (" + std::to_string(m_Changes.size()) + " tiles)";
        }

        bool IsEmpty() const { return m_Changes.empty(); }
        size_t GetTileCount() const { return m_Changes.size(); }

    private:
        Domain::TileGrid& m_Layer;
        std::vector<TileChange> m_Changes;
    };

    // Command for erasing a tile
    class EraseTileCommand : public ICommand
    {
    public:
        EraseTileCommand(Domain::TileGrid& layer, const Domain::Position& position)
            : m_Layer(layer)
            , m_Position(position)
            , m_OldTile(layer.GetTile(position))
        {}

        void Execute() override
        {
            m_Layer.ClearTile(m_Position);
        }

        void Undo() override
        {
            m_Layer.SetTile(m_Position, m_OldTile);
        }

        std::string GetDescription() const override
        {
            return "Erase Tile";
        }

    private:
        Domain::TileGrid& m_Layer;
        Domain::Position m_Position;
        Domain::TileData m_OldTile;
    };

    // Command for filling a layer
    class FillLayerCommand : public ICommand
    {
    public:
        FillLayerCommand(Domain::TileGrid& layer, const Domain::TileData& fillTile)
            : m_Layer(layer)
            , m_FillTile(fillTile)
            , m_OldTiles(layer.GetRawData())
        {}

        void Execute() override
        {
            m_Layer.Fill(m_FillTile);
        }

        void Undo() override
        {
            m_Layer.SetRawData(m_OldTiles);
        }

        std::string GetDescription() const override
        {
            return "Fill Layer";
        }

    private:
        Domain::TileGrid& m_Layer;
        Domain::TileData m_FillTile;
        std::vector<Domain::TileData> m_OldTiles;
    };

    // Command for clearing a layer
    class ClearLayerCommand : public ICommand
    {
    public:
        explicit ClearLayerCommand(Domain::TileGrid& layer)
            : m_Layer(layer)
            , m_OldTiles(layer.GetRawData())
        {}

        void Execute() override
        {
            m_Layer.Clear();
        }

        void Undo() override
        {
            m_Layer.SetRawData(m_OldTiles);
        }

        std::string GetDescription() const override
        {
            return "Clear Layer";
        }

    private:
        Domain::TileGrid& m_Layer;
        std::vector<Domain::TileData> m_OldTiles;
    };
}
