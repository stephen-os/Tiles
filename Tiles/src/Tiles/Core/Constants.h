#pragma once
#include <glm/glm.hpp>
#include <cstddef>
#include <cstdint>

namespace Tiles
{
    namespace Viewport
    {
        namespace Render
        {
            constexpr float DefaultTileSize = 64.0f;
            constexpr float MinZoom = 0.5f;
            constexpr float MaxZoom = 3.0f;
            constexpr float ZoomSensitivity = 0.05f;
            constexpr float PanSensitivity = 20.0f;
        }

        namespace Depth
        {
            constexpr float Grid = -1.0f;
            constexpr float Outline = -0.5f;
            constexpr float Tile = 0.0f;
            constexpr float Overlay = 0.1f;
            constexpr float HoverTile = 0.2f;
        }

        namespace Input
        {
            constexpr float CameraMoveSpeed = 5.0f;
            constexpr float MousePanSensitivity = 0.002f;
        }

        namespace Grid
        {
            constexpr float HoverOutlineThickness = 2.0f;
            constexpr glm::vec4 GridColor = { 0.5f, 0.5f, 0.5f, 0.8f };
            constexpr glm::vec4 CheckerColor1 = { 0.27f, 0.27f, 0.27f, 1.0f };
            constexpr glm::vec4 CheckerColor2 = { 0.15f, 0.15f, 0.15f, 1.0f };
            constexpr glm::vec4 BoundaryColor = { 1.0f, 0.0f, 0.0f, 1.0f };
            constexpr glm::vec4 HoverColor = { 0.0f, 1.0f, 0.0f, 0.6f };
        }
    }

    namespace Grid
    {
        // Upper bound on a single grid dimension. Guards against corrupt or
        // malicious project files whose width/height would overflow the
        // 32-bit tile-buffer size computation and trigger an out-of-bounds
        // write. Clamped dimensions (<= MaxDimension each) multiply to at
        // most MaxDimension^2, which fits comfortably in size_t.
        constexpr uint32_t MaxDimension = 10000;

        constexpr uint32_t ClampDimension(uint32_t dimension)
        {
            return dimension < MaxDimension ? dimension : MaxDimension;
        }

        // Tile count of a grid whose dimensions are already clamped. Computed
        // in 64-bit space so the multiply can never wrap.
        constexpr size_t TileCount(uint32_t width, uint32_t height)
        {
            return static_cast<size_t>(width) * static_cast<size_t>(height);
        }
    }

    namespace JSON
    {
        namespace Tile
        {
			constexpr const char* Rotation = "tile_rotation";
			constexpr const char* TintColor = "tile_tint_color";
			constexpr const char* Size = "tile_size";
			constexpr const char* TextureCoords = "tile_texture_coords";
			constexpr const char* AtlasIndex = "tile_atlas_index";
			constexpr const char* Textured = "tile_is_textured";
			constexpr const char* Painted = "tile_is_painted";
        }

        namespace TileLayer
        {
            static constexpr const char* Name = "tile_layer_name";
            static constexpr const char* Width = "tile_layer_width";
            static constexpr const char* Height = "tile_layer_height";
            static constexpr const char* Visible = "tile_layer_visible";
            static constexpr const char* RenderGroup = "tile_layer_render_group";
            static constexpr const char* Tiles = "tile_layer_tiles";
            static constexpr const char* TileX = "x";   // sparse tile-entry coordinate
            static constexpr const char* TileY = "y";
        }

        namespace LayerStack
        {
            static constexpr const char* Width = "layer_stack_width";
            static constexpr const char* Height = "layer_stack_height";
            static constexpr const char* TileLayers = "layer_stack_layers";
		}

        namespace Project
        {
            static constexpr const char* Name = "project_name";
			static constexpr const char* LayerStack = "project_layer_stack";
        }

        namespace Atlas
        {
            static constexpr const char* Array = "atlas_array";
			static constexpr const char* Path = "atlas_path";       // legacy: external image path
			static constexpr const char* Image = "atlas_image";     // v2: embedded image entry name
			static constexpr const char* Width = "atlas_width";
			static constexpr const char* Height = "atlas_height";
        }

        namespace Region
        {
            static constexpr const char* Object = "export_region";
            static constexpr const char* X = "x";
            static constexpr const char* Y = "y";
            static constexpr const char* Width = "width";
            static constexpr const char* Height = "height";
            static constexpr const char* Enabled = "enabled";
        }
    }

    namespace File
    {
        constexpr const char* ProjectExtension = ".tiles";
        constexpr const char* ProjectFilesFilter = "*.tiles";
        constexpr const char* AllFilesFilter = "*.*";
		constexpr const char* TextureFilesFilter = "*.png;*.jpg;*.jpeg";
    }
}