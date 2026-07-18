#pragma once

namespace Tiles
{
	namespace JSON
	{
		namespace Tile
		{
			constexpr const char* Rotation = "tile_rotation";
			constexpr const char* TintColor = "tile_tint_color";
			constexpr const char* Size = "tile_size";
			constexpr const char* AtlasId = "tile_atlas_id";       // stable atlas reference
			constexpr const char* CellIndex = "tile_cell_index";   // cell within that atlas
			constexpr const char* Textured = "tile_is_textured";
			constexpr const char* Painted = "tile_is_painted";

			// Pre-AtlasId keys, read only when migrating a legacy manifest.
			constexpr const char* TextureCoords = "tile_texture_coords";
			constexpr const char* AtlasIndex = "tile_atlas_index";
		}

		namespace TileLayer
		{
			constexpr const char* Name = "tile_layer_name";
			constexpr const char* Width = "tile_layer_width";
			constexpr const char* Height = "tile_layer_height";
			constexpr const char* Visible = "tile_layer_visible";
			constexpr const char* RenderGroup = "tile_layer_render_group";
			constexpr const char* Tiles = "tile_layer_tiles";
			constexpr const char* TileX = "x";   // sparse tile-entry coordinate
			constexpr const char* TileY = "y";
		}

		namespace LayerStack
		{
			constexpr const char* Width = "layer_stack_width";
			constexpr const char* Height = "layer_stack_height";
			constexpr const char* TileLayers = "layer_stack_layers";
		}

		namespace Project
		{
			constexpr const char* Name = "project_name";
			constexpr const char* LayerStack = "project_layer_stack";
			constexpr const char* Version = "manifest_version";   // schema version; absent => legacy
			constexpr const char* NextAtlasId = "next_atlas_id";  // atlas-id high-water mark
		}

		namespace Atlas
		{
			constexpr const char* Array = "atlas_array";
			constexpr const char* Path = "atlas_path";       // legacy: external image path
			constexpr const char* Image = "atlas_image";     // v2: embedded image entry name
			constexpr const char* Width = "atlas_width";
			constexpr const char* Height = "atlas_height";
			constexpr const char* Id = "atlas_id";           // stable per-project id
		}

		namespace Region
		{
			constexpr const char* Object = "export_region";
			constexpr const char* X = "x";
			constexpr const char* Y = "y";
			constexpr const char* Width = "width";
			constexpr const char* Height = "height";
			constexpr const char* Enabled = "enabled";
		}
	}
}
