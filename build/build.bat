@echo off
rem cmake -G "Visual Studio 14 2015 Win64" ..
devenv /build Release alrm_graphics.sln
cd assign01\Release
assign01.exe
rem cd lab03
rem Release\lab03.exe