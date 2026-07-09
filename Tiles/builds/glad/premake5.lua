project "GLAD"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    -- Enable multi-core compilation (cross-platform)
    flags { "MultiProcessorCompile" }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "../../vendor/glad/include/glad/gl.h",
        "../../vendor/glad/include/KHR/khrplatform.h",
        "../../vendor/glad/src/gl.c"
    }

    includedirs
    {
        "../../vendor/glad/include"
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
