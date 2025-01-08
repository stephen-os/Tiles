@echo off

pushd ..
Lumina\Dependencies\premake\premake5.exe --file=Build-Tiles.lua vs2022
popd
pause