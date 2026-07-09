project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    multiprocessorcompile "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "../../vendor/imgui/imgui.cpp",
        "../../vendor/imgui/imgui.h",
        "../../vendor/imgui/imgui_demo.cpp",
        "../../vendor/imgui/imgui_draw.cpp",
        "../../vendor/imgui/imgui_internal.h",
        "../../vendor/imgui/imgui_tables.cpp",
        "../../vendor/imgui/imgui_widgets.cpp",
        "../../vendor/imgui/imstb_rectpack.h",
        "../../vendor/imgui/imstb_textedit.h",
        "../../vendor/imgui/imstb_truetype.h",

        "../../vendor/imgui/backends/imgui_impl_glfw.cpp",
        "../../vendor/imgui/backends/imgui_impl_glfw.h",
        "../../vendor/imgui/backends/imgui_impl_opengl3.cpp",
        "../../vendor/imgui/backends/imgui_impl_opengl3.h",
        "../../vendor/imgui/backends/imgui_impl_opengl3_loader.h"
    }

    includedirs
    {
        "../../vendor/imgui",
        "../../vendor/glfw/include"
    }

    defines
    {
        "IMGUI_ENABLE_DOCKING"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
