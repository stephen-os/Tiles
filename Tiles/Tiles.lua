project "Tiles"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "../Lumina/Lumina/src",

      "../Lumina/Dependencies/imgui",
      "../Lumina/Dependencies/glew/include",
      "../Lumina/Dependencies/glfw/include",
      "../Lumina/Dependencies/glm",
      "../Lumina/Dependencies/glad/include",
      "../Lumina/Dependencies/tinygltf",
      "../Lumina/Dependencies/imguifd",
      "../Lumina/Dependencies/spdlog/include"
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