# Rocket Rumble

A small arcade game released in 2015. The goal of the game is to dodge asteroids whilst flying through
space, the longer you survive the higher your end score will be. Reaching certain milestones will also
unlock new cosmetic costumes for your rocket which you can use whilst you play. A download for the game
can be found over on **[itch.io](https://jrob774.itch.io/rocket-rumble)** where there is also a browser
version available.

## Development

The game was developed in C++ using **[SDL2](https://libsdl.org/)**, with the web port being developed
using **[emscripten](https://github.com/emscripten-core/emscripten)**.

## Building

After cloning the repository you will need to run `git submodule update --init --recursive` to setup submodules.

The game can then be built by running the `build.bat` script with a build profile as the first argument. The
following build profiles are available:

- `win32` which builds the Windows version of the game.
- `web` which builds the web version of the game.
- `tools` which builds auxiliary tools used for development.

The Windows build also accepts an extra argument `release` which can be used to build the optimized release
executable of the game.

## License

The project's code is available under the **[MIT License](https://github.com/JROB774/rocket/blob/master/LICENSE)**.
