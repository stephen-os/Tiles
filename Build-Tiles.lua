-- premake5.lua
workspace "Tiles"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TilesEditor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "Tiles/vendor/imgui"
   include "Tiles/vendor/glfw"
   include "Tiles/vendor/glad"
   include "Tiles/vendor/imguifd"
group ""

group "Engine"
   include "Tiles"
group ""

group "Tools"
   include "TilesEditor"
group ""