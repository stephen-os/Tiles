#include "Tile.h"

#include <glm/gtc/epsilon.hpp>

namespace Tiles
{
	namespace TileFields
	{
		static constexpr const char* IS_TEXTURED = "is_textured";
		static constexpr const char* ATLAS_INDEX = "atlas_index";
		static constexpr const char* ROTATION = "rotation";
		static constexpr const char* SIZE = "size";
		static constexpr const char* TEXTURE_COORDS = "texture_coords";
	}

	static constexpr float EPSILON = 1e-6f;

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
            && glm::all(glm::epsilonEqual(m_Rotation, other.m_Rotation, EPSILON))
            && glm::all(glm::epsilonEqual(m_Size, other.m_Size, EPSILON))
            && glm::all(glm::epsilonEqual(TintColor, other.TintColor, EPSILON))
            && glm::all(glm::epsilonEqual(m_TextureCoords, other.m_TextureCoords, EPSILON));
	}

	bool Tile::operator!=(const Tile& other) const
	{
		return !(*this == other);
	}

    nlohmann::json Tile::ToJSON() const
    {
        nlohmann::json jsonTile;
        jsonTile[TileFields::IS_TEXTURED] = IsTextured();
        
        if (IsTextured())
        {
            if (HasValidAtlas())
            {
                jsonTile[TileFields::ATLAS_INDEX] = GetAtlasIndex();
            }
            else
            {
                jsonTile[TileFields::ATLAS_INDEX] = nullptr;
            }

            const glm::vec3& rotation = GetRotation();
            jsonTile[TileFields::ROTATION] = { rotation.x, rotation.y, rotation.z };
            
            const glm::vec2& size = GetSize();
            jsonTile[TileFields::SIZE] = { size.x, size.y };
            
            const glm::vec4& textureCoords = GetTextureCoords();
            jsonTile[TileFields::TEXTURE_COORDS] = { textureCoords.x, textureCoords.y, textureCoords.z, textureCoords.w };
        }
        return jsonTile;
    }

    Tile Tile::FromJSON(const nlohmann::json& jsonTile)
    {
        Tile tile;
        bool isTextured = jsonTile.at(TileFields::IS_TEXTURED).get<bool>();
        tile.SetTextured(isTextured);
        
        if (isTextured)
        {
            if (jsonTile.contains(TileFields::ATLAS_INDEX) && !jsonTile[TileFields::ATLAS_INDEX].is_null())
            {
                size_t atlasIndex = jsonTile[TileFields::ATLAS_INDEX].get<size_t>();
                tile.SetAtlasIndex(atlasIndex);
            }
            else
            {
                tile.SetAtlasIndex(INVALID_ATLAS_INDEX);
            }

            if (jsonTile.contains(TileFields::ROTATION))
            {
                const auto& rotationArray = jsonTile[TileFields::ROTATION];
                if (rotationArray.size() >= 3)
                {
                    glm::vec3 rotation(rotationArray[0], rotationArray[1], rotationArray[2]);
                    tile.SetRotation(rotation);
                }
            }

            if (jsonTile.contains(TileFields::SIZE))
            {
                const auto& sizeArray = jsonTile[TileFields::SIZE];
                if (sizeArray.size() >= 2)
                {
                    glm::vec2 size(sizeArray[0], sizeArray[1]);
                    tile.SetSize(size);
                }
            }

            if (jsonTile.contains(TileFields::TEXTURE_COORDS))
            {
                const auto& textureCoordsArray = jsonTile[TileFields::TEXTURE_COORDS];
                if (textureCoordsArray.size() >= 4)
                {
                    glm::vec4 textureCoords(textureCoordsArray[0], textureCoordsArray[1], textureCoordsArray[2], textureCoordsArray[3]);
                    tile.SetTextureCoords(textureCoords);
                }
            }
        }

        return tile;
    }
}