#include "Domain/Tile.h"

#include "Domain/Constants.h"

#include <glm/gtc/epsilon.hpp>

namespace Tiles
{
	// Restores every field to the default empty-tile state.
	void Tile::Reset()
	{
		m_IsPainted = false;
		m_IsTexture = false;
		m_AtlasId = AtlasId::Invalid;
		m_CellIndex = 0;

		m_Rotation = { 0.0f, 0.0f, 0.0f };
		m_Size = { 1.0f, 1.0f };
		m_TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// Value equality with an epsilon tolerance on the float fields.
	bool Tile::operator==(const Tile& other) const
	{
		return m_IsPainted == other.m_IsPainted
			&& m_IsTexture == other.m_IsTexture
			&& m_AtlasId == other.m_AtlasId
			&& m_CellIndex == other.m_CellIndex
			&& glm::all(glm::epsilonEqual(m_Rotation, other.m_Rotation, 1e-6f))
			&& glm::all(glm::epsilonEqual(m_Size, other.m_Size, 1e-6f))
			&& glm::all(glm::epsilonEqual(m_TintColor, other.m_TintColor, 1e-6f));
	}

	// Negation of operator==.
	bool Tile::operator!=(const Tile& other) const
	{
		return !(*this == other);
	}

	// Serializes the tile's flags, transform, tint, and atlas cell reference.
	nlohmann::json Tile::ToJSON() const
	{
		nlohmann::json jsonTile;
		jsonTile[JSON::Tile::Painted] = IsPainted();
		jsonTile[JSON::Tile::Textured] = IsTextured();
		jsonTile[JSON::Tile::AtlasId] = static_cast<uint32_t>(GetAtlasId());
		jsonTile[JSON::Tile::CellIndex] = GetCellIndex();

		const glm::vec3& rotation = GetRotation();
		jsonTile[JSON::Tile::Rotation] = { rotation.x, rotation.y, rotation.z };

		const glm::vec2& size = GetSize();
		jsonTile[JSON::Tile::Size] = { size.x, size.y };

		const glm::vec4& tint = GetTint();
		jsonTile[JSON::Tile::TintColor] = { tint.r, tint.g, tint.b, tint.a };

		return jsonTile;
	}

	// Reconstructs a tile from JSON, keeping the default for any missing field.
	Tile Tile::FromJSON(const nlohmann::json& jsonTile)
	{
		Tile tile;

		if (jsonTile.contains(JSON::Tile::Textured))
			tile.SetTextured(jsonTile[JSON::Tile::Textured].get<bool>());

		if (jsonTile.contains(JSON::Tile::Painted))
			tile.SetPainted(jsonTile[JSON::Tile::Painted].get<bool>());

		if (jsonTile.contains(JSON::Tile::AtlasId))
			tile.SetAtlasId(static_cast<AtlasId>(jsonTile[JSON::Tile::AtlasId].get<uint32_t>()));

		if (jsonTile.contains(JSON::Tile::CellIndex))
			tile.SetCellIndex(jsonTile[JSON::Tile::CellIndex].get<int>());

		if (jsonTile.contains(JSON::Tile::Rotation))
		{
			const auto& rotationArray = jsonTile[JSON::Tile::Rotation];
			if (rotationArray.size() >= 3)
				tile.SetRotation(glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]));
		}

		if (jsonTile.contains(JSON::Tile::Size))
		{
			const auto& sizeArray = jsonTile[JSON::Tile::Size];
			if (sizeArray.size() >= 2)
				tile.SetSize(glm::vec2(sizeArray[0], sizeArray[1]));
		}

		if (jsonTile.contains(JSON::Tile::TintColor))
		{
			const auto& tintArray = jsonTile[JSON::Tile::TintColor];
			if (tintArray.size() >= 4)
				tile.SetTint(glm::vec4(tintArray[0], tintArray[1], tintArray[2], tintArray[3]));
		}

		return tile;
	}
}
