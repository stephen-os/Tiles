#pragma once

#include "../Renderer2D.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles
{
	struct PointLight;
	struct RendererState;

	// Accumulates point lights into a uniform buffer consumed by the lit shaders.
	// Unlike the vertex batchers this never issues its own draw call; it only
	// uploads and binds the UBO around the other primitives' draws.
	class PointLightBuffer
	{
	public:
		void Init();
		void Shutdown();

		void Append(RendererState& state);
		void Reset();
		void Upload(RendererState& state);
		void Bind() const;
		void Unbind() const;

		std::shared_ptr<UniformBuffer> Buffer;

		uint32_t Count = 0;
		PointLight* Base = nullptr;
		PointLight* Ptr = nullptr;

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 1.0f;
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Radius = 5.0f;
		BlendMode Blend = BlendMode::Additive;
		float BlendAlpha = 1.0f;
		AttenuationModel FalloffType = AttenuationModel::Quadratic;
		float Falloff = 1.0f;
	};
}
