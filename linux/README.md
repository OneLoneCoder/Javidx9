# Linux Makefile
This is a Makefile to build the examples in this repository on GNU Linux. It has been tested on Ubuntu 18.04. Running make will create temporary cpp files from the original files, that have been modified to work with Linux. Main differences are using `olcConsoleGameEngineSDL.h` instead of `olcConsoleGameEngine.h` and substituting the `VK_SHIFT` and `VK_CONTROL` flags. Look at the Makefile to know which programs will not build, yet, and why.

## Dependencies
Install the development files for the SDL library:
```
sudo apt install libsdl2-dev
```

## Build
```
make
```
## Run
You need to run the programs from the top directory of the repository in order to find the additional files, such as sprites, for example:
```
cd ..
linux/OneLoneCoder_Asteroids
```
