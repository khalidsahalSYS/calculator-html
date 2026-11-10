@echo off
setlocal

where cl >nul 2>nul
if errorlevel 1 (
  echo Run this script from an x64 Native Tools Command Prompt for Visual Studio.
  exit /b 1
)

if not exist build mkdir build
cl /nologo /O2 /W4 /TC /D_CRT_SECURE_NO_WARNINGS src\server.c src\calculator.c /Febuild\calculator.exe /link /SUBSYSTEM:WINDOWS ws2_32.lib shell32.lib
if errorlevel 1 exit /b %errorlevel%
copy /Y index.html build\index.html >nul
if errorlevel 1 exit /b %errorlevel%
echo Built build\calculator.exe. Keep index.html beside the executable.
