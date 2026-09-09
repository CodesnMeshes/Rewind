@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

set ROOT=%~dp0..\
set SDL2=%ROOT%third-party\sdl2\SDL2-2.30.9
set LRCPP=%ROOT%third-party\lrcpp

if not exist "%ROOT%build" mkdir "%ROOT%build"

cl.exe /nologo /EHsc /std:c++17 /O2 /W3 /D_CRT_SECURE_NO_WARNINGS ^
  /I"%LRCPP%\include" /I"%LRCPP%\examples\sdl2" /I"%LRCPP%\examples\sdl2\win32compat" /I"%SDL2%\include" ^
  "%LRCPP%\examples\sdl2\Audio.cpp" ^
  "%LRCPP%\examples\sdl2\Config.cpp" ^
  "%LRCPP%\examples\sdl2\Entry.c" ^
  "%LRCPP%\examples\sdl2\Input.cpp" ^
  "%LRCPP%\examples\sdl2\Logger.cpp" ^
  "%LRCPP%\examples\sdl2\Main.cpp" ^
  "%LRCPP%\examples\sdl2\Perf.cpp" ^
  "%LRCPP%\examples\sdl2\Player.cpp" ^
  "%LRCPP%\examples\sdl2\Video.cpp" ^
  "%LRCPP%\examples\sdl2\Vfs.cpp" ^
  "%LRCPP%\src\Components.cpp" ^
  "%LRCPP%\src\CoreFsm.cpp" ^
  "%LRCPP%\src\Frontend.cpp" ^
  /Fo"%ROOT%build\\" ^
  /Fe"%ROOT%build\sdl2lrcpp.exe" ^
  /link /LIBPATH:"%SDL2%\lib\x64" SDL2.lib SDL2main.lib shell32.lib

echo BUILD_EXIT_CODE=%ERRORLEVEL%
