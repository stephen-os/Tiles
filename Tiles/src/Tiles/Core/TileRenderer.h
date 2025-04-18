#pragma once

#include "Lumina/Renderer/Cameras/OrthographicCamera.h"
#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Renderer/Renderer.h"

#include "Lumina/Core/Aliases.h"

#include <vector>
#include <string>

#include "Layers.h"
#include "Layer.h"

namespace Tiles
{
	struct ExportAttributes
	{
		std::string Filename;
		std::string Filepath;
		size_t Resolution = 200;
	};

	class TileRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void SetResolution(glm::vec2 resolution);
		
		static void Zoom(float delta); 
		static float GetZoom(); 

		static void DrawGrid(Shared<Layers>& layers);
		static void DrawLayers(Shared<Layers>& layers, Shared<Lumina::TextureAtlas> atlas);
		static void DrawLayer(Layer& layer, Shared<Lumina::TextureAtlas> atlas);
		static void DrawTile(Tile& tile, Shared<Lumina::TextureAtlas> atlas, glm::vec2 position);

		static void ExportLayers(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, ExportAttributes& exportAttributes);

		static void* GetImage();
		static Lumina::Camera& GetCamera();
	};
}