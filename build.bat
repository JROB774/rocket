@echo off
setlocal

if "%~1"=="win32" goto build_win32
if "%~1"=="web" goto build_web

echo please specify a platform to build for (win32 or web)...
goto end

:build_win32
echo ----------------------------------------

if not exist binary\SDL2.dll copy depends\sdl\bin\win32\SDL2.dll binary\SDL2.dll > NUL
if not exist binary\SDL2_mixer.dll copy depends\sdl_mixer\bin\win32\SDL2_mixer.dll binary\SDL2_mixer.dll > NUL
if not exist binary\libogg-0.dll copy depends\sdl_mixer\bin\win32\libogg-0.dll binary\libogg-0.dll > NUL
if not exist binary\libvorbis-0.dll copy depends\sdl_mixer\bin\win32\libvorbis-0.dll binary\libvorbis-0.dll > NUL
if not exist binary\libvorbisfile-3.dll copy depends\sdl_mixer\bin\win32\libvorbisfile-3.dll binary\libvorbisfile-3.dll > NUL

set defs=-D BUILD_DEBUG -D SDL_MAIN_HANDLED
set idir=-I ../source -I ../depends/sdl/include -I ../depends/sdl_mixer/include -I ../depends/glew/include -I ../depends/glew/source -I ../depends/stb -I ../depends/nksdk/nklibs
set ldir=-libpath:../depends/sdl/lib/win32 -libpath:../depends/sdl_mixer/lib/win32
set libs=SDL2main.lib SDL2.lib SDL2_mixer.lib opengl32.lib shell32.lib
set cflg=-EHsc -std:c++17
set lflg=-incremental:no

if "%~2"=="release" (
    set defs=
    set cflg=%cflg% -O2
    set lflg=%lflg% -release -subsystem:windows
)

pushd binary
cl ../source/rocket.cpp %cflg% %defs% %idir% -Fe:rocket.exe -link %lflg% %ldir% %libs%
popd

goto end

:build_web
echo ----------------------------------------
:: @Incomplete: ...
goto end

:end

endlocal
