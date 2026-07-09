project "ImGuiFD"
	kind "StaticLib"
	language "C++"
    cppdialect "C++20"
    staticruntime "off"

	-- Enable multi-core compilation
	flags { "MultiProcessorCompile" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"../../vendor/imguifd/ImGuiFileDialog.h",
		"../../vendor/imguifd/ImGuiFileDialog.cpp",

		"../../vendor/imguifd/ImGuiFileDialogConfig.h",

		"../../vendor/imguifd/stb/stb_image.h",
		"../../vendor/imguifd/stb/stb_image_resize2.h"
	}

	includedirs
	{
		"../../vendor/imgui",
		"../../vendor/imguifd"
	}

	links { "ImGui" }

	defines
    {
        "USE_THUMBNAILS",
        "DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION"
    }

 -- Platform-specific settings
    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        pic "On"
        systemversion "latest"

    filter "system:macosx"
        systemversion "latest"

    -- Configuration-specific settings
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        defines { "NDEBUG" }

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"
        symbols "Off"
        defines { "NDEBUG", "DIST_BUILD" }

    -- Clear filters
    filter {}
