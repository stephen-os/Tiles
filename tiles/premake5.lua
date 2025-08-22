project "Tiles"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "../lumina/lumina/src",

      "../lumina/dependencies/imgui",
      "../lumina/dependencies/glew/include",
      "../lumina/dependencies/glfw/include",
      "../lumina/dependencies/glm",
      "../lumina/dependencies/glad/include",
      "../lumina/dependencies/tinygltf",
      "../lumina/dependencies/imguifd",
      "../lumina/dependencies/spdlog/include"
   }

    links
    {
        "Lumina",
    }

    
   buildoptions { "/utf-8" }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "LUMINA_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "LUMINA_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "LUMINA_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "LUMINA_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"