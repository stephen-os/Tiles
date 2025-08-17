#include "PanelTextureSelection.h"

#include "ImGuiFileDialog.h"

#include "Lumina/Core/Log.h"

#include <filesystem>
#include <algorithm>

namespace Tiles
{
    // Asset paths
    namespace AssetPaths
    {
        static constexpr const char* CHECKERBOARD_TEXTURE = "res/assets/checkerboard.png";
    }

    // UI layout constants
    namespace UILayout
    {
        static constexpr float INPUT_WIDTH = 100.0f;
        static constexpr float FILE_DIALOG_WIDTH = 600.0f;
        static constexpr float FILE_DIALOG_HEIGHT = 400.0f;
    }

    // Texture grid configuration
    namespace TextureGrid
    {
        static constexpr float MIN_TILE_SIZE = 16.0f;
        static constexpr float MAX_TILE_SIZE = 128.0f;
        static constexpr float PREFERRED_TILE_SIZE = 40.0f;
        static constexpr int SMALL_ATLAS_THRESHOLD = 4;
        static constexpr int MEDIUM_ATLAS_THRESHOLD = 16;
        static constexpr float BORDER_THICKNESS_RATIO = 0.0625f;
        static constexpr float MIN_BORDER_THICKNESS = 1.0f;
    }

    // Atlas creation defaults
    namespace AtlasDefaults
    {
        static constexpr int DEFAULT_WIDTH = 16;
        static constexpr int DEFAULT_HEIGHT = 16;
    }

    // File dialog configuration
    namespace FileDialog
    {
        static constexpr const char* DIALOG_KEY = "ChooseAtlasFileDlg";
        static constexpr const char* DIALOG_TITLE = "Choose Atlas File";
        static constexpr const char* FILE_FILTERS = ".png,.jpg,.jpeg";
        static constexpr const char* DEFAULT_PATH = ".";
    }

    // Color scheme
    namespace ColorScheme
    {
        // Vec4
        static const ImVec4 NO_PROJECT_COLOR = { 0.7f, 0.7f, 0.7f, 1.0f };
        static const ImVec4 TRANSPARENT_BUTTON = { 0.0f, 0.0f, 0.0f, 0.0f };

		// ImU32
        static constexpr ImU32 SELECTED_BORDER = IM_COL32(255, 165, 0, 255);
        static constexpr ImU32 DEFAULT_BORDER = IM_COL32(51, 51, 51, 255);
    }

    // UI text constants
    namespace UIText
    {
        static constexpr const char* NO_ACTIVE_PROJECT = "No active project";
        static constexpr const char* NO_ATLASES_MESSAGE = "No texture atlases. Click 'Add Atlas' to create one.";
        static constexpr const char* NO_ATLAS_SELECTED = "No atlas selected";
        static constexpr const char* NO_FILE_SELECTED = "[ No file selected ]";
        static constexpr const char* ATLAS_DIMENSIONS = "Atlas Dimensions";
        static constexpr const char* ATLAS_LABEL = "Atlas:";
        static constexpr const char* BROWSE_BUTTON = "Browse...";
        static constexpr const char* REMOVE_BUTTON = "Remove";
        static constexpr const char* ADD_ATLAS_BUTTON = "Add Atlas";
        static constexpr const char* REMOVE_ATLAS_BUTTON = "Remove Atlas";
    }

    PanelTextureSelection::PanelTextureSelection()
    {
        m_CheckerBoard = Texture::Create(AssetPaths::CHECKERBOARD_TEXTURE);
    }

    void PanelTextureSelection::Render()
    {
        ImGui::Begin("Texture Selection");

        if (!HasContext())
        {
            ImGui::TextColored(ColorScheme::NO_PROJECT_COLOR, UIText::NO_ACTIVE_PROJECT);
            ImGui::End();
            return;
        }

        RenderAtlasTabs();
        ImGui::Separator();

        RenderAtlasControls();
        ImGui::Separator();

        RenderCurrentAtlasContent();
        RenderFileDialog();

        ImGui::End();
    }

    void PanelTextureSelection::Update()
    {
        // This panel does not require any specific update logic
	}

    void PanelTextureSelection::RenderAtlasTabs()
    {
        auto& atlases = m_Context->GetProject().GetTextureAtlases();

        if (atlases.empty())
        {
            ImGui::Text(UIText::NO_ATLASES_MESSAGE);
            return;
        }

        // Render tabs for each atlas
        if (ImGui::BeginTabBar("AtlasTabs"))
        {
            for (size_t i = 0; i < atlases.size(); ++i)
            {
                std::string tabName = "Atlas " + std::to_string(i + 1);

                if (ImGui::BeginTabItem(tabName.c_str()))
                {
                    ImGui::EndTabItem();
                }

                if (ImGui::IsItemClicked())
                {
                    SetCurrentAtlasIndex(i);
                }
            }
            ImGui::EndTabBar();
        }
    }

