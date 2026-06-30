@echo off

pushd ..
Tiles\vendor\premake\premake5.exe --file=Build-Tiles.lua vs2022
popd
pause