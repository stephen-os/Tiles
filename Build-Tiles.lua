-- premake5.lua
workspace "Tiles"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Tiles"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "Tiles/vendor/imgui"
   include "Tiles/vendor/glfw"
   include "Tiles/vendor/glad"
   include "Tiles/vendor/imguifd"
group ""

group "App"
   include "Tiles"
group ""