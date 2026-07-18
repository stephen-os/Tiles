#pragma once
#include "Popup.h"
#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace Tiles::Editor
{
    enum class ExportFormat
    {
        PNG = 0,
        JPG,
        BMP,
        TGA
    };

    // Export dialog. Presents a matrix that assigns each layer to a render group,
    // then renders the layers of each used group to an image file. The pending
    // matrix is edited locally and only written back to the layers on Apply/Export.
    class PopupRenderMatrix : public Popup
    {
    public:
        PopupRenderMatrix(EditorHost& host);
        ~PopupRenderMatrix() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override;

    private:
        void RenderLayerMatrix();
        void RenderExportSettings();
        void RenderActionButtons();
        void ShowDirectoryDialog();
        void ResetToDefaults();

        // Renders each used render group to its own image file. A single used
        // group exports to the base file name; multiple groups get numbered suffixes.
        void ExecuteExport();

        // Distinct render-group ids (as strings) currently assigned to any layer,
        // excluding the unassigned sentinel (-1).
        std::vector<std::string> GetUsedRenderGroups() const;

        // Builds the output file name and extension; groupIndex >= 0 appends a
        // 1-based group suffix, -1 yields the un-suffixed base name.
        std::string GetExportFileName(int groupIndex = -1) const;
        std::filesystem::path GetFullExportPath(const std::string& fileName) const;
        const char* GetFormatExtension() const;

        // Renders the given layers into an offscreen target sized to the project
        // bounds and writes it to fileName. Returns true only if a file was written
        // (false on no content to draw or a failed write).
        [[nodiscard]] bool ExportRenderGroup(const std::vector<size_t>& layerIndices, const std::filesystem::path& fileName);

        void InitializeDialog();

        // Writes the locally edited matrix back onto the layers' render groups,
        // marking the project modified only if something actually changed.
        void ApplyRenderGroupChanges();

    private:
        // ImGui::InputText writes into this fixed buffer; its size is independent
        // of the string contents so the writable region is always well defined.
        static constexpr size_t ExportFileNameBufferSize = 256;
        char m_ExportFileNameBuffer[ExportFileNameBufferSize] = "export";
        std::filesystem::path m_ExportDirectory = ".";
        ExportFormat m_ExportFormat = ExportFormat::PNG;
        std::map<size_t, int> m_LayerToRenderGroup;
        bool m_ExportVisible = true;
        bool m_ExportInvisible = false;
        bool m_ShowDirectorySelector = false;
        bool m_ShowSuccessMessage = false;
        bool m_FirstShow = true;

        static constexpr int RENDER_GROUPS = 8;
    };
}