# Linux Makefile
The is a Makefile for GNU Linux to build the examples in this repository. It has been tested on Ubuntu 18.04. Running make will create temporary cpp files from the original files, that have been modified to work with Linux. Main differences are using `olcConsoleGameEngineSDL.h` instead of `olcConsoleGameEngine.h` and substituting the `VK_SHIFT` and `VK_CONTROL` flags. Look at the Makefile to know which programs will not build, yet, and why.

## Dependencies
Install the development files for the SDL library:
```
sudo apt install libsdl2-dev
```

## Build
```
make
```