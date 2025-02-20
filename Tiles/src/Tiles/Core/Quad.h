#pragma once

#include <Lumina/Renderer/VertexArray.h>
#include <Lumina/Renderer/ShaderProgram.h>

#include "Layers.h"
#include "Camera.h"
#include "Base.h"

namespace Tiles
{
	class Quad
	{
	public:
		Quad();
		~Quad() = default;

		void SetUniforms(const Camera& camera, const Shared<Layers>& layers) const; 

		void Bind() const;
		void Unbind() const;

		const Shared<Lumina::VertexArray>& GetArrays() { return m_VertexArray; }
		const Shared<Lumina::ShaderProgram>& GetShader() { return m_Shader; }

	private:
		Shared<Lumina::VertexArray> m_VertexArray;
		Shared<Lumina::ShaderProgram> m_Shader;
	};
}