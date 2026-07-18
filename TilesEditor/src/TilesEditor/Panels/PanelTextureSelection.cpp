#include "PanelTextureSelection.h"
#include "../UIConstants.h"
#include "../UI/Theme.h"
#include "../UI/Widgets.h"
#include "ImGuiFileDialog.h"
#include "Core/Logger.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string>

namespace Tiles::Editor
{
    PanelTextureSelection::PanelTextureSelection(EditorHost& host) : Panel(host)
    {
        m_CheckerboardTexture = Tiles::Texture::Create(AssetPath::Checkerboard);
    }

    // Draws the panel: an add-atlas toolbar over a stack of per-atlas sections.
    void PanelTextureSelection::Render()
    {
        ImGui::Begin("Texture Selection", OpenFlag());
        ImGui::PushID("TextureSelection");

        RenderBlockToolbar();
        ImGui::Separator();
        RenderBlockAtlasList();
        RenderBlockFileDialog();

        ImGui::PopID();
        ImGui::End();
    }

    // No per-frame state to advance.
    void PanelTextureSelection::Update()
    {
    }

    // The top toolbar: a single control to append a new blank atlas.
    void PanelTextureSelection::RenderBlockToolbar()
    {
        if (UI::Button("Add Atlas", UI::ButtonVariant::Primary))
            AddNewAtlas();
    }

    // Renders one collapsible section per atlas, then applies a click-to-remove
    // after the loop so the atlas vector is never mutated mid-iteration.
    void PanelTextureSelection::RenderBlockAtlasList()
    {
        auto& atlases = Ctx().GetProject()->GetTextureAtlases();
        if (atlases.empty())
        {
            UI::TextMuted("No texture atlases. Click 'Add Atlas' to create one.");
            return;
        }

        AtlasId toRemove = AtlasId::Invalid;
        for (size_t i = 0; i < atlases.size(); ++i)
        {
            if (RenderComponentAtlasHeader(i))
                toRemove = atlases[i]->GetId();
        }

        // Resolve the id back to its current position and remove it. Ids are stable,
        // so this is unambiguous even though other atlases may have shifted.
        if (toRemove != AtlasId::Invalid)
        {
            for (size_t i = 0; i < atlases.size(); ++i)
            {
                if (atlases[i]->GetId() == toRemove)
                {
                    Ctx().GetProject()->RemoveTextureAtlas(i);
                    break;
                }
            }
        }

        // Commit a drag-select on release, wherever the pointer ended up.
        if (m_Selecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            for (size_t i = 0; i < atlases.size(); ++i)
            {
                if (atlases[i]->GetId() == m_SelectAtlasId)
                {
                    FinalizeSelection(i);
                    break;
                }
            }
            m_Selecting = false;
        }
    }

    // One atlas section: a header (its display name) over rename, image, dimensions,
    // the tile grid, and a remove control. Returns true if remove was requested.
    bool PanelTextureSelection::RenderComponentAtlasHeader(size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        // Scope every child id to this atlas so identical labels never collide.
        ImGui::PushID(static_cast<int>(atlas->GetId()));

        // "###" keeps the header's id stable while its visible name changes.
        std::string header = AtlasDisplayName(*atlas, atlasIndex) + "###AtlasHeader";
        bool removeRequested = false;

        if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();

            RenderSectionAtlasName(atlasIndex);
            RenderSectionAtlasImage(atlasIndex);
            RenderSectionAtlasDimensions(atlasIndex);

            // Bound the grid's height so multiple atlas sections stack instead of one
            // filling the panel; the child scrolls when the atlas is taller.
            float tileSize = CalculateDynamicTileSize(ImGui::GetContentRegionAvail().x, atlas->GetWidth());
            float naturalHeight = atlas->GetHeight() * tileSize;
            float gridHeight = std::min(naturalHeight, TextureConstants::Panel::MaxGridHeight);

            ImGui::BeginChild("##TextureGrid", ImVec2(0, gridHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
            RenderSectionTextureGrid(atlasIndex, tileSize);
            ImGui::EndChild();

            if (UI::Button("Remove Atlas", UI::ButtonVariant::Danger))
                removeRequested = true;

            ImGui::Unindent();
        }

        ImGui::PopID();
        return removeRequested;
    }

    // An inline rename field; an empty name shows the positional fallback as a hint.
    void PanelTextureSelection::RenderSectionAtlasName(size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        char buffer[TextureConstants::Panel::NameBufferSize];
        std::snprintf(buffer, sizeof(buffer), "%s", atlas->GetName().c_str());

        std::string hint = "Atlas " + std::to_string(atlasIndex + 1);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputTextWithHint("##AtlasName", hint.c_str(), buffer, sizeof(buffer)))
        {
            atlas->SetName(buffer);
            Ctx().GetProject()->MarkAsModified();
        }
    }

