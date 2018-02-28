@echo off
if not defined DevEnvDir (
    call vcvarsall.bat
)
mkdir Debug
set IMGUI_DIR=../external/imgui
set SDL2_DIR=../external/SDL
set GL3W_DIR=../external/gl3w
cl /nologo /Zi /MD /I include /I %IMGUI_DIR% /I %GL3W_DIR% /I %SDL2_DIR%\include src\main.cpp src\imgui_impl_sdl_gl3.cpp %IMGUI_DIR%\imgui*.cpp  %GL3W_DIR%\GL\gl3w.c /FeDebug/assign02.exe /FoDebug/ /link /libpath:%SDL2_DIR%\lib\x86 SDL2.lib SDL2main.lib opengl32.lib /subsystem:console
