#pragma once

#include "Lumina/Renderer/Cameras/OrthographicCamera.h"
#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Renderer/Renderer.h"

#include "Base.h"
#include "Layers.h"

namespace Tiles
{
	class TileRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Render(Lumina::Camera& camera, Shared<Layers>& layers, Shared<Lumina::TextureAtlas> atlas, glm::vec2& viewportSize, float& zoom);
		static void* GetImage();
	};
}