#pragma once

#include "AtlasId.h"

#include <glm/glm.hpp>

#include "json.hpp"

#include <cstdint>

namespace Tiles
{
	// A single grid cell: paint/texture flags plus transform, tint, and the
	// atlas cell it samples. The atlas is referenced by stable id (not position),
	// so the reference survives atlas add/remove/reorder; the UV rectangle is
	// resolved from the atlas grid at draw time. Default-constructs to an empty,
	// unpainted tile.
	class Tile
	{
	public:
		Tile() = default;
		~Tile() = default;

		// Restores the tile to its default empty state.
		void Reset();

		[[nodiscard]] bool IsPainted() const { return m_IsPainted; }
		void SetPainted(bool painted) { m_IsPainted = painted; }

		[[nodiscard]] bool IsTextured() const { return m_IsTexture; }
		void SetTextured(bool textured) { m_IsTexture = textured; }

		// The stable id of the atlas this tile samples (Invalid when untextured).
		[[nodiscard]] AtlasId GetAtlasId() const { return m_AtlasId; }
		void SetAtlasId(AtlasId id) { m_AtlasId = id; }
		[[nodiscard]] bool HasValidAtlas() const { return m_AtlasId != AtlasId::Invalid; }

		// The cell within that atlas's grid; its UV rect is resolved on demand.
		[[nodiscard]] int GetCellIndex() const { return m_CellIndex; }
		void SetCellIndex(int index) { m_CellIndex = index; }

		[[nodiscard]] glm::vec3& GetRotation() { return m_Rotation; }
		[[nodiscard]] glm::vec2& GetSize() { return m_Size; }
		[[nodiscard]] glm::vec4& GetTint() { return m_TintColor; }

		[[nodiscard]] const glm::vec3& GetRotation() const { return m_Rotation; }
		[[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }
		[[nodiscard]] const glm::vec4& GetTint() const { return m_TintColor; }

		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
		void SetSize(const glm::vec2& size) { m_Size = size; }
		void SetTint(const glm::vec4& tint) { m_TintColor = tint; }

		// Value equality; float fields are compared with an epsilon tolerance.
		[[nodiscard]] bool operator==(const Tile& other) const;
		[[nodiscard]] bool operator!=(const Tile& other) const;

		[[nodiscard]] nlohmann::json ToJSON() const;
		[[nodiscard]] static Tile FromJSON(const nlohmann::json& j);

	private:
		bool m_IsPainted = false;
		bool m_IsTexture = false;

		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 1.0f, 1.0f };
		glm::vec4 m_TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		AtlasId m_AtlasId = AtlasId::Invalid;
		int m_CellIndex = 0;
	};
}
