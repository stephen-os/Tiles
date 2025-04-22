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

		static void DrawGrid(const Layers& layers);
		static void DrawLayers(const Layers& layers, const Lumina::TextureAtlas& atlas);
		static void DrawLayer(const Layer& layer, const Lumina::TextureAtlas& atlas);
		static void DrawTile(const Tile& tile, const Lumina::TextureAtlas& atlas, size_t row, size_t col);

		static void ExportLayers(const Layers& layers, const Lumina::TextureAtlas& atlas, const ExportAttributes& exportAttributes);

		static void* GetImage();
		static Lumina::Camera& GetCamera();
	};
}