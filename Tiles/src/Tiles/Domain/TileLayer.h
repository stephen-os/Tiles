#pragma once

#include "Domain/Tile.h"

#include "json.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Tiles
{
	enum class RenderGroup : int32_t
	{
		Disabled = -1,      // Layer is not rendered
		Background = 0,     // Default background layer
		Midground = 1,      // Middle layer for objects
		Foreground = 2,     // Front layer for UI/overlays
		Debug = 99          // Debug/temporary rendering
	};

	// Display/enumeration helpers for RenderGroup, shared by the layer UI. Inline
	// (not static) so every translation unit sees one definition.
	namespace TileLayerUtils
	{
		// The display name of a render group.
		[[nodiscard]] inline const char* GetRenderGroupName(RenderGroup renderGroup)
		{
			switch (renderGroup)
			{
			case RenderGroup::Disabled:   return "Disabled";
			case RenderGroup::Background:  return "Background";
			case RenderGroup::Midground:   return "Midground";
			case RenderGroup::Foreground:  return "Foreground";
			case RenderGroup::Debug:       return "Debug";
			default:                       return "Unknown";
			}
		}

		// Every render group, in display order.
		[[nodiscard]] inline std::vector<RenderGroup> GetAllRenderGroups()
		{
			return {
				RenderGroup::Disabled,
				RenderGroup::Background,
				RenderGroup::Midground,
				RenderGroup::Foreground,
				RenderGroup::Debug
			};
		}

		// The display name of every render group.
		[[nodiscard]] inline std::vector<const char*> GetAllRenderGroupNames()
		{
			std::vector<const char*> names;
			for (RenderGroup group : GetAllRenderGroups())
				names.push_back(GetRenderGroupName(group));
			return names;
		}

		// The underlying int value of every render group.
		[[nodiscard]] inline std::vector<int32_t> GetAllRenderGroupValues()
		{
			std::vector<int32_t> values;
			for (RenderGroup group : GetAllRenderGroups())
				values.push_back(static_cast<int32_t>(group));
			return values;
		}

		// The number of render groups.
		[[nodiscard]] inline size_t GetRenderGroupCount()
		{
			return GetAllRenderGroups().size();
		}
	}

	// Hash for signed integer tile coordinates used as sparse-map keys.
	struct TileCoordHash
	{
		size_t operator()(const glm::ivec2& c) const noexcept
		{
			// Pack the two 32-bit coords into one 64-bit key, then hash.
			uint64_t key = (static_cast<uint64_t>(static_cast<uint32_t>(c.x)) << 32)
				| static_cast<uint32_t>(c.y);
			return std::hash<uint64_t>()(key);
		}
	};

	// A sparse grid of tiles with a name, visibility, and render group. Only
	// painted cells are stored, addressed by signed (x, y), so the board is
	// unbounded in every direction and empty cells cost nothing.
	class TileLayer
	{
	public:
		using TileMap = std::unordered_map<glm::ivec2, Tile, TileCoordHash>;

		TileLayer() = default;
		~TileLayer() = default;

		// Removes every painted tile.
		void Clear();

		// The tile at (x, y), or a shared empty (unpainted) tile if none exists.
		[[nodiscard]] const Tile& GetTile(int x, int y) const;
		[[nodiscard]] bool HasTile(int x, int y) const;

		// Paints (x, y). An unpainted tile erases the cell, keeping the map minimal.
		void SetTile(int x, int y, const Tile& tile);
		// Clears a single cell.
		void EraseTile(int x, int y);

		[[nodiscard]] const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name);

		[[nodiscard]] bool GetVisibility() const { return m_Visible; }
		void SetVisibility(bool visible) { m_Visible = visible; }

		[[nodiscard]] RenderGroup GetRenderGroup() const { return m_RenderGroup; }
		void SetRenderGroup(RenderGroup group) { m_RenderGroup = group; }
		void DisableRendering() { m_RenderGroup = RenderGroup::Disabled; }
		[[nodiscard]] bool IsRenderingEnabled() const { return m_RenderGroup != RenderGroup::Disabled; }

		[[nodiscard]] size_t GetTileCount() const { return m_Tiles.size(); }
		[[nodiscard]] bool IsEmpty() const { return m_Tiles.empty(); }

		// Iteration yields (const glm::ivec2 coord, Tile) pairs for painted cells only.
		[[nodiscard]] TileMap::iterator begin() { return m_Tiles.begin(); }
		[[nodiscard]] TileMap::iterator end() { return m_Tiles.end(); }
		[[nodiscard]] TileMap::const_iterator begin() const { return m_Tiles.begin(); }
		[[nodiscard]] TileMap::const_iterator end() const { return m_Tiles.end(); }

		// Inclusive bounding box of painted tiles as (minX, minY, maxX, maxY),
		// or nullopt when the layer is empty.
		[[nodiscard]] std::optional<glm::ivec4> GetBounds() const;

		[[nodiscard]] nlohmann::json ToJSON() const;
		[[nodiscard]] static TileLayer FromJSON(const nlohmann::json& jsonLayer);

	private:
		std::string m_Name = "New Layer";
		bool m_Visible = true;
		RenderGroup m_RenderGroup = RenderGroup::Background;
		TileMap m_Tiles;                                // sparse map of painted tiles by coordinate
	};
}
