-- premake5.lua
workspace "Tiles"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TilesEditor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "Tiles/builds/imgui"
   include "Tiles/builds/glfw"
   include "Tiles/builds/glad"
   include "Tiles/builds/imguifd"
group ""

group "Engine"
   include "Tiles"
group ""

group "Tools"
   include "TilesEditor"
group ""