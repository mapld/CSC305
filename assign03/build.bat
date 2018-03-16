@echo off
if not defined DevEnvDir (
    call vcvars32.bat
)
mkdir Debug
set IMGUI_DIR=../external/imgui
set SDL2_DIR=../external/SDL
set GL3W_DIR=../external/gl3w
set GLM_DIR=../external/GLM
cl /nologo /Zi /MD /I ../general /I include /I %GLM_DIR% /I %IMGUI_DIR% /I %GL3W_DIR% /I %SDL2_DIR%\include src\main.cpp ..\general\imgui_impl_sdl_gl3.cpp %IMGUI_DIR%\imgui*.cpp  %GL3W_DIR%\GL\gl3w.c /FeDebug/assign03.exe /FoDebug/ /link /libpath:%SDL2_DIR%\lib\x86 SDL2.lib SDL2main.lib SDL2_image.lib opengl32.lib /subsystem:console
