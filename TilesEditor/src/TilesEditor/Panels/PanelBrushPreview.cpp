#include "PanelBrushPreview.h"

#include "Core/Constants.h"

#include "Core/Log.h"

namespace Tiles
{
    PanelBrushPreview::PanelBrushPreview(std::shared_ptr<Context> context) : Panel(context)
    {
        m_Camera = std::make_shared<Tiles::OrthographicCamera>();

        auto bounds = CameraConstants::Settings::DefaultBounds;
        m_Camera->SetBounds(-bounds, bounds, -bounds, bounds);
        m_Camera->SetPosition({ 0.0f, 0.0f, 1.0f });
        m_Camera->LookAt({ 0.0f, 0.0f, 0.0f });

        m_PreviewRenderTarget = Tiles::Renderer2D::CreateRenderTarget(512, 512);
    }

    void PanelBrushPreview::Render()
    {
        ImGui::Begin("Brush Preview", nullptr, ImGuiWindowFlags_MenuBar);

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

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

        // Update camera bounds based on zoom and pan
        float bounds = CameraConstants::Settings::DefaultBounds / m_ZoomLevel;
        m_Camera->SetBounds(
            -bounds + m_PanOffset.x,
            bounds + m_PanOffset.x,
            -bounds + m_PanOffset.y,
            bounds + m_PanOffset.y
        );

        uint32_t resolutionX = static_cast<uint32_t>(previewDimensions.x);
        uint32_t resolutionY = static_cast<uint32_t>(previewDimensions.y);

        // Begin 3D rendering
        Tiles::Renderer2D::SetRenderTarget(m_PreviewRenderTarget);
        Tiles::Renderer2D::SetResolution(resolutionX, resolutionY);
        Tiles::Renderer2D::Begin(m_Camera);

        // Render background
        Tiles::Renderer2D::SetQuadPosition({ 0.0f, 0.0f, Render2D::Depth::Background });
        Tiles::Renderer2D::SetQuadSize({ Render2D::Grid::BackgroundSize, Render2D::Grid::BackgroundSize });
        Tiles::Renderer2D::SetQuadTintColor({ m_BackgroundBrightness, m_BackgroundBrightness, m_BackgroundBrightness, 1.0f });
        Tiles::Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Tiles::Renderer2D::DrawQuad();

        // Render grid if enabled
        if (m_ShowGrid)
        {
            Tiles::Renderer2D::SetLineColor(Render2D::Grid::Color);
            Tiles::Renderer2D::SetLineThickness(Render2D::Grid::LineThickness);

            // Vertical lines
            for (float x = -CameraConstants::Settings::DefaultBounds; x <= CameraConstants::Settings::DefaultBounds; x += Render2D::Grid::Spacing)
            {
                Tiles::Renderer2D::SetLineStart({ x, -CameraConstants::Settings::DefaultBounds, Render2D::Depth::Grid });
                Tiles::Renderer2D::SetLineEnd({ x, CameraConstants::Settings::DefaultBounds, Render2D::Depth::Grid });
                Tiles::Renderer2D::DrawLine();
            }

            // Horizontal lines
            for (float y = -CameraConstants::Settings::DefaultBounds; y <= CameraConstants::Settings::DefaultBounds; y += Render2D::Grid::Spacing)
            {
                Tiles::Renderer2D::SetLineStart({ -CameraConstants::Settings::DefaultBounds, y, Render2D::Depth::Grid });
                Tiles::Renderer2D::SetLineEnd({ CameraConstants::Settings::DefaultBounds, y, Render2D::Depth::Grid });
                Tiles::Renderer2D::DrawLine();
            }
        }

        // Render brush
        auto& brush = m_Context->GetBrush();
        Tiles::Renderer2D::SetQuadPosition({ 0.0f, 0.0f, Render2D::Depth::Brush });
        Tiles::Renderer2D::SetQuadSize(brush.GetSize());
        Tiles::Renderer2D::SetQuadRotation(brush.GetRotation());
        Tiles::Renderer2D::SetQuadTextureCoords(brush.GetTextureCoords());
        Tiles::Renderer2D::SetQuadTintColor(brush.GetTint());

        if (brush.IsTextured() && brush.HasValidAtlas())
        {
            auto texture = m_Context->GetProject()->GetTextureAtlas(brush.GetAtlasIndex())->GetTexture();
            Tiles::Renderer2D::SetQuadTexture(texture);
        }

        Tiles::Renderer2D::DrawQuad();

        // Render bounds if enabled
        if (m_ShowBounds)
        {
            Tiles::Renderer2D::SetQuadPosition({ 0.0f, 0.0f, Render2D::Depth::Bounds });
            Tiles::Renderer2D::SetQuadSize({ brush.GetSize().x + Render2D::Bounds::Offset,
                                      brush.GetSize().y + Render2D::Bounds::Offset });
            Tiles::Renderer2D::SetQuadTintColor(Render2D::Bounds::Color);
            Tiles::Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
            Tiles::Renderer2D::DrawQuad();
        }

        Tiles::Renderer2D::End();
        Tiles::Renderer2D::SetRenderTarget(nullptr);

		Tiles::Renderer2D::ResetQuadState();
        Tiles::Renderer2D::ResetLineState();

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
        // Set custom spacing for the entire info section
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(UI::Component::SpaceBetween, UI::Component::SpaceBetween));

        ImGui::PushID("BrushInfo");

        RenderSectionRotation();
        RenderSectionSize();
        RenderSectionTint();
        RenderSectionUVs();
        RenderSectionAtlas();

        ImGui::PopID();

