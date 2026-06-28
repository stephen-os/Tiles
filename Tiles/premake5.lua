project "Tiles"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

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
      "vendor/json"
   }

    links
    {
        "ImGui",
        "GLFW",
        "Glad",
        "ImGuiFileDialog",
        "opengl32.lib"
    }

    
   buildoptions { "/utf-8" }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

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