#include "Tile.h"

#include <sstream>

#include <glm/gtc/epsilon.hpp>

#include "Lumina/Core/Log.h"

namespace Tiles
{
	void Tile::Reset()
	{
		m_TextureIndex = -1;
		m_TintColor = glm::vec4(1.0f);
		m_Rotation = 0.0f; 
	}

	std::string Tile::ToString() const
	{
		std::ostringstream oss;
		oss << "Tile(TextureIndex=" << m_TextureIndex
			<< ", TintColor=("
			<< m_TintColor.r << ", "
			<< m_TintColor.g << ", "
			<< m_TintColor.b << ", "
			<< m_TintColor.a << ")"
			<< ", Rotation=" << m_Rotation
			<< ")";
		return oss.str();
	}

	bool Tile::operator==(const Tile& other) const
	{
		const float epsilon = 0.0001f;

		return m_TextureIndex == other.m_TextureIndex &&
			glm::all(glm::epsilonEqual(m_TintColor, other.m_TintColor, epsilon)) &&
			glm::epsilonEqual(m_Rotation, other.m_Rotation, epsilon);
	}

	bool Tile::operator!=(const Tile& other) const
	{
		return !(*this == other);
	}

	Tile& Tile::operator=(const Tile& other)
	{
		if (this != &other)
		{
			m_TextureIndex = other.m_TextureIndex;
			m_TintColor = other.m_TintColor;
			m_Rotation = other.m_Rotation;
		}
		return *this;
	}
}