        ImGui::PopStyleVar();
    }

    void PanelBrushPreview::RenderSectionRotation()
    {
        auto& brush = m_Context->GetBrush();
        auto rotation = brush.GetRotation();

        RenderComponentTitle("Rotation");
        RenderComponentVec3Table("Rotation", rotation, "%.1f\u00B0");
    }

    void PanelBrushPreview::RenderSectionSize()
    {
        auto& brush = m_Context->GetBrush();
        auto size = brush.GetSize();

        RenderComponentTitle("Size");
        RenderComponentVec2Table("Size", size, "%.1f", "W", "H");
    }

    void PanelBrushPreview::RenderSectionTint()
    {
        auto& brush = m_Context->GetBrush();
        auto tint = brush.GetTint();

        RenderComponentTitle("Tint");
        RenderComponentVec4Table("Tint", tint, "%.1f", "R", "G", "B", "A");
        RenderComponentColorSwatch("TintSwatch", tint);
    }

    void PanelBrushPreview::RenderSectionUVs()
    {
        auto& brush = m_Context->GetBrush();
        auto uvs = brush.GetTextureCoords();

        RenderComponentTitle("UVs");
        RenderComponentVec4Table("UVs", uvs, "%.1f", "U1", "V1", "U2", "V2");
    }

    void PanelBrushPreview::RenderSectionAtlas()
    {
        auto& brush = m_Context->GetBrush();

        if (brush.IsTextured())
        {
            RenderComponentTitle("Atlas");

            // Create a simple display for atlas index
            ImGui::PushID("AtlasIndex");
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundDark);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundDark);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundDark);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

            char atlasText[32];
            snprintf(atlasText, sizeof(atlasText), "Index: %zu", brush.GetAtlasIndex());
            ImGui::Button(atlasText, ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();
            ImGui::PopID();

            ImGui::Spacing();
        }
    }

    void PanelBrushPreview::RenderComponentTitle(const char* title)
    {
        ImGui::PushID(title);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button(title, ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();

        ImGui::Spacing();
    }

    void PanelBrushPreview::RenderComponentVec2Table(const char* id, const glm::vec2& vec, const char* format, const char* xName, const char* yName)
    {
        std::string tableId = std::string("##") + id + "Table";

        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";

        std::string labelXId = std::string(id) + "LabelX";
        std::string labelYId = std::string(id) + "LabelY";

        std::string valueXId = std::string(id) + "ValueX";
        std::string valueYId = std::string(id) + "ValueY";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 2, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueXId.c_str(), vec.x, format);

            // Y component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueYId.c_str(), vec.y, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushPreview::RenderComponentVec3Table(const char* id, const glm::vec3& vec, const char* format, const char* xName, const char* yName, const char* zName)
    {
        std::string tableId = std::string("##") + id + "Table";

        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";
        std::string colZId = std::string(id) + "Z";

        std::string labelXId = std::string(id) + "LabelX";
        std::string labelYId = std::string(id) + "LabelY";
        std::string labelZId = std::string(id) + "LabelZ";

        std::string valueXId = std::string(id) + "ValueX";
        std::string valueYId = std::string(id) + "ValueY";
        std::string valueZId = std::string(id) + "ValueZ";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 3, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colZId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueXId.c_str(), vec.x, format);

            // Y component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueYId.c_str(), vec.y, format);

            // Z component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelZId.c_str(), zName, UI::Color::Blue);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueZId.c_str(), vec.z, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushPreview::RenderComponentVec4Table(const char* id, const glm::vec4& vec, const char* format, const char* xName, const char* yName, const char* zName, const char* wName)
    {
        std::string tableId = std::string("##") + id + "Table";

        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";
        std::string colZId = std::string(id) + "Z";
        std::string colWId = std::string(id) + "W";

        std::string labelXId = std::string(id) + "LabelX";
        std::string labelYId = std::string(id) + "LabelY";
        std::string labelZId = std::string(id) + "LabelZ";
        std::string labelWId = std::string(id) + "LabelW";

        std::string valueXId = std::string(id) + "ValueX";
        std::string valueYId = std::string(id) + "ValueY";
        std::string valueZId = std::string(id) + "ValueZ";
        std::string valueWId = std::string(id) + "ValueW";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 4, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colZId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colWId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueXId.c_str(), vec.x, format);

            // Y component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueYId.c_str(), vec.y, format);

            // Z component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelZId.c_str(), zName, UI::Color::Blue);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueZId.c_str(), vec.z, format);

            // W component
            ImGui::TableNextColumn();
            RenderComponentSpacing();
            RenderComponentLabel(labelWId.c_str(), wName, UI::Color::Gray);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentValue(valueWId.c_str(), vec.w, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushPreview::RenderComponentLabel(const char* id, const char* label, const ImVec4& color)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button(label, ImVec2(UI::Component::ButtonWidth, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void PanelBrushPreview::RenderComponentValue(const char* id, float value, const char* format)
    {
        ImGui::PushID(id);
        char buffer[32];
        snprintf(buffer, sizeof(buffer), format, value);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundDark);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundDark);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundDark);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button(buffer, ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void PanelBrushPreview::RenderComponentColorSwatch(const char* id, const glm::vec4& color)
    {
        ImGui::PushID(id);
        ImVec4 colorPreview = ImVec4(color.r, color.g, color.b, color.a);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::ColorButton("##ColorSwatch", colorPreview,
            ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip,
            ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleVar();
        ImGui::PopID();

        ImGui::Spacing();
    }

    void PanelBrushPreview::RenderComponentSpacing()
    {
        ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
        ImGui::SameLine(0, 0);
    }
}