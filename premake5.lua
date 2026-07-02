-- premake5.lua
workspace "Tiles"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TilesEditor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "Tiles/builds/imgui.lua"
   include "Tiles/builds/glfw.lua"
   include "Tiles/builds/glad.lua"
   include "Tiles/builds/imguifd.lua"
group ""

group "Engine"
   include "Tiles/builds/tiles.lua"
group ""

group "Tools"
   include "TilesEditor/builds/tileseditor.lua"
group ""