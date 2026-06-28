#include "Tile.h"

#include "Constants.h"  

#include <glm/gtc/epsilon.hpp>

namespace Tiles
{
	void Tile::Reset()
	{
        m_IsPainted = false; 

		m_IsTexture = false;
		m_AtlasIndex = INVALID_ATLAS_INDEX;

		m_Rotation = { 0.0f, 0.0f, 0.0f };
		m_Size = { 1.0f, 1.0f };
		TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_TextureCoords = { 0, 0, 1, 1 };
	}

	bool Tile::operator==(const Tile& other) const
	{
        return m_IsPainted == other.m_IsPainted
            && m_IsTexture == other.m_IsTexture
            && m_AtlasIndex == other.m_AtlasIndex
            && glm::all(glm::epsilonEqual(m_Rotation, other.m_Rotation, 1e-6f))
            && glm::all(glm::epsilonEqual(m_Size, other.m_Size, 1e-6f))
            && glm::all(glm::epsilonEqual(TintColor, other.TintColor, 1e-6f))
            && glm::all(glm::epsilonEqual(m_TextureCoords, other.m_TextureCoords, 1e-6f));
	}

	bool Tile::operator!=(const Tile& other) const
	{
		return !(*this == other);
	}

    nlohmann::json Tile::ToJSON() const
    {
        nlohmann::json jsonTile;
        jsonTile[JSON::Tile::Painted] = IsPainted();
        jsonTile[JSON::Tile::Textured] = IsTextured();   
        jsonTile[JSON::Tile::AtlasIndex] = GetAtlasIndex();

        const glm::vec3& rotation = GetRotation();
        jsonTile[JSON::Tile::Rotation] = { rotation.x, rotation.y, rotation.z };

        const glm::vec2& size = GetSize();
        jsonTile[JSON::Tile::Size] = { size.x, size.y };

        const glm::vec4& textureCoords = GetTextureCoords();
        jsonTile[JSON::Tile::TextureCoords] = { textureCoords.x, textureCoords.y, textureCoords.z, textureCoords.w };

        const glm::vec4& tint = GetTint();
        jsonTile[JSON::Tile::TintColor] = { tint.r, tint.g, tint.b, tint.a };

        return jsonTile;
    }

    Tile Tile::FromJSON(const nlohmann::json& jsonTile)
    {
        Tile tile;

        if (jsonTile.contains(JSON::Tile::Textured))
        {
            bool isTextured = jsonTile[JSON::Tile::Textured].get<bool>();
            tile.SetTextured(isTextured);
        }

        if (jsonTile.contains(JSON::Tile::Painted))
        {
            bool isPainted = jsonTile[JSON::Tile::Painted].get<bool>();
            tile.SetPainted(isPainted);
        }

        if (jsonTile.contains(JSON::Tile::AtlasIndex))
        {
            size_t atlasIndex = jsonTile[JSON::Tile::AtlasIndex].get<size_t>();
            tile.SetAtlasIndex(atlasIndex);
        }

        if (jsonTile.contains(JSON::Tile::Rotation))
        {
            const auto& rotationArray = jsonTile[JSON::Tile::Rotation];
            if (rotationArray.size() >= 3)
            {
                glm::vec3 rotation(rotationArray[0], rotationArray[1], rotationArray[2]);
                tile.SetRotation(rotation);
            }
        }

        if (jsonTile.contains(JSON::Tile::Size))
        {
            const auto& sizeArray = jsonTile[JSON::Tile::Size];
            if (sizeArray.size() >= 2)
            {
                glm::vec2 size(sizeArray[0], sizeArray[1]);
                tile.SetSize(size);
            }
        }

        if (jsonTile.contains(JSON::Tile::TextureCoords))
        {
            const auto& textureCoordsArray = jsonTile[JSON::Tile::TextureCoords];
            if (textureCoordsArray.size() >= 4)
            {
                glm::vec4 textureCoords(textureCoordsArray[0], textureCoordsArray[1], textureCoordsArray[2], textureCoordsArray[3]);
                tile.SetTextureCoords(textureCoords);
            }
        }

        if (jsonTile.contains(JSON::Tile::TintColor))
        {
            const auto& tintArray = jsonTile[JSON::Tile::TintColor];
            if (tintArray.size() >= 4)
            {
                glm::vec4 tint(tintArray[0], tintArray[1], tintArray[2], tintArray[3]);
                tile.SetTint(tint);
            }
        }

        return tile;
    }
}