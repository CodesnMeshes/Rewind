@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

set ROOT=%~dp0..\
set LRCPP=%ROOT%third-party\lrcpp
set ENGINE=%ROOT%src\engine
set OUT=%ROOT%build

if not exist "%OUT%" mkdir "%OUT%"

cl.exe /nologo /EHsc /std:c++17 /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /LD ^
  /I"%LRCPP%\include" /I"%ENGINE%" ^
  "%ENGINE%\RewindCore.cpp" ^
  "%LRCPP%\src\Components.cpp" ^
  "%LRCPP%\src\CoreFsm.cpp" ^
  "%LRCPP%\src\Frontend.cpp" ^
  /Fo"%OUT%\\" ^
  /Fe"%OUT%\RewindCore.dll"

echo BUILD_EXIT_CODE=%ERRORLEVEL%
