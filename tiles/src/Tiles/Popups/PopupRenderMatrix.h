#pragma once
#include "Popup.h"
#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace Tiles
{
    enum class ExportFormat
    {
        PNG = 0,
        JPG,
        BMP,
        TGA
    };

    class PopupRenderMatrix : public Popup
    {
    public:
        PopupRenderMatrix(Ref<Context> context);
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
        void ExecuteExport();
        std::vector<std::string> GetUsedRenderGroups() const;
        std::string GetExportFileName(int groupIndex = -1) const;
        std::filesystem::path GetFullExportPath(const std::string& fileName) const;
        const char* GetFormatExtension() const;
        void ExportRenderGroup(const std::vector<size_t>& layerIndices, const std::filesystem::path& fileName);
        void InitializeDialog();
        void ApplyRenderGroupChanges();

    private:
        std::string m_ExportFileName = "export";
        std::filesystem::path m_ExportDirectory = ".";
        ExportFormat m_ExportFormat = ExportFormat::PNG;
        std::map<size_t, int> m_LayerToRenderGroup;
        bool m_ExportVisible = true;
        bool m_ExportInvisible = false;
        bool m_UseProjectBounds = true;
        bool m_ShowDirectorySelector = false;
        bool m_FirstShow = true;

        static constexpr int RENDER_GROUPS = 8;
    };
}