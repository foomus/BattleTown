# Battle town
A Battle City clone written in C

## Features
* All types of blocks from the original game 
* All types of enemies from the original game
* A bit reworked GUI
* All power-ups from the original game
    * Power-up spawns aren't fixed like in the original game
    * Enemies can steal power-ups but can't use them
* Reworked enemy spawning
* 5 stages instead of 35 in the original one
    * Stage difficulty increases exponentially. The last levels are pretty much impossible. Go for 2nd or 3rd stage those are most balanced
* Bugs and crashes

# Controls
* Arrows -> Move, Select stage
* z, x -> Shoot
* Enter -> Next screen

## Fun fact
This project was made in August 2024 in around 36 hours of coding, debugging, and researching.

## Building from source
It was developed and tested only on Windows 10. Linux and macOS are unsupported, forks that add compatibility to those OSes are welcomed.

The `MinGW-w64` toolchain was used to compile and test the application. Use `mingw32-make` while in the src folder to build. No additional dependencies are required.

Why is SDL embedded directly into the repo? Well, it was just easier for me to set up the makefile this way.

## Libraries used
* [SDL2](https://github.com/libsdl-org/) [SDL2 LICENSE](./src/SDL2/LICENSE)

## License
[MIT License](LICENSE.md)