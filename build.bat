@echo off
setlocal

set input=../source/rocket.cpp
set output=rocket.exe
set defines=-D _CRT_SECURE_NO_WARNINGS=1 -D CS_PLATFORM_SDL2=1 -D CS_COMPILER_MSVC=1 -D CS_DEBUG=1 -D SDL_MAIN_HANDLED=1
set idirs=-I ../source -I ../source/cstech -I ../depends/sdl2/include -I ../depends/sdl2_mixer/include -I ../depends/glew/include -I ../depends/glew/source -I ../depends/imgui -I ../depends/stb -I ../depends/nlohmann -I ../depends/csmath
set ldirs=-libpath:../depends/sdl2/lib/win32 -libpath:../depends/sdl2_mixer/lib/win32
set libs=SDL2main.lib SDL2.lib SDL2_mixer.lib opengl32.lib

copy depends\sdl2\bin\win32\*.dll binary\
copy depends\sdl2_mixer\bin\win32\*.dll binary\

pushd binary
cl -EHsc -std:c++17 %input% %defines% %idirs% -Fe:%output% -link %ldirs% %libs%
popd

endlocal
