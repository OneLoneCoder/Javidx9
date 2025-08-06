# Makefile to compile the examples on linux
# make will generate temporary linux compatible cpp files and build them into executables
# 
# Tested on Ubuntu 18.04 with:
# sudo apt install libsdl2-dev
# make
#
# (C) 2019 Giampiero Salvi <giampisalvi@gmail.com>
# LICENSE: GPL 3

MAKEFLAGS += --no-builtin-rules
# most programs work with c++11, but OneLoneCoder_Mazes requires 14 for the period definition
CPPFLAGS=-DUNICODE -I/usr/include/SDL2 -I.. -std=c++14
LDFLAGS=
LDLIBS=-lSDL2 -lpthread

# Not working yet:
# OneLoneCoder_AR_OpticFlow (missing escapi.h)
# OneLoneCoder_Balls2 (missing function max)
# OneLoneCoder_CaveDiver (imports Windows.h)
# OneLoneCoder_CommandLineFPS (imports Windows.h)
# OneLoneCoder_FlappyBird (imports Windows.h)
# OneLoneCoder_LogitechG13Twitch (winsock2.h)
# OneLoneCoder_LudumDare42 (missing function max)
# OneLoneCoder_olcEngine3D_Part1-4 strange errors, haven't checked
# OneLoneCoder_PanAndZoom (missing to_wstring)
# OneLoneCoder_RacingLines (missing FillTriangle)
# OneLoneCoder_Snake (imports Windows.h)
# OneLoneCoder_Tetris (imports Windows.h)
# OneLoneCoder_Webcam (missing escapi.h)
# worms/OneLoneCoder_Worms1-3 (missing escapi.h)

# these are working
EXAMPLES= \
	OneLoneCoder_Asteroids \
	OneLoneCoder_Balls1 \
	OneLoneCoder_ComandLineFPS_2 \
	OneLoneCoder_Frogger \
	OneLoneCoder_GameOfLife \
	OneLoneCoder_Matrix \
	OneLoneCoder_Mazes \
	OneLoneCoder_OverEngineered \
	OneLoneCoder_PathFinding_AStar \
	OneLoneCoder_PerlinNoise \
	OneLoneCoder_PlatformGame1 \
	OneLoneCoder_Pseudo3DPlanesMode7 \
	OneLoneCoder_RetroArcadeRacer \
	OneLoneCoder_Splines1 \
	OneLoneCoder_Splines2 \
	OneLoneCoder_SpriteEditor

all: $(EXAMPLES)

%_linux.cpp: ../%.cpp
	cat $< | sed 's/olcConsoleGameEngine.h/olcConsoleGameEngineSDL.h/g' | sed 's/VK_SHIFT/VK_LSHIFT/g' | sed 's/VK_CONTROL/VK_LCONTROL/g' > $@

%: %_linux.cpp
	g++ $< $(CPPFLAGS) -o $@ $(LDLIBS)

