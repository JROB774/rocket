@echo off
setlocal

set input=../source/rocket.cpp
set output=rocket.exe
set defines=-D BUILD_DEBUG=1
set idirs=-I ../source -I ../depends/sdl/include -I ../depends/sdl_mixer/include -I ../depends/glew/include -I ../depends/glew/source -I ../depends/stb -I ../depends/nksdk/nklibs
set ldirs=-libpath:../depends/sdl/lib/win32 -libpath:../depends/sdl_mixer/lib/win32
set libs=SDL2main.lib SDL2.lib SDL2_mixer.lib opengl32.lib

copy depends\sdl\bin\win32\*.dll binary\
copy depends\sdl_mixer\bin\win32\*.dll binary\

pushd binary
cl -EHsc -std:c++17 %input% %defines% %idirs% -Fe:%output% -link %ldirs% %libs%
popd

endlocal
