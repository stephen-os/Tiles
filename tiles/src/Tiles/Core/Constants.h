#pragma once
#include "imgui.h"
#include <glm/glm.hpp>

namespace Tiles
{
    namespace UI
    {
        namespace Color
        {
            // Component colors
            constexpr ImVec4 Red = { 0.8f, 0.2f, 0.2f, 1.0f };
            constexpr ImVec4 Green = { 0.2f, 0.8f, 0.2f, 1.0f };
            constexpr ImVec4 Blue = { 0.2f, 0.2f, 0.8f, 1.0f };
            constexpr ImVec4 Gray = { 0.4f, 0.4f, 0.4f, 1.0f };

            // Text colors
            constexpr ImVec4 Text = { 1.0f, 1.0f, 1.0f, 1.0f };
            constexpr ImVec4 TextHint = { 0.7f, 0.7f, 0.7f, 1.0f };
            constexpr ImVec4 TextError = { 1.0f, 0.3f, 0.3f, 1.0f };

            // Background colors
            constexpr ImVec4 BackgroundDark = { 0.15f, 0.15f, 0.15f, 1.0f };
            constexpr ImVec4 BackgroundMedium = { 0.2f, 0.2f, 0.2f, 1.0f };

            // Button colors
            constexpr ImVec4 Button = { 0.2f, 0.4f, 0.6f, 1.0f };
            constexpr ImVec4 ButtonHover = { 0.3f, 0.5f, 0.7f, 1.0f };
            constexpr ImVec4 ButtonActive = { 0.1f, 0.3f, 0.5f, 1.0f };

            // Danger/Reset button colors
            constexpr ImVec4 Danger = { 0.8f, 0.3f, 0.3f, 1.0f };
            constexpr ImVec4 DangerHover = { 0.9f, 0.4f, 0.4f, 1.0f };
            constexpr ImVec4 DangerActive = { 0.7f, 0.2f, 0.2f, 1.0f };

            // Interactive element states
            constexpr ImVec4 Normal = { 0.15f, 0.15f, 0.15f, 1.0f };
            constexpr ImVec4 Hover = { 0.2f, 0.2f, 0.2f, 1.0f };
            constexpr ImVec4 Active = { 0.25f, 0.25f, 0.25f, 1.0f };
        }

        namespace Component
        {
            constexpr float ButtonWidth = 25.0f;
            constexpr float ButtonHeight = 25.0f;
            constexpr float SpaceBetween = 2.0f;
            constexpr float FramePadding = 4.0f;
            constexpr float MinSliderWidth = 100.0f;
            constexpr float SliderWidthOffset = 60.0f;
            constexpr float ControlButtonWidth = 80.0f;
            constexpr float ChildRounding = 5.0f;
            constexpr float InputWidth = 100.0f;
        }

        namespace Tool
        {
            constexpr float ButtonSize = 32.0f;
            constexpr float ButtonSpacing = 4.0f;
            constexpr float CursorSize = 24.0f;
        }

        namespace Dialog
        {
            constexpr float FileDialogWidth = 600.0f;
            constexpr float FileDialogHeight = 400.0f;
        }

        namespace Selection
        {
            constexpr ImVec4 BorderColor = { 1.0f, 0.647f, 0.0f, 1.0f };
            constexpr ImVec4 DefaultBorderColor = { 0.2f, 0.2f, 0.2f, 1.0f };
            constexpr ImVec4 TransparentButton = { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        namespace List
        {
            constexpr float LayerListHeight = 300.0f;
        }

        namespace Layer
        {
            constexpr ImVec4 ItemBackground = { 0.15f, 0.15f, 0.15f, 1.0f };
            constexpr ImVec4 ItemHover = { 0.25f, 0.25f, 0.25f, 1.0f };
            constexpr ImVec4 OrangeAccent = { 1.0f, 0.5f, 0.0f, 1.0f };
        }

        namespace Window
        {
            constexpr float MinimumSize = 64.0f;
        }
    }

    namespace AssetPath
    {
        constexpr const char* Checkerboard = "res/assets/checkerboard.png";
        constexpr const char* Brush = "res/assets/brush.png";
        constexpr const char* Eraser = "res/assets/eraser.png";
        constexpr const char* Fill = "res/assets/bucket.png";
    }

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

    namespace Texture
    {
        namespace Tile
        {
            constexpr float MinSize = 16.0f;
            constexpr float MaxSize = 128.0f;
            constexpr float PreferredSize = 40.0f;
            constexpr float BorderThicknessRatio = 0.0625f;
            constexpr float MinBorderThickness = 1.0f;
        }

        namespace Atlas
        {
            constexpr int SmallThreshold = 4;
            constexpr int MediumThreshold = 16;
            constexpr int DefaultWidth = 16;
            constexpr int DefaultHeight = 16;
        }

        
        namespace FileDialog
        {
            constexpr const char* DialogKey = "ChooseAtlasFileDlg";
            constexpr const char* DialogTitle = "Choose Atlas File";
            constexpr const char* FileFilters = ".png,.jpg,.jpeg";
            constexpr const char* DefaultPath = ".";
        }
    }

    namespace Render2D
    {
        namespace Depth
        {
            constexpr float Background = -0.9f;
            constexpr float Grid = -0.8f;
            constexpr float Brush = 0.0f;
            constexpr float Bounds = 0.1f;
        }

        namespace Grid
        {
            constexpr float Spacing = 0.2f;
            constexpr float LineThickness = 1.0f;
            constexpr float BackgroundSize = 4.0f;
            constexpr glm::vec4 Color = { 0.4f, 0.4f, 0.4f, 0.5f };
        }

        namespace Bounds
        {
            constexpr float Offset = 0.05f;
            constexpr glm::vec4 Color = { 1.0f, 0.0f, 0.0f, 0.3f };
        }
    }

    namespace Camera
    {
        namespace Settings
        {
            constexpr float DefaultBounds = 2.0f;
            constexpr float MinZoom = 1.0f;
            constexpr float MaxZoom = 5.0f;
            constexpr float ZoomSensitivity = 0.1f;
            constexpr float PanSensitivity = 1.0f;
        }
    }

    namespace Brush
    {
        namespace Rotation
        {
            constexpr float Min = -180.0f;
            constexpr float Max = 180.0f;
            constexpr float Zero = 0.0f;
            constexpr float Quarter = 90.0f;
            constexpr float Half = 180.0f;
            constexpr float ThreeQuarter = 270.0f;
        }

        namespace Size
        {
            constexpr float Min = 0.1f;
            constexpr float Max = 5.0f;
            constexpr float Half = 0.5f;
            constexpr float Normal = 1.0f;
            constexpr float Double = 2.0f;
            constexpr float Triple = 3.0f;
        }

        namespace Tint
        {
            constexpr glm::vec4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
            constexpr glm::vec4 Red = { 1.0f, 0.3f, 0.3f, 1.0f };
            constexpr glm::vec4 Green = { 0.3f, 1.0f, 0.3f, 1.0f };
            constexpr glm::vec4 Blue = { 0.3f, 0.3f, 1.0f, 1.0f };
        }
    }

    namespace JSON
    {
        namespace Tile
        {
			constexpr const char* Rotation = "rotation";
			constexpr const char* TintColor = "tint_color";
			constexpr const char* Size = "size";
			constexpr const char* TextureCoords = "texture_coords";
			constexpr const char* AtlasIndex = "atlas_index";
			constexpr const char* Textured = "is_textured";
			constexpr const char* Painted = "is_painted";
        }

        namespace project
        {

        }
        
        namespace Layer
        {

        }
    }
}