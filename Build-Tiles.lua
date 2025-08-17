-- premake5.lua
workspace "Tiles"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Tiles"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "lumina/lumina-external.lua"

group "App"
   include "tiles"
group ""