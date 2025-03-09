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

## Dependencies
* [SDL2](https://github.com/libsdl-org/) 

## Building from source
It was developed and tested only on Windows 10. Linux and macOS are unsupported.

The `MinGW-w64` toolchain was used to compile and test the application. Use `mingw32-make`, which comes with MinGW toolchain, to build.

## License
[MIT License](LICENSE.md)