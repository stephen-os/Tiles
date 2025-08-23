#include "PanelTextureSelection.h"
#include "Constants.h"
#include "ImGuiFileDialog.h"
#include "Lumina/Core/Log.h"
#include <filesystem>
#include <algorithm>

namespace Tiles
{
    PanelTextureSelection::PanelTextureSelection(Ref<Context> context) : Panel(context)
    { 
        m_CheckerboardTexture = Lumina::Texture::Create(AssetPath::Checkerboard);
    }

    void PanelTextureSelection::Render()
    {
        ImGui::Begin("Texture Selection");

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextHint, "No active project");
            ImGui::End();
            return;
        }

        ImGui::PushID("TextureSelection");

        RenderBlockAtlasTabs();
        ImGui::Separator();
        RenderBlockAtlasControls();
        ImGui::Separator();
        RenderBlockCurrentAtlasContent();
        RenderBlockFileDialog();

        ImGui::PopID();
        ImGui::End();
    }

    void PanelTextureSelection::Update()
    {
        // No specific update logic required for this panel
    }

    void PanelTextureSelection::RenderBlockAtlasTabs()
    {
        auto& atlases = m_Context->GetProject()->GetTextureAtlases();

        if (atlases.empty())
        {
            ImGui::Text("No texture atlases. Click 'Add Atlas' to create one.");
            return;
        }

        if (ImGui::BeginTabBar("##AtlasTabBar"))
        {
            for (size_t i = 0; i < atlases.size(); ++i)
            {
                std::string tabName = "Atlas " + std::to_string(i + 1);
                RenderComponentAtlasTab(("AtlasTab_" + std::to_string(i)).c_str(), i, tabName.c_str());
            }
            ImGui::EndTabBar();
        }
    }

    void PanelTextureSelection::RenderComponentAtlasTab(const char* id, size_t atlasIndex, const char* tabName)
    {
        if (ImGui::BeginTabItem(tabName))
        {
            ImGui::EndTabItem();
        }

        if (ImGui::IsItemClicked())
        {
            SetCurrentAtlasIndex(atlasIndex);
        }
    }

    void PanelTextureSelection::RenderBlockAtlasControls()
    {
        if (ImGui::Button("Add Atlas"))
        {
            AddNewAtlas();
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Atlas"))
        {
            RemoveCurrentAtlas();
        }
    }

    void PanelTextureSelection::RenderBlockCurrentAtlasContent()
    {
        if (!HasValidCurrentAtlas())
        {
            ImGui::Text("No atlas selected");
            return;
        }

        RenderSectionAtlasPath();
        ImGui::Separator();
        RenderSectionAtlasDimensions();
        ImGui::Separator();

        ImGui::BeginChild("##TextureSelectionChild", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        RenderSectionTextureGrid();
        ImGui::EndChild();
    }

    void PanelTextureSelection::RenderSectionAtlasPath()
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(m_CurrentAtlasIndex);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Atlas:");
        ImGui::SameLine();

        if (!atlas->HasTexture())
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("[ No file selected ]");
            ImGui::SameLine();

            if (ImGui::Button("Browse..."))
            {
                OpenFileDialog();
            }
        }
        else
        {
            std::string path = atlas->GetTexture()->GetPath();
            RenderComponentFilePathDisplay("AtlasPath", path);
            ImGui::SameLine();

            if (ImGui::Button("Remove"))
            {
                atlas->RemoveTexture();
                m_Context->GetProject()->MarkAsModified();
            }
        }
    }

    void PanelTextureSelection::RenderComponentFilePathDisplay(const char* id, const std::string& path)
    {
        ImGui::AlignTextToFramePadding();
        std::string filename = path.substr(path.find_last_of("/\\") + 1);
        ImGui::TextWrapped("[ %s ]", filename.c_str());
    }

    void PanelTextureSelection::RenderSectionAtlasDimensions()
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(m_CurrentAtlasIndex);

        ImGui::Text("Atlas Dimensions");
        ImGui::PushItemWidth(UI::Component::InputWidth);

        int width = atlas->GetWidth();
        RenderComponentDimensionInput("AtlasWidth", "Width", &width);
        if (width != atlas->GetWidth())
        {
            atlas->Resize(std::max(1, width), atlas->GetHeight());
            m_Context->GetProject()->MarkAsModified();
        }

        int height = atlas->GetHeight();
        RenderComponentDimensionInput("AtlasHeight", "Height", &height);
        if (height != atlas->GetHeight())
        {
            atlas->Resize(atlas->GetWidth(), std::max(1, height));
            m_Context->GetProject()->MarkAsModified();
        }

        ImGui::PopItemWidth();
    }

    void PanelTextureSelection::RenderComponentDimensionInput(const char* id, const char* label, int* value)
    {
        std::string inputId = std::string("##") + id + "_Input";
        ImGui::InputInt(inputId.c_str(), value);
    }

    void PanelTextureSelection::RenderSectionTextureGrid()
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(m_CurrentAtlasIndex);

        // Set up grid styling for seamless tiles
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        // Calculate dynamic tile size based on available width
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        float currentTileSize = CalculateDynamicTileSize(availableSize.x, atlas->GetWidth());

        // Render texture grid with checkerboard backgrounds
        for (int y = 0; y < atlas->GetHeight(); y++)
        {
            for (int x = 0; x < atlas->GetWidth(); x++)
            {
                // Draw checkerboard background
                ImVec2 currentPos = ImGui::GetCursorScreenPos();
                ImVec2 minPos = currentPos;
                ImVec2 maxPos = ImVec2(currentPos.x + currentTileSize, currentPos.y + currentTileSize);

                if (m_CheckerboardTexture)
                {
                    auto checkerboardID = reinterpret_cast<void*>(static_cast<uintptr_t>(m_CheckerboardTexture->GetID()));
                    ImGui::GetWindowDrawList()->AddImage(checkerboardID, minPos, maxPos, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
                }

                int index = y * atlas->GetWidth() + x;
                std::string itemId = "GridItem_" + std::to_string(x) + "_" + std::to_string(y);
                RenderComponentTextureGridItem(itemId.c_str(), index, x, y, m_CurrentAtlasIndex, currentTileSize);

                // Handle row wrapping
                if ((index + 1) % atlas->GetWidth() != 0)
                {
                    ImGui::SameLine();
                }
            }
        }

        ImGui::PopStyleVar(4);
    }

    void PanelTextureSelection::RenderComponentTextureGridItem(const char* id, int index, int x, int y, size_t atlasIndex, float tileSize)
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(m_CurrentAtlasIndex);

        ImVec2 buttonSize(tileSize, tileSize);

        // Render texture or transparent empty button
        if (atlas->HasTexture())
        {
            glm::vec4 texCoords = atlas->GetTextureCoords(index);
            ImVec2 uvMin(texCoords.x, texCoords.y);
            ImVec2 uvMax(texCoords.z, texCoords.w);
            auto textureID = reinterpret_cast<void*>(static_cast<uintptr_t>(atlas->GetTexture()->GetID()));

            ImGui::Image(textureID, buttonSize, uvMin, uvMax, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
        }
        else
        {
            // Transparent button for empty atlas slots
            ImGui::PushStyleColor(ImGuiCol_Button, UI::Selection::TransparentButton);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Selection::TransparentButton);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Selection::TransparentButton);

            std::string buttonId = std::string("##") + id + "_Button";
            ImGui::Button(buttonId.c_str(), buttonSize);

            ImGui::PopStyleColor(3);
        }

        // Handle texture selection
        if (ImGui::IsItemClicked())
        {
            HandleTextureSelection(index, atlasIndex);
        }

        // Draw selection border
        RenderComponentSelectionBorder(id, index, atlasIndex, tileSize);
    }

    void PanelTextureSelection::RenderComponentSelectionBorder(const char* id, int index, size_t atlasIndex, float tileSize)
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(atlasIndex);
        auto& brush = m_Context->GetBrush();

        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();

        // Check if this texture is selected
        bool isSelected = (brush.GetAtlasIndex() == atlasIndex &&
            brush.GetTextureCoords() == atlas->GetTextureCoords(index) &&
            brush.IsTextured());

        // Scale border thickness with tile size
        float borderThickness = std::max(Texture::Tile::MinBorderThickness,
            tileSize * Texture::Tile::BorderThicknessRatio);

        // Draw appropriate border
        ImU32 borderColor = isSelected ?
            ImGui::ColorConvertFloat4ToU32(UI::Selection::BorderColor) :
            ImGui::ColorConvertFloat4ToU32(UI::Selection::DefaultBorderColor);
        float thickness = isSelected ? borderThickness : Texture::Tile::MinBorderThickness;

        ImGui::GetWindowDrawList()->AddRect(itemMin, itemMax, borderColor, 0.0f, 0, thickness);
    }

    void PanelTextureSelection::RenderBlockFileDialog()
    {
        ImVec2 dialogSize(UI::Dialog::FileDialogWidth, UI::Dialog::FileDialogHeight);

        if (ImGuiFileDialog::Instance()->Display(Texture::FileDialog::DialogKey, ImGuiWindowFlags_NoCollapse, dialogSize))
        {
            HandleFileDialogResult();
        }
    }

    float PanelTextureSelection::CalculateDynamicTileSize(float availableWidth, int atlasWidth) const
    {
        if (atlasWidth <= 0)
        {
            return Texture::Tile::PreferredSize;
        }

        // Calculate what tile size would fit the available width
        float widthBasedTileSize = availableWidth / atlasWidth;

        // Clamp to our min/max bounds
        float clampedSize = std::clamp(widthBasedTileSize, Texture::Tile::MinSize, Texture::Tile::MaxSize);

        // For small atlases, prefer larger tiles for visibility
        if (atlasWidth <= Texture::Atlas::SmallThreshold)
        {
            return std::max(clampedSize, Texture::Tile::PreferredSize);
        }

        // For medium atlases, use calculated size
        if (atlasWidth <= Texture::Atlas::MediumThreshold)
        {
            return clampedSize;
        }

        // For large atlases, prefer smaller tiles but ensure minimum visibility
        return std::max(clampedSize, Texture::Tile::MinSize);
    }

    void PanelTextureSelection::HandleAtlasFileSelection(const std::string& newPath)
    {
        if (std::filesystem::exists(newPath))
        {
            auto atlas = m_Context->GetProject()->GetTextureAtlas(m_CurrentAtlasIndex);
            if (atlas)
            {
                std::filesystem::path relativePath = std::filesystem::relative(newPath, std::filesystem::current_path());
                atlas->SetTexture(relativePath.string());
                m_Context->GetProject()->MarkAsModified();
            }
        }
    }

    void PanelTextureSelection::HandleTextureSelection(int index, size_t atlasIndex)
    {
        auto atlas = m_Context->GetProject()->GetTextureAtlas(atlasIndex);
        auto& brush = m_Context->GetBrush();
        glm::vec4 texCoords = atlas->GetTextureCoords(index);

        // Check if this texture is already selected
        bool isCurrentlySelected = (brush.GetAtlasIndex() == atlasIndex &&
            brush.GetTextureCoords() == texCoords &&
            brush.IsTextured());

        if (isCurrentlySelected)
        {
            // Deselect current texture
            Tile newBrush = brush;
            newBrush.SetTextured(false);
            newBrush.SetAtlasIndex(Tile::INVALID_ATLAS_INDEX);
            m_Context->SetBrush(newBrush);
        }
        else
        {
            // Select new texture
            Tile newBrush = brush;
            newBrush.SetTextured(true);
            newBrush.SetAtlasIndex(atlasIndex);
            newBrush.SetTextureCoords(texCoords);
            m_Context->SetBrush(newBrush);
        }

        m_Context->GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::OpenFileDialog()
    {
        IGFD::FileDialogConfig config;
        config.path = Texture::FileDialog::DefaultPath;
        config.flags = ImGuiFileDialogFlags_Modal;
        config.countSelectionMax = 1;

        ImGuiFileDialog::Instance()->OpenDialog(
            Texture::FileDialog::DialogKey,
            Texture::FileDialog::DialogTitle,
            Texture::FileDialog::FileFilters,
            config
        );
    }

    void PanelTextureSelection::HandleFileDialogResult()
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            HandleAtlasFileSelection(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    void PanelTextureSelection::AddNewAtlas()
    {
        auto newAtlas = TextureAtlas::Create(Texture::Atlas::DefaultWidth, Texture::Atlas::DefaultHeight);
        m_Context->GetProject()->AddTextureAtlas(newAtlas);
        SetCurrentAtlasIndex(m_Context->GetProject()->GetTextureAtlasCount() - 1);
        m_Context->GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::RemoveCurrentAtlas()
    {
        auto& atlases = m_Context->GetProject()->GetTextureAtlases();
        if (atlases.empty())
        {
            return;
        }

        m_Context->GetProject()->RemoveTextureAtlas(m_CurrentAtlasIndex);

        // Adjust current index if necessary
        if (m_CurrentAtlasIndex >= atlases.size() && !atlases.empty())
        {
            m_CurrentAtlasIndex = atlases.size() - 1;
        }
        else if (atlases.empty())
        {
            m_CurrentAtlasIndex = 0;
        }

        m_Context->GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::SetCurrentAtlasIndex(size_t index)
    {
        auto& atlases = m_Context->GetProject()->GetTextureAtlases();
        if (index < atlases.size())
        {
            m_CurrentAtlasIndex = index;
        }
    }

    bool PanelTextureSelection::HasValidCurrentAtlas() const
    {
        auto& atlases = m_Context->GetProject()->GetTextureAtlases();
        return m_CurrentAtlasIndex < atlases.size();
    }
}