#include "PanelBrushPreview.h"

#include "Lumina/Core/Log.h"

#include <algorithm>

using namespace Lumina;

namespace Tiles
{
    namespace CameraSettings
    {
        static constexpr float DEFAULT_BOUNDS = 2.0f;
        static constexpr float MIN_ZOOM = 1.0f;
        static constexpr float MAX_ZOOM = 5.0f;
        static constexpr float ZOOM_SENSITIVITY = 0.1f;
        static constexpr float PAN_SENSITIVITY = 1.0f;
    }

    namespace RenderDepths
    {
        static constexpr float BACKGROUND = -0.9f;
        static constexpr float GRID = -0.8f;
        static constexpr float BRUSH = 0.0f;
        static constexpr float BOUNDS = 0.1f;
    }

    namespace UILayout
    {
        static constexpr float MIN_WINDOW_SIZE = 64.0f;
        static constexpr float PROPERTY_COLUMN_WIDTH = 80.0f;
    }

    namespace RenderSettings
    {
        static constexpr float BACKGROUND_SIZE = 4.0f;
        static constexpr float GRID_SPACING = 0.2f;
        static constexpr float BOUNDS_OFFSET = 0.05f;
        static constexpr float GRID_LINE_THICKNESS = 1.0f;
    }

    namespace Colors
    {
        static const glm::vec4 GRID_COLOR = { 0.4f, 0.4f, 0.4f, 0.5f };
        static const glm::vec4 BOUNDS_COLOR = { 1.0f, 0.0f, 0.0f, 0.3f };
        static const ImVec4 STATUS_ENABLED_COLOR = { 0.3f, 1.0f, 0.3f, 1.0f };
        static const ImVec4 STATUS_DISABLED_COLOR = { 1.0f, 0.3f, 0.3f, 1.0f };
        static const ImVec4 STATUS_WARNING_COLOR = { 1.0f, 0.6f, 0.0f, 1.0f };
    }

    PanelBrushPreview::PanelBrushPreview()
    {
        m_Camera = CreateRef<OrthographicCamera>();

        auto bounds = CameraSettings::DEFAULT_BOUNDS;
        m_Camera->SetBounds(-bounds, bounds, -bounds, bounds);
        m_Camera->SetPosition({ 0.0f, 0.0f, 1.0f });
        m_Camera->LookAt({ 0.0f, 0.0f, 0.0f });

        m_PreviewRenderTarget = Renderer2D::CreateRenderTarget(512, 512);
    }

