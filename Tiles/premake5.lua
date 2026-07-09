project "Tiles"
   kind "StaticLib"
   language "C++"
   cppdialect "C++23"
   staticruntime "off"

   multiprocessorcompile "On"

   files { "src/**.h", "src/**.cpp", "vendor/miniz/miniz.c", "vendor/miniz/miniz.h" }

   includedirs
   {
      "src/Tiles",

      "vendor/imgui",
      "vendor/glfw/include",
      "vendor/glm",
      "vendor/glad/include",
      "vendor/imguifd",
      "vendor/spdlog/include",
      "vendor/stb",
      "vendor/json",
      "vendor/miniz"
   }

    links
    {
        "ImGui",
        "GLFW",
        "Glad",
        "ImGuiFD",
        "opengl32.lib"
    }


   buildoptions { "/utf-8" }

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "TILES_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "TILES_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "TILES_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "TILES_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
