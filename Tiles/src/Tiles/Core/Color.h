#pragma once

#include "imgui.h"

namespace Tiles
{
    struct Color
    {
        static constexpr ImU32 FILL_COLOR = IM_COL32(255, 255, 255, 255);               // White
        static constexpr ImU32 BACKGROUND_COLOR = IM_COL32(77, 77, 77, 255);            // Dark Gray
        static constexpr ImU32 OUTLINE_COLOR = IM_COL32(169, 169, 169, 255);            // Lighter Gray
        static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255);     // Orange
        static constexpr ImU32 DEAULT_BORDER_COLOR = IM_COL32(51, 51, 51, 255);         // Black

        static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255);     // Light Gray
        static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);        // Dark Gray
    };

    namespace Theme
    {

        static void ApplyTheme()
        {
            ImGui::StyleColorsDark();

            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 5.0f;                                            // Rounded corners for windows
            style.FrameRounding = 5.0f;                                             // Rounded corners for frames
            style.FramePadding = ImVec2(5.0f, 5.0f);                                // Padding within a frame
            style.ItemSpacing = ImVec2(8.0f, 6.0f);                                 // Spacing between items

            ImVec4* colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);             // Dark background
            colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);              // Frame background
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);       // Hovered frame background
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        // Active frame background
            colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);               // Button
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);        // Hovered button
            colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);         // Active button
            colors[ImGuiCol_Header] = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);               // Header (Orange)
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);        // Hovered header
            colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);         // Active header
            colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);            // Checkmark (Orange)
            colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);           // Slider grab
            colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);     // Active slider grab
            colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.4f, 0.1f, 1.0f);                  // Tab (Orange)
            colors[ImGuiCol_TabHovered] = ImVec4(0.9f, 0.5f, 0.2f, 1.0f);           // Hovered tab
            colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);            // Active tab
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);         // Unfocused tab
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);   // Unfocused active tab

            // Maintain compatibility with multiple viewports
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;                                        // Disable rounding for additional platform windows
                colors[ImGuiCol_WindowBg].w = 1.0f;                                 // Fully opaque background
            }

            // Load and set a custom font
            io.Fonts->Clear();  // Clear existing fonts if any

            ImFont* customFont = io.Fonts->AddFontFromFileTTF("res/assets/fonts/Modeseven-L3n5.ttf", 12.0f);
            if (!customFont)
                return;

            io.FontDefault = customFont;  // Set as default font
        }

    }
    
}