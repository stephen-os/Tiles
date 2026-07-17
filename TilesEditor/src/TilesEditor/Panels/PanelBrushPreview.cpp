#include "PanelBrushPreview.h"

#include "../UIConstants.h"
#include "../UI/Widgets.h"

#include "Core/Logger.h"

#include <glm/gtc/type_ptr.hpp>

namespace Tiles::Editor
{
    PanelBrushPreview::PanelBrushPreview(EditorHost& host) : Panel(host)
    {
        m_PreviewRenderTarget = Tiles::RenderTarget::Create(512, 512);
    }

    void PanelBrushPreview::Render()
    {
        ImGui::Begin("Brush Preview", OpenFlag(), ImGuiWindowFlags_MenuBar);

        // Menu bar controls
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("View Settings"))
            {
                ImGui::Checkbox("Show Grid", &m_ShowGrid);
                ImGui::Checkbox("Show Bounds", &m_ShowBounds);
                ImGui::Separator();
                ImGui::SliderFloat("Background", &m_BackgroundBrightness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Zoom", &m_ZoomLevel, CameraConstants::Settings::MinZoom, CameraConstants::Settings::MaxZoom, "%.1fx");

                if (ImGui::Button("Reset View"))
                {
                    m_ShowGrid = true;
                    m_ShowBounds = false;
                    m_BackgroundBrightness = 0.2f;
                    m_ZoomLevel = 2.0f;
                    m_PanOffset = { 0.0f, 0.0f };
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        RenderBlockViewport();
        ImGui::Separator();
        RenderBlockBrushInfo();

        ImGui::End();
    }

    void PanelBrushPreview::Update()
    {
    }

    void PanelBrushPreview::RenderBlockViewport()
    {
        auto windowSize = ImGui::GetContentRegionAvail();
        windowSize.x = std::max(windowSize.x, UI::Window::MinimumSize);
        windowSize.y = std::max(windowSize.y, UI::Window::MinimumSize);

        float previewSize = std::min(windowSize.x, windowSize.y);
        ImVec2 previewDimensions(previewSize, previewSize);

        // Build the view from pan and zoom: a fixed world region of half-extent
        // DefaultBounds/zoom, centred on the pan offset.
        float bounds = CameraConstants::Settings::DefaultBounds / m_ZoomLevel;
        Tiles::Camera2D camera;
        camera.Center = m_PanOffset;
        camera.Zoom = previewDimensions.x / (2.0f * bounds);

        uint32_t resolutionX = static_cast<uint32_t>(previewDimensions.x);
        uint32_t resolutionY = static_cast<uint32_t>(previewDimensions.y);

        Tiles::Renderer2D::SetRenderTarget(m_PreviewRenderTarget);
        Tiles::Renderer2D::SetResolution(resolutionX, resolutionY);
        Tiles::Renderer2D::BeginFrame(camera.ViewProjection({ previewDimensions.x, previewDimensions.y }));

        // Render background
        Tiles::Renderer2D::DrawSquare({
            .Position = { 0.0f, 0.0f, Render2D::Depth::Background },
            .Size = { Render2D::Grid::BackgroundSize, Render2D::Grid::BackgroundSize },
            .Tint = { m_BackgroundBrightness, m_BackgroundBrightness, m_BackgroundBrightness, 1.0f },
        });

        // Render grid if enabled
        if (m_ShowGrid)
        {
            Tiles::Line line;
            line.Color = Render2D::Grid::Color;
            line.Thickness = Render2D::Grid::LineThickness;

            // Vertical lines
            for (float x = -CameraConstants::Settings::DefaultBounds; x <= CameraConstants::Settings::DefaultBounds; x += Render2D::Grid::Spacing)
            {
                line.Start = { x, -CameraConstants::Settings::DefaultBounds, Render2D::Depth::Grid };
                line.End = { x, CameraConstants::Settings::DefaultBounds, Render2D::Depth::Grid };
                Tiles::Renderer2D::DrawLine(line);
            }

            // Horizontal lines
            for (float y = -CameraConstants::Settings::DefaultBounds; y <= CameraConstants::Settings::DefaultBounds; y += Render2D::Grid::Spacing)
            {
                line.Start = { -CameraConstants::Settings::DefaultBounds, y, Render2D::Depth::Grid };
                line.End = { CameraConstants::Settings::DefaultBounds, y, Render2D::Depth::Grid };
                Tiles::Renderer2D::DrawLine(line);
            }
        }

        // Render brush
        auto& brush = Ctx().GetBrush();

        Tiles::Square brushQuad;
        brushQuad.Position = { 0.0f, 0.0f, Render2D::Depth::Brush };
        brushQuad.Size = brush.GetSize();
        brushQuad.Rotation = brush.GetRotation();
        brushQuad.Tint = brush.GetTint();
        brushQuad.TexCoords = brush.GetTextureCoords();

        if (brush.IsTextured() && brush.HasValidAtlas())
        {
            auto atlas = Ctx().GetProject()->GetTextureAtlas(brush.GetAtlasIndex());
            brushQuad.Texture = Host().GetAtlasTexture(*atlas);
        }

        Tiles::Renderer2D::DrawSquare(brushQuad);

        // Render bounds if enabled
        if (m_ShowBounds)
        {
            Tiles::Renderer2D::DrawSquare({
                .Position = { 0.0f, 0.0f, Render2D::Depth::Bounds },
                .Size = { brush.GetSize().x + Render2D::Bounds::Offset, brush.GetSize().y + Render2D::Bounds::Offset },
                .Tint = Render2D::Bounds::Color,
            });
        }

        Tiles::Renderer2D::EndFrame();
        Tiles::Renderer2D::SetRenderTarget(nullptr);


        // An InvisibleButton reserves the canvas rect and captures hover/drag for
        // pan and zoom; the rendered texture is drawn over it afterward.
        ImGui::InvisibleButton("PreviewCanvas", previewDimensions);
        bool isCanvasHovered = ImGui::IsItemHovered();
        bool isCanvasActive = ImGui::IsItemActive();

        if (isCanvasActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            float panScaleX = (delta.x / previewDimensions.x) * (Render2D::Grid::BackgroundSize / m_ZoomLevel) * CameraConstants::Settings::PanSensitivity;
            float panScaleY = (delta.y / previewDimensions.y) * (Render2D::Grid::BackgroundSize / m_ZoomLevel) * CameraConstants::Settings::PanSensitivity;

            m_PanOffset.x -= panScaleX;
            m_PanOffset.y += panScaleY;

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }

        if (isCanvasHovered)
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                m_ZoomLevel = std::clamp(m_ZoomLevel + wheel * CameraConstants::Settings::ZoomSensitivity,
                    CameraConstants::Settings::MinZoom, CameraConstants::Settings::MaxZoom);
            }
        }

        // Display rendered texture (overlay on the invisible button)
        ImVec2 canvasMin = ImGui::GetItemRectMin();
        ImVec2 canvasMax = ImGui::GetItemRectMax();

        ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<void*>(static_cast<uintptr_t>(m_PreviewRenderTarget->GetTexture())),
            canvasMin,
            canvasMax
        );
    }

    void PanelBrushPreview::RenderBlockBrushInfo()
    {
        ImGui::PushID("BrushInfo");

        RenderSectionRotation();
        RenderSectionSize();
        RenderSectionTint();
        RenderSectionUVs();
        RenderSectionAtlas();

        ImGui::PopID();
    }

    void PanelBrushPreview::RenderSectionRotation()
    {
        const glm::vec3 rotation = Ctx().GetBrush().GetRotation();

        UI::SectionHeader("Rotation");
        UI::ValueVec3("Rotation", glm::value_ptr(rotation), "%.1f°");
    }

    void PanelBrushPreview::RenderSectionSize()
    {
        const glm::vec2 size = Ctx().GetBrush().GetSize();

        UI::SectionHeader("Size");
        UI::ValueVec2("Size", glm::value_ptr(size), "%.1f", "W", "H");
    }

    void PanelBrushPreview::RenderSectionTint()
    {
        const glm::vec4 tint = Ctx().GetBrush().GetTint();

        UI::SectionHeader("Tint");
        UI::ValueVec4("Tint", glm::value_ptr(tint), "%.1f", "R", "G", "B", "A");
        UI::ColorSwatch("TintSwatch", glm::value_ptr(tint));
    }

    void PanelBrushPreview::RenderSectionUVs()
    {
        const glm::vec4 uvs = Ctx().GetBrush().GetTextureCoords();

        UI::SectionHeader("UVs");
        UI::ValueVec4("UVs", glm::value_ptr(uvs), "%.1f", "U1", "V1", "U2", "V2");
    }

    void PanelBrushPreview::RenderSectionAtlas()
    {
        auto& brush = Ctx().GetBrush();

        if (!brush.IsTextured())
            return;

        UI::SectionHeader("Atlas");

        char atlasText[32];
        snprintf(atlasText, sizeof(atlasText), "Index: %zu", brush.GetAtlasIndex());
        UI::ValueField("AtlasIndex", atlasText);
    }
}