    void PanelTextureSelection::RenderAtlasControls()
    {
        if (ImGui::Button(UIText::ADD_ATLAS_BUTTON))
        {
            AddNewAtlas();
        }

        ImGui::SameLine();

        if (ImGui::Button(UIText::REMOVE_ATLAS_BUTTON))
        {
            RemoveCurrentAtlas();
        }
    }

    void PanelTextureSelection::RenderCurrentAtlasContent()
    {
        if (!HasValidCurrentAtlas())
        {
            ImGui::Text(UIText::NO_ATLAS_SELECTED);
            return;
        }

        RenderAtlasPathSection();
        ImGui::Separator();
        RenderAtlasDimensionsSection();
        ImGui::Separator();

        ImGui::BeginChild("TextureSelection", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        RenderTextureGrid();
        ImGui::EndChild();
    }

    void PanelTextureSelection::RenderAtlasPathSection()
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(m_CurrentAtlasIndex);

        ImGui::AlignTextToFramePadding();
        ImGui::Text(UIText::ATLAS_LABEL);
        ImGui::SameLine();

        if (!atlas->HasTexture())
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped(UIText::NO_FILE_SELECTED);
            ImGui::SameLine();

            if (ImGui::Button(UIText::BROWSE_BUTTON))
            {
                OpenFileDialog();
            }
        }
        else
        {
            ImGui::AlignTextToFramePadding();
            std::string path = atlas->GetTexture()->GetPath();
            std::string filename = path.substr(path.find_last_of("/\\") + 1);
            ImGui::TextWrapped("[ %s ]", filename.c_str());
            ImGui::SameLine();

            if (ImGui::Button(UIText::REMOVE_BUTTON))
            {
                atlas->RemoveTexture();
                m_Context->GetProject().MarkAsModified();
            }
        }
    }

    void PanelTextureSelection::RenderAtlasDimensionsSection()
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(m_CurrentAtlasIndex);

        ImGui::Text(UIText::ATLAS_DIMENSIONS);
        ImGui::PushItemWidth(UILayout::INPUT_WIDTH);

        int width = atlas->GetWidth();
        if (ImGui::InputInt("##Width", &width))
        {
            atlas->Resize(std::max(1, width), atlas->GetHeight());
            m_Context->GetProject().MarkAsModified();
        }

        int height = atlas->GetHeight();
        if (ImGui::InputInt("##Height", &height))
        {
            atlas->Resize(atlas->GetWidth(), std::max(1, height));
            m_Context->GetProject().MarkAsModified();
        }

        ImGui::PopItemWidth();
    }

    void PanelTextureSelection::RenderTextureGrid()
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(m_CurrentAtlasIndex);

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

                if (m_CheckerBoard)
                {
                    auto checkerboardID = reinterpret_cast<void*>(static_cast<uintptr_t>(m_CheckerBoard->GetID()));
                    ImGui::GetWindowDrawList()->AddImage(checkerboardID, minPos, maxPos, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
                }

                int index = y * atlas->GetWidth() + x;
                RenderTextureGridItem(index, x, y, m_CurrentAtlasIndex, currentTileSize);

                // Handle row wrapping
                if ((index + 1) % atlas->GetWidth() != 0)
                {
                    ImGui::SameLine();
                }
            }
        }

        ImGui::PopStyleVar(4);
    }

    float PanelTextureSelection::CalculateDynamicTileSize(float availableWidth, int atlasWidth) const
    {
        if (atlasWidth <= 0)
        {
            return TextureGrid::PREFERRED_TILE_SIZE;
        }

        // Calculate what tile size would fit the available width
        float widthBasedTileSize = availableWidth / atlasWidth;

        // Clamp to our min/max bounds
        float clampedSize = std::clamp(widthBasedTileSize, TextureGrid::MIN_TILE_SIZE, TextureGrid::MAX_TILE_SIZE);

        // For small atlases, prefer larger tiles for visibility
        if (atlasWidth <= TextureGrid::SMALL_ATLAS_THRESHOLD)
        {
            return std::max(clampedSize, TextureGrid::PREFERRED_TILE_SIZE);
        }

        // For medium atlases, use calculated size
        if (atlasWidth <= TextureGrid::MEDIUM_ATLAS_THRESHOLD)
        {
            return clampedSize;
        }

        // For large atlases, prefer smaller tiles but ensure minimum visibility
        return std::max(clampedSize, TextureGrid::MIN_TILE_SIZE);
    }

    void PanelTextureSelection::RenderTextureGridItem(int index, int x, int y, size_t atlasIndex, float tileSize)
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(m_CurrentAtlasIndex);
        auto& brush = m_Context->GetBrush();

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
            ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::TRANSPARENT_BUTTON);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::TRANSPARENT_BUTTON);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::TRANSPARENT_BUTTON);

            std::string buttonId = "##" + std::to_string(atlasIndex) + "_" + std::to_string(x) + "_" + std::to_string(y);
            ImGui::Button(buttonId.c_str(), buttonSize);

            ImGui::PopStyleColor(3);
        }

        // Handle texture selection
        if (ImGui::IsItemClicked())
        {
            HandleTextureSelection(index, atlasIndex);
        }

        // Draw selection border
        DrawSelectionBorder(index, atlasIndex, tileSize);
    }

    void PanelTextureSelection::HandleTextureSelection(int index, size_t atlasIndex)
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(atlasIndex);
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

        m_Context->GetProject().MarkAsModified();
    }

    void PanelTextureSelection::DrawSelectionBorder(int index, size_t atlasIndex, float tileSize)
    {
        auto atlas = m_Context->GetProject().GetTextureAtlas(atlasIndex);
        auto& brush = m_Context->GetBrush();

        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();

        // Check if this texture is selected
        bool isSelected = (brush.GetAtlasIndex() == atlasIndex &&
            brush.GetTextureCoords() == atlas->GetTextureCoords(index) &&
            brush.IsTextured());

        // Scale border thickness with tile size
        float borderThickness = std::max(TextureGrid::MIN_BORDER_THICKNESS,
            tileSize * TextureGrid::BORDER_THICKNESS_RATIO);

        // Draw appropriate border
        ImU32 borderColor = isSelected ? ColorScheme::SELECTED_BORDER : ColorScheme::DEFAULT_BORDER;
        float thickness = isSelected ? borderThickness : TextureGrid::MIN_BORDER_THICKNESS;

        ImGui::GetWindowDrawList()->AddRect(itemMin, itemMax, borderColor, 0.0f, 0, thickness);
    }

    void PanelTextureSelection::RenderFileDialog()
    {
        ImVec2 dialogSize(UILayout::FILE_DIALOG_WIDTH, UILayout::FILE_DIALOG_HEIGHT);

        if (ImGuiFileDialog::Instance()->Display(FileDialog::DIALOG_KEY, ImGuiWindowFlags_NoCollapse, dialogSize))
        {
            HandleFileDialogResult();
        }
    }

    // Helper Methods Implementation
    void PanelTextureSelection::HandleAtlasFileSelection(const std::string& newPath)
    {
        if (std::filesystem::exists(newPath))
        {
            auto atlas = m_Context->GetProject().GetTextureAtlas(m_CurrentAtlasIndex);
            if (atlas)
            {
                std::filesystem::path relativePath = std::filesystem::relative(newPath, std::filesystem::current_path());
                atlas->SetTexture(relativePath.string());
                m_Context->GetProject().MarkAsModified();
            }
        }
    }

    void PanelTextureSelection::OpenFileDialog()
    {
        IGFD::FileDialogConfig config;
        config.path = FileDialog::DEFAULT_PATH;
        config.flags = ImGuiFileDialogFlags_Modal;
        config.countSelectionMax = 1;

        ImGuiFileDialog::Instance()->OpenDialog(
            FileDialog::DIALOG_KEY,
            FileDialog::DIALOG_TITLE,
            FileDialog::FILE_FILTERS,
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
        auto newAtlas = TextureAtlas::Create(AtlasDefaults::DEFAULT_WIDTH, AtlasDefaults::DEFAULT_HEIGHT);
        m_Context->GetProject().AddTextureAtlas(newAtlas);
        SetCurrentAtlasIndex(m_Context->GetProject().GetTextureAtlasCount() - 1);
        m_Context->GetProject().MarkAsModified();
    }

    void PanelTextureSelection::RemoveCurrentAtlas()
    {
        auto& atlases = m_Context->GetProject().GetTextureAtlases();
        if (atlases.empty())
        {
            return;
        }

        m_Context->GetProject().RemoveTextureAtlas(m_CurrentAtlasIndex);

        // Adjust current index if necessary
        if (m_CurrentAtlasIndex >= atlases.size() && !atlases.empty())
        {
            m_CurrentAtlasIndex = atlases.size() - 1;
        }
        else if (atlases.empty())
        {
            m_CurrentAtlasIndex = 0;
        }

        m_Context->GetProject().MarkAsModified();
    }

    void PanelTextureSelection::SetCurrentAtlasIndex(size_t index)
    {
        auto& atlases = m_Context->GetProject().GetTextureAtlases();
        if (index < atlases.size())
        {
            m_CurrentAtlasIndex = index;
        }
    }

    bool PanelTextureSelection::HasValidCurrentAtlas() const
    {
        auto& atlases = m_Context->GetProject().GetTextureAtlases();
        return m_CurrentAtlasIndex < atlases.size();
    }
}