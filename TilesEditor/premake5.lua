project "TilesEditor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++23"
   staticruntime "off"

   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",

      -- Tiles includes
      "../Tiles/src/Tiles",

      -- Vendor includes (from Tiles)
      "../Tiles/vendor/imgui",
      "../Tiles/vendor/glfw/include",
      "../Tiles/vendor/glm",
      "../Tiles/vendor/glad/include",
      "../Tiles/vendor/imguifd",
      "../Tiles/vendor/spdlog/include",
      "../Tiles/vendor/stb",
      "../Tiles/vendor/json"
   }

   links
   {
      "Tiles",
      "ImGui",
      "GLFW",
      "Glad",
      "ImGuiFD",
      "opengl32.lib"
   }

   buildoptions { "/utf-8" }

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   -- Assets load via cwd-relative paths (e.g. "res/shaders/Quad.vert"), so the
   -- debugger must run from TilesEditor/ where res/ lives, not the build dir.
   debugdir "%{wks.location}/TilesEditor"

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
      kind "WindowedApp"
      defines { "TILES_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