    // The atlas image row: browse when unset, otherwise show the file + a clear button.
    void PanelTextureSelection::RenderSectionAtlasImage(size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Image:");
        ImGui::SameLine();

        if (!atlas->HasImage())
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("[ none ]");
            ImGui::SameLine();

            if (UI::Button("Browse..."))
                OpenFileDialog(atlasIndex);
        }
        else
        {
            RenderComponentFilePathDisplay(atlas->GetSourcePath());
            ImGui::SameLine();

            if (UI::Button("Clear"))
            {
                atlas->RemoveImage();
                Ctx().GetProject()->MarkAsModified();
            }
        }
    }

    // Shows just the file name of a stored atlas path, wrapped in brackets.
    void PanelTextureSelection::RenderComponentFilePathDisplay(const std::string& path)
    {
        ImGui::AlignTextToFramePadding();
        std::string filename = path.substr(path.find_last_of("/\\") + 1);
        ImGui::TextWrapped("[ %s ]", filename.c_str());
    }

    // Width/height cell counts; a change re-slices the atlas's UV grid.
    void PanelTextureSelection::RenderSectionAtlasDimensions(size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        ImGui::PushItemWidth(UI::Component::InputWidth);

        int width = atlas->GetWidth();
        ImGui::InputInt("Width##AtlasWidth", &width);
        if (width != atlas->GetWidth())
        {
            atlas->Resize(std::max(1, width), atlas->GetHeight());
            Ctx().GetProject()->MarkAsModified();
        }

        int height = atlas->GetHeight();
        ImGui::InputInt("Height##AtlasHeight", &height);
        if (height != atlas->GetHeight())
        {
            atlas->Resize(atlas->GetWidth(), std::max(1, height));
            Ctx().GetProject()->MarkAsModified();
        }

        ImGui::PopItemWidth();
    }

    // Lays out the atlas's cells as a clickable grid at the given cell size.
    void PanelTextureSelection::RenderSectionTextureGrid(size_t atlasIndex, float tileSize)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        // Seamless tiles: no rounding, no spacing, no frame padding.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        for (int y = 0; y < atlas->GetHeight(); y++)
        {
            for (int x = 0; x < atlas->GetWidth(); x++)
            {
                // Checkerboard behind the cell shows transparency.
                ImVec2 currentPos = ImGui::GetCursorScreenPos();
                ImVec2 minPos = currentPos;
                ImVec2 maxPos = ImVec2(currentPos.x + tileSize, currentPos.y + tileSize);

                if (m_CheckerboardTexture)
                {
                    auto checkerboardID = static_cast<ImTextureID>(m_CheckerboardTexture->GetID());
                    ImGui::GetWindowDrawList()->AddImage(checkerboardID, minPos, maxPos, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
                }

                int index = y * atlas->GetWidth() + x;
                std::string itemId = "GridItem_" + std::to_string(index);
                RenderComponentTextureGridItem(itemId.c_str(), index, atlasIndex, tileSize);

                // Drag-to-select: anchor on press over a cell, extend to the hovered
                // cell while the button is held. Commit happens on release (see list).
                if (ImGui::IsItemHovered())
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        m_Selecting = true;
                        m_SelectAtlasId = atlas->GetId();
                        m_SelectAnchor = { x, y };
                        m_SelectCurrent = { x, y };
                    }
                    else if (m_Selecting && m_SelectAtlasId == atlas->GetId() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        m_SelectCurrent = { x, y };
                    }
                }

                // Keep cells on one row until the atlas width wraps.
                if ((index + 1) % atlas->GetWidth() != 0)
                    ImGui::SameLine();
            }
        }

        ImGui::PopStyleVar(4);
    }

    // One grid cell: the atlas sub-image (or a transparent button for an imageless
    // atlas), click-to-select, plus its selection border.
    void PanelTextureSelection::RenderComponentTextureGridItem(const char* id, int index, size_t atlasIndex, float tileSize)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);
        ImVec2 buttonSize(tileSize, tileSize);

        auto texture = Host().GetAtlasTexture(*atlas);
        if (texture)
        {
            glm::vec4 texCoords = atlas->GetTextureCoords(index);
            ImVec2 uvMin(texCoords.x, texCoords.y);
            ImVec2 uvMax(texCoords.z, texCoords.w);
            auto textureID = static_cast<ImTextureID>(texture->GetID());

            ImGui::Image(textureID, buttonSize, uvMin, uvMax, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

            std::string buttonId = std::string("##") + id + "_Button";
            ImGui::Button(buttonId.c_str(), buttonSize);

            ImGui::PopStyleColor(3);
        }

        RenderComponentSelectionBorder(index, atlasIndex, tileSize);
    }

    // Outlines the cell: accent-thick when it is the brush's selected tile, a thin
    // surface line otherwise.
    void PanelTextureSelection::RenderComponentSelectionBorder(int index, size_t atlasIndex, float tileSize)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);
        auto& brush = Ctx().GetBrush();

        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();

        // While dragging or after committing a stamp, highlight the whole rectangle;
        // otherwise highlight the single brush cell.
        bool isSelected;
        if ((m_Selecting || m_HasStampSelection) && atlas->GetId() == m_SelectAtlasId)
        {
            int cx = index % atlas->GetWidth();
            int cy = index / atlas->GetWidth();
            int minX = std::min(m_SelectAnchor.x, m_SelectCurrent.x);
            int maxX = std::max(m_SelectAnchor.x, m_SelectCurrent.x);
            int minY = std::min(m_SelectAnchor.y, m_SelectCurrent.y);
            int maxY = std::max(m_SelectAnchor.y, m_SelectCurrent.y);
            isSelected = (cx >= minX && cx <= maxX && cy >= minY && cy <= maxY);
        }
        else
        {
            isSelected = (brush.GetAtlasId() == atlas->GetId() &&
                brush.GetCellIndex() == index &&
                brush.IsTextured());
        }

        float borderThickness = std::max(TextureConstants::Tile::MinBorderThickness,
            tileSize * TextureConstants::Tile::BorderThicknessRatio);

        ImU32 borderColor = isSelected ?
            ImGui::ColorConvertFloat4ToU32(UI::GetTheme().Accent) :
            ImGui::ColorConvertFloat4ToU32(UI::GetTheme().Surface);
        float thickness = isSelected ? borderThickness : TextureConstants::Tile::MinBorderThickness;

        ImGui::GetWindowDrawList()->AddRect(itemMin, itemMax, borderColor, 0.0f, 0, thickness);
    }

    // Displays the modal atlas-image file dialog and applies its result.
    void PanelTextureSelection::RenderBlockFileDialog()
    {
        ImVec2 dialogSize(UI::Dialog::FileDialogWidth, UI::Dialog::FileDialogHeight);

        if (ImGuiFileDialog::Instance()->Display(TextureConstants::FileDialog::DialogKey, ImGuiWindowFlags_NoCollapse, dialogSize))
        {
            HandleFileDialogResult();
        }
    }

    float PanelTextureSelection::CalculateDynamicTileSize(float availableWidth, int atlasWidth) const
    {
        if (atlasWidth <= 0)
        {
            return TextureConstants::Tile::PreferredSize;
        }

        // Calculate what tile size would fit the available width
        float widthBasedTileSize = availableWidth / atlasWidth;

        // Clamp to our min/max bounds
        float clampedSize = std::clamp(widthBasedTileSize, TextureConstants::Tile::MinSize, TextureConstants::Tile::MaxSize);

        // For small atlases, prefer larger tiles for visibility
        if (atlasWidth <= TextureConstants::Atlas::SmallThreshold)
        {
            return std::max(clampedSize, TextureConstants::Tile::PreferredSize);
        }

        // For medium atlases, use calculated size
        if (atlasWidth <= TextureConstants::Atlas::MediumThreshold)
        {
            return clampedSize;
        }

        // For large atlases, prefer smaller tiles but ensure minimum visibility
        return std::max(clampedSize, TextureConstants::Tile::MinSize);
    }

    // The atlas's name, or a positional "Atlas N" when it has none.
    std::string PanelTextureSelection::AtlasDisplayName(const Tiles::TextureAtlas& atlas, size_t atlasIndex) const
    {
        if (!atlas.GetName().empty())
            return atlas.GetName();

        return "Atlas " + std::to_string(atlasIndex + 1);
    }

    void PanelTextureSelection::HandleAtlasFileSelection(const std::string& newPath)
    {
        if (!std::filesystem::exists(newPath))
            return;

        auto atlas = Ctx().GetProject()->GetTextureAtlasById(m_DialogAtlasId);
        if (!atlas)
            return;

        std::filesystem::path relativePath = std::filesystem::relative(newPath, std::filesystem::current_path());
        atlas->SetImage(relativePath.string());
        Ctx().GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::HandleTextureSelection(int index, size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);
        auto& brush = Ctx().GetBrush();

        // Check if this texture is already selected
        bool isCurrentlySelected = (brush.GetAtlasId() == atlas->GetId() &&
            brush.GetCellIndex() == index &&
            brush.IsTextured());

        if (isCurrentlySelected)
        {
            // Deselect current texture
            Tile newBrush = brush;
            newBrush.SetTextured(false);
            newBrush.SetAtlasId(AtlasId::Invalid);
            Ctx().SetBrush(newBrush);
        }
        else
        {
            // Select new texture
            Tile newBrush = brush;
            newBrush.SetTextured(true);
            newBrush.SetAtlasId(atlas->GetId());
            newBrush.SetCellIndex(index);
            Ctx().SetBrush(newBrush);
        }

        Ctx().GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::FinalizeSelection(size_t atlasIndex)
    {
        auto atlas = Ctx().GetProject()->GetTextureAtlas(atlasIndex);

        int minX = std::min(m_SelectAnchor.x, m_SelectCurrent.x);
        int maxX = std::max(m_SelectAnchor.x, m_SelectCurrent.x);
        int minY = std::min(m_SelectAnchor.y, m_SelectCurrent.y);
        int maxY = std::max(m_SelectAnchor.y, m_SelectCurrent.y);
        int width = maxX - minX + 1;
        int height = maxY - minY + 1;

        // A single cell is the ordinary click-to-toggle brush, not a stamp.
        if (width == 1 && height == 1)
        {
            m_HasStampSelection = false;
            Ctx().ClearStamp();
            HandleTextureSelection(minY * atlas->GetWidth() + minX, atlasIndex);
            return;
        }

        // Build the M x N stamp from the current brush (transform / tint) plus the
        // selected atlas cells, row-major with row 0 the top row of the selection.
        Tile templateTile = Ctx().GetBrush();
        std::vector<Tile> tiles;
        tiles.reserve(static_cast<size_t>(width) * height);
        for (int r = 0; r < height; ++r)
            for (int c = 0; c < width; ++c)
            {
                Tile tile = templateTile;
                tile.SetPainted(true);
                tile.SetTextured(true);
                tile.SetAtlasId(atlas->GetId());
                tile.SetCellIndex((minY + r) * atlas->GetWidth() + (minX + c));
                tiles.push_back(tile);
            }

        Ctx().SetStamp(std::move(tiles), width, height);

        // Point the single brush at the top-left cell so the brush preview / attribute
        // panels still show a representative tile while a stamp is active.
        Tile origin = templateTile;
        origin.SetTextured(true);
        origin.SetAtlasId(atlas->GetId());
        origin.SetCellIndex(minY * atlas->GetWidth() + minX);
        Ctx().SetBrush(origin);

        m_HasStampSelection = true;
        Ctx().GetProject()->MarkAsModified();
    }

    void PanelTextureSelection::OpenFileDialog(size_t atlasIndex)
    {
        // Remember which atlas the browse is for, by stable id, before the dialog opens.
        m_DialogAtlasId = Ctx().GetProject()->GetTextureAtlas(atlasIndex)->GetId();

        IGFD::FileDialogConfig config;
        config.path = TextureConstants::FileDialog::DefaultPath;
        config.flags = ImGuiFileDialogFlags_Modal;
        config.countSelectionMax = 1;

        ImGuiFileDialog::Instance()->OpenDialog(
            TextureConstants::FileDialog::DialogKey,
            TextureConstants::FileDialog::DialogTitle,
            TextureConstants::FileDialog::FileFilters,
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
        auto newAtlas = Tiles::TextureAtlas::Create(TextureConstants::Atlas::DefaultWidth, TextureConstants::Atlas::DefaultHeight);
        Ctx().GetProject()->AddTextureAtlas(newAtlas);
    }
}