    void PanelBrushPreview::Render()
    {
        ImGui::Begin("Brush Preview", nullptr, ImGuiWindowFlags_MenuBar);

        if (!m_Context)
        {
            ImGui::TextColored(Colors::STATUS_DISABLED_COLOR, "No project loaded");
            ImGui::End();
            return;
        }

        RenderPreviewControls();

        Tile& brush = m_Context->GetBrush();

        auto windowSize = ImGui::GetContentRegionAvail();
        windowSize.x = std::max(windowSize.x, UILayout::MIN_WINDOW_SIZE);
        windowSize.y = std::max(windowSize.y, UILayout::MIN_WINDOW_SIZE);

        float previewSize = std::min(windowSize.x, windowSize.y);
        ImVec2 previewDimensions(previewSize, previewSize);

        float bounds = CameraSettings::DEFAULT_BOUNDS / m_ZoomLevel;
        m_Camera->SetBounds(
            -bounds + m_PanOffset.x,
            bounds + m_PanOffset.x,
            bounds + m_PanOffset.y,
            -bounds + m_PanOffset.y
        );

        uint32_t resolutionX = static_cast<uint32_t>(previewDimensions.x);
        uint32_t resolutionY = static_cast<uint32_t>(previewDimensions.y);

        Renderer2D::SetRenderTarget(m_PreviewRenderTarget);
        Renderer2D::SetResolution(resolutionX, resolutionY);
        Renderer2D::Begin(m_Camera);

        RenderBackground();

        if (m_ShowGrid)
        {
            RenderGrid();
        }

        RenderBrush(brush);

        if (m_ShowBounds)
        {
            RenderBounds(brush);
        }

        Renderer2D::End();
        Renderer2D::SetRenderTarget(nullptr);

        HandleMouseInteraction(previewDimensions);

        ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(m_PreviewRenderTarget->GetTexture())), previewDimensions);

        ImGui::Spacing();
        ImGui::Separator();
        RenderBrushInfo();

        ImGui::End();
    }

    void PanelBrushPreview::Update()
    {
    }

    void PanelBrushPreview::RenderPreviewControls()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                ImGui::Checkbox("Show Grid", &m_ShowGrid);
                ImGui::Checkbox("Show Bounds", &m_ShowBounds);
                ImGui::Separator();
                ImGui::SliderFloat("Background", &m_BackgroundBrightness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Zoom", &m_ZoomLevel, CameraSettings::MIN_ZOOM, CameraSettings::MAX_ZOOM, "%.1fx");

                if (ImGui::Button("Reset View"))
                {
                    m_ZoomLevel = 1.0f;
                    m_PanOffset = { 0.0f, 0.0f };
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void PanelBrushPreview::RenderBackground()
    {
        Renderer2D::SetQuadPosition({ 0.0f, 0.0f, RenderDepths::BACKGROUND });
        Renderer2D::SetQuadSize({ RenderSettings::BACKGROUND_SIZE, RenderSettings::BACKGROUND_SIZE });
        Renderer2D::SetQuadTintColor({ m_BackgroundBrightness, m_BackgroundBrightness, m_BackgroundBrightness, 1.0f });
        Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Renderer2D::DrawQuad();
    }

    void PanelBrushPreview::RenderGrid()
    {
        Renderer2D::SetLineColor(Colors::GRID_COLOR);
        Renderer2D::SetLineThickness(RenderSettings::GRID_LINE_THICKNESS);

        for (float x = -CameraSettings::DEFAULT_BOUNDS; x <= CameraSettings::DEFAULT_BOUNDS; x += RenderSettings::GRID_SPACING)
        {
            Renderer2D::SetLineStart({ x, -CameraSettings::DEFAULT_BOUNDS, RenderDepths::GRID });
            Renderer2D::SetLineEnd({ x, CameraSettings::DEFAULT_BOUNDS, RenderDepths::GRID });
            Renderer2D::DrawLine();
        }

        for (float y = -CameraSettings::DEFAULT_BOUNDS; y <= CameraSettings::DEFAULT_BOUNDS; y += RenderSettings::GRID_SPACING)
        {
            Renderer2D::SetLineStart({ -CameraSettings::DEFAULT_BOUNDS, y, RenderDepths::GRID });
            Renderer2D::SetLineEnd({ CameraSettings::DEFAULT_BOUNDS, y, RenderDepths::GRID });
            Renderer2D::DrawLine();
        }
    }

    void PanelBrushPreview::RenderBrush(const Tile& brush)
    {
        Renderer2D::SetQuadPosition({ 0.0f, 0.0f, RenderDepths::BRUSH });
        Renderer2D::SetQuadSize(brush.GetSize());
        Renderer2D::SetQuadRotation(brush.GetRotation());
        Renderer2D::SetQuadTextureCoords(brush.GetTextureCoords());
        Renderer2D::SetQuadTintColor(brush.GetTint());

        if (brush.IsTextured() && brush.HasValidAtlas())
        {
            auto texture = m_Context->GetProject().GetTextureAtlas(brush.GetAtlasIndex())->GetTexture();
            Renderer2D::SetQuadTexture(texture);
        }

        Renderer2D::DrawQuad();
    }

    void PanelBrushPreview::RenderBounds(const Tile& brush)
    {
        Renderer2D::SetQuadPosition({ 0.0f, 0.0f, RenderDepths::BOUNDS });
        Renderer2D::SetQuadSize({ brush.GetSize().x + RenderSettings::BOUNDS_OFFSET,
                                  brush.GetSize().y + RenderSettings::BOUNDS_OFFSET });
        Renderer2D::SetQuadTintColor(Colors::BOUNDS_COLOR);
        Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Renderer2D::DrawQuad();
    }

    void PanelBrushPreview::HandleMouseInteraction(const ImVec2& previewSize)
    {
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("PreviewCanvas", previewSize);

        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            float panScaleX = (delta.x / previewSize.x) * (RenderSettings::BACKGROUND_SIZE / m_ZoomLevel) * CameraSettings::PAN_SENSITIVITY;
            float panScaleY = (delta.y / previewSize.y) * (RenderSettings::BACKGROUND_SIZE / m_ZoomLevel) * CameraSettings::PAN_SENSITIVITY;

            m_PanOffset.x -= panScaleX;
            m_PanOffset.y += panScaleY;

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }

        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                m_ZoomLevel = std::clamp(m_ZoomLevel + wheel * CameraSettings::ZOOM_SENSITIVITY,
                    CameraSettings::MIN_ZOOM, CameraSettings::MAX_ZOOM);
            }
        }

        ImGui::SetCursorScreenPos(canvasPos);
    }

    void PanelBrushPreview::RenderBrushInfo()
    {
        Tile& brush = m_Context->GetBrush();

        ImGui::Text("Brush Information");
        ImGui::Separator();

        if (ImGui::BeginTable("BrushInfo", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, UILayout::PROPERTY_COLUMN_WIDTH);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Textured:");
            ImGui::TableNextColumn();
            {
                auto color = brush.IsTextured() ? Colors::STATUS_ENABLED_COLOR : Colors::STATUS_DISABLED_COLOR;
                ImGui::TextColored(color, brush.IsTextured() ? "Yes" : "No");
            }

            if (brush.IsTextured())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Atlas:");
                ImGui::TableNextColumn();
                if (brush.HasValidAtlas())
                {
                    ImGui::Text("%zu", brush.GetAtlasIndex());
                }
                else
                {
                    ImGui::TextColored(Colors::STATUS_WARNING_COLOR, "Invalid");
                }
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Size:");
            ImGui::TableNextColumn();
            {
                auto size = brush.GetSize();
                ImGui::Text("%.2f x %.2f", size.x, size.y);
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Rotation:");
            ImGui::TableNextColumn();
            {
                auto rotation = brush.GetRotation();
                ImGui::Text("%.1f\u00B0, %.1f\u00B0, %.1f\u00B0", rotation.x, rotation.y, rotation.z);
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Tint:");
            ImGui::TableNextColumn();
            {
                auto tint = brush.GetTint();
                ImGui::Text("R:%.2f G:%.2f B:%.2f A:%.2f", tint.r, tint.g, tint.b, tint.a);
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("UV Coords:");
            ImGui::TableNextColumn();
            {
                auto uvs = brush.GetTextureCoords();
                ImGui::Text("(%.2f,%.2f) to (%.2f,%.2f)", uvs.x, uvs.y, uvs.z, uvs.w);
            }

            ImGui::EndTable();
        }
    }
}