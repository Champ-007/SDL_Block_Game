@echo off
REM Add the dll folder to PATH so Windows can find SDL2 and its dependencies
set PATH=%~dp0dll;%PATH%
REM Run the executable
%~dp0main.exe