@echo off
setlocal

if "%~1"=="win32" goto build_win32
if "%~1"=="web" goto build_web
if "%~1"=="tools" goto build_tools

echo please specify a platform to build for (win32, web, tools)...
goto end

:build_tools
echo ----------------------------------------

if not exist tools mkdir tools

pushd tools
cl ../source/tools/packer.cpp -I ../depends/nksdk/nklibs -Fe:packer.exe
del *.obj
popd

goto end

:build_win32
echo ----------------------------------------

if not exist binary\win32 mkdir binary\win32

copy depends\sdl\bin\win32\*.dll binary\win32\ > NUL
copy depends\sdl_mixer\bin\win32\*.dll binary\win32\ > NUL
copy docs\asset_paths.txt binary\win32\asset_paths.txt > NUL
copy docs\credits.txt binary\win32\credits.txt > NUL

set defs=-D BUILD_NATIVE
set idir=-I ../../depends/sdl/include -I ../../depends/sdl_mixer/include -I ../../depends/glew/include -I ../../depends/glew/source -I ../../depends/stb -I ../../depends/nksdk/nklibs -I ../../depends/gon
set ldir=-libpath:../../depends/sdl/lib/win32 -libpath:../../depends/sdl_mixer/lib/win32
set libs=SDL2main.lib SDL2.lib SDL2_mixer.lib opengl32.lib shell32.lib
set cflg=-EHsc -std:c++17 -Zc:__cplusplus
set lflg=-incremental:no

if "%~2"=="release" (
    set cflg=%cflg% -O2
    set lflg=%lflg% -release -subsystem:windows
    tools\packer.exe
    echo.
) else (
    set defs=%defs% -D BUILD_DEBUG -D SDL_MAIN_HANDLED
    set cflg=%cflg% -Z7
)

pushd binary\win32
rc -i ../../redist/win32/res ../../redist/win32/res/icon.rc
cl ../../source/rocket.cpp %cflg% %defs% %idir% -Fe:rocket.exe -link %lflg% %ldir% %libs% ../../redist/win32/res/icon.res
popd

goto end

:build_web
echo ----------------------------------------

call depends\emsdk\emsdk install latest
call depends\emsdk\emsdk activate latest
call depends\emsdk\emsdk_env.bat

set defs=-D BUILD_WEB
set idir=-I ../../depends/stb -I ../../depends/nksdk/nklibs
set libs=-s WASM=1 -s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_OGG=1 -s USE_VORBIS=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -lidbfs.js
set cflg=-std=c++17
set lflg=--preload-file ../../assets -s EXPORTED_FUNCTIONS="['_main','_main_start']" -s EXPORTED_RUNTIME_METHODS="['ccall']"

if not exist binary\web mkdir binary\web

pushd binary\web
emcc %libs% %idir% %cflg% %lflg% ../../source/rocket.cpp -o rocket.html
popd

goto end

:end

endlocal
