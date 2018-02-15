@echo off
cmake -G "Visual Studio 14 2015 Win64" ..
devenv /build Release alrm_graphics.sln

cd Examples\Ex1_Simple\Release
Ex1_Simple.exe
cd ../../..

rem cd lab03
rem Release\lab03.exe