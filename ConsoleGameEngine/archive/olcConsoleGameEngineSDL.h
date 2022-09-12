/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9

For the *nix and Mac users - like most Linux software, this
may not be fully compatible or fully featured, and development
will invariably lag behind the rest - sorry.
(You thought, Javid, you thought)

I recommend compiling as follows, of course you need SDL from your vendor

 g++ thing.cpp -DUNICODE -I/usr/include/SDL2 -lSDL2 -lpthread -std=C++11

I'd like to explain how this version came about, because without the help of
some awesome individuals, this would not be a thing!

=========== IMPORTANT!!! PLEASE READ ==========================================

Firstly, Thanks to Jack Clarke (JackOJC on Discord and Github). He had the
idea of porting over the olcConsoleGameEngine to non-windows operating systems.

Also big thanks to Gorbit99 for adding audio facilities and tidying up a lot
of loose ends for this file. (Aww, haven't seen this 'til now)

He developed a prototype which can be found here:
https://github.com/Jackojc/olcNotSoConsoleGameEngine
which really proved that it could be possible.

I then added functionality to ensure that this header is 99% compatible with
my existing code base, so almost all the videos I've made should be compatible
with this version of the olcConsoleGameEngine, without modification.

Finally I'd also like to thank MrBadNews on Discord for testing and finding
many bugs, and creating workarounds, and Slavka on Discord for help debug SDL
double buffering issues - stupid SDL :P

Please remember I, javidx9, am not a Linux or SDL specialist. Therefore please
report bugs, or ask for help on the Discord Server, as I probably don't use
the same non-windows operating systems as you, and each have their own methods
for building and installing.

===============================================================================

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.

Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9

GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE

Cheers!

Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.

Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9

Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o

Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy

Last Updated: 15/01/2019

Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.

int main()
{
// Use olcConsoleGameEngine derived app
OneLoneCoder_Example game;

// Create a console with resolution 160x100 characters
// Each character occupies 8x8 pixels
game.ConstructConsole(160, 100, 8, 8);

// Start the engine!
game.Start();

return 0;
}


The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.

There may be bugs!

See my other videos for examples!
http://www.youtube.com/javidx9

Lots of programs to try:
http://www.github.com/OneLoneCoder/videos

Chat on the Discord server:
https://discord.gg/WhwHUMV

Be bored by Twitch:
http://www.twitch.tv/javidx9

*/

#include <fstream>

#pragma once

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! For now, Ill try enabling it for you - Javidx9
#define UNICODE
#define _UNICODE
#endif

#define __STDC_LIB_EXT1__ 
#define __STDC_WANT_LIB_EXT1__ 1
#define _CRT_SECURE_NO_WARNINGS

// NOTE: You will need to have SDL2 installed on your machine for this to work
// https://www.libsdl.org/download-2.0.php
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#undef main

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>

#define KEY_OFFSET 256
#define VK_SPACE (SDLK_SPACE & 0xffff) 			 + KEY_OFFSET
#define VK_BACK (SDLK_BACKSPACE & 0xffff)		  + KEY_OFFSET
#define VK_DELETE (SDLK_DELETE & 0xffff)		  + KEY_OFFSET
#define VK_HOME (SDLK_HOME & 0xffff)			  + KEY_OFFSET
#define VK_END (SDLK_END & 0xffff)				  + KEY_OFFSET
#define VK_RETURN (SDLK_RETURN & 0xffff)		  + KEY_OFFSET
#define VK_TAB (SDLK_TAB & 0xffff)				  + KEY_OFFSET
#define VK_INSERT (SDLK_INSERT & 0xffff)		  + KEY_OFFSET
#define VK_LSHIFT (SDLK_LSHIFT & 0xffff)		  + KEY_OFFSET
#define VK_RSHIFT (SDLK_RSHIFT & 0xffff)		  + KEY_OFFSET
#define VK_LCONTROL (SDLK_LCTRL & 0xffff)		  + KEY_OFFSET
#define VK_RCONTROL (SDLK_RCTRL & 0xffff)		  + KEY_OFFSET
#define VK_PRIOR (SDLK_PAGEUP & 0xffff)			  + KEY_OFFSET
#define VK_NEXT (SDLK_PAGEDOWN & 0xffff)		  + KEY_OFFSET
#define VK_ESCAPE (SDLK_ESCAPE & 0xffff)		  + KEY_OFFSET
#define VK_UP (SDLK_UP & 0xffff)				  + KEY_OFFSET
#define VK_DOWN	(SDLK_DOWN & 0xffff)			  + KEY_OFFSET
#define VK_LEFT (SDLK_LEFT & 0xffff)			  + KEY_OFFSET
#define VK_RIGHT (SDLK_RIGHT & 0xffff)			  + KEY_OFFSET
#define VK_F1 (SDLK_F1 & 0xffff)				  + KEY_OFFSET
#define VK_F2 (SDLK_F2 & 0xffff)				  + KEY_OFFSET
#define VK_F3 (SDLK_F3 & 0xffff)				  + KEY_OFFSET
#define VK_F4 (SDLK_F4 & 0xffff)				  + KEY_OFFSET
#define VK_F5 (SDLK_F5 & 0xffff)				  + KEY_OFFSET
#define VK_F6 (SDLK_F6 & 0xffff)				  + KEY_OFFSET
#define VK_F7 (SDLK_F7 & 0xffff)				  + KEY_OFFSET
#define VK_F8 (SDLK_F8 & 0xffff)				  + KEY_OFFSET
#define VK_F9 (SDLK_F9 & 0xffff)				  + KEY_OFFSET
#define VK_F10 (SDLK_F10 & 0xffff)				  + KEY_OFFSET
#define VK_F11 (SDLK_F11 & 0xffff)				  + KEY_OFFSET
#define VK_F12 (SDLK_F12 & 0xffff)				  + KEY_OFFSET
#define VK_MENU (SDLK_MENU & 0xffff)			  + KEY_OFFSET


struct CHAR_INFO
{
	unsigned short glyph;
	short colour;
};

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

constexpr SDL_Color colour_lookup[] = {
	SDL_Color{ 0,0,0,255 },      // 0
	SDL_Color{ 0,0,127,255 },    // 1
	SDL_Color{ 0,127,0,255 },    // 2
	SDL_Color{ 0,127,127,255 },  // 3
	SDL_Color{ 127,0,0,255 },    // 4
	SDL_Color{ 127,0,127,255 },  // 5
	SDL_Color{ 127,127,0,255 },  // 6
	SDL_Color{ 192,192,192,255 },// 7
	SDL_Color{ 127,127,127,255 },// 8
	SDL_Color{ 0,0,255,255 },    // 9
	SDL_Color{ 0,255,0,255 },    // A
	SDL_Color{ 0,255,255,255 },  // B
	SDL_Color{ 255,0,0,255 },    // C
	SDL_Color{ 255,0,255,255 },  // D
	SDL_Color{ 255,255,0,255 },  // E
	SDL_Color{ 255,255,255,255 },// F
};

class olcSprite
{
public:
	olcSprite()
	{

	}

	olcSprite(int w, int h)
	{
		Create(w, h);
	}

	olcSprite(std::wstring sFile)
	{
		if (!Load(sFile))
			Create(8, 8);
	}

	~olcSprite() {
		delete[] m_Glyphs;
		delete[] m_Colours;
	}

	int nWidth = 0;
	int nHeight = 0;

private:
	unsigned short *m_Glyphs = nullptr;
	short *m_Colours = nullptr;

	void Create(int w, int h)
	{
		nWidth = w;
		nHeight = h;
		m_Glyphs = new unsigned short[w*h];
		m_Colours = new short[w*h];
		for (int i = 0; i < w*h; i++)
		{
			m_Glyphs[i] = L' ';
			m_Colours[i] = FG_BLACK;
		}
	}

public:
	void SetGlyph(int x, int y, wchar_t c)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Glyphs[y * nWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Colours[y * nWidth + x] = c;
	}

	wchar_t GetGlyph(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return L' ';
		else
			return m_Glyphs[y * nWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[y * nWidth + x];
	}

	wchar_t SampleGlyph(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return L' ';
		else
			return m_Glyphs[sy * nWidth + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight - 1.0f);
		if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[sy * nWidth + sx];
	}

	bool Save(std::wstring sFile)
	{
		std::string path(sFile.begin(), sFile.end());

#ifdef __APPLE__
		std::ofstream f(path.c_str(), std::ios::out | std::ios::binary);
#else
		std::ofstream f(sFile.c_str(), std::ios::out | std::ios::binary);
#endif
		if (!f.is_open()) return false;

		f.write((char*)&nWidth, sizeof(int));
		f.write((char*)&nHeight, sizeof(int));
		f.write((char*)m_Colours, sizeof(short) * nWidth * nHeight);
		f.write((char*)m_Glyphs, sizeof(wchar_t) * nWidth * nHeight);

		f.close();

		return true;
	}

	bool Load(std::wstring sFile)
	{
		delete[] m_Glyphs;
		delete[] m_Colours;
		nWidth = 0;
		nHeight = 0;

		std::string path(sFile.begin(), sFile.end());

#ifdef __APPLE__
		std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
#else
		std::ifstream f(sFile.c_str(), std::ios::in | std::ios::binary);
#endif
		if (!f.is_open()) return false;

		// get file data
		f.read((char*)&nWidth, sizeof(int));
		f.read((char*)&nHeight, sizeof(int));

		Create(nWidth, nHeight);

		f.read((char*)m_Colours, sizeof(short) * nWidth * nHeight);
		f.read((char*)m_Glyphs, sizeof(wchar_t) * nWidth * nHeight);

		f.close();

		return true;
	}
};

int len = 0, done = 0, bits = 0, which = 0,
sample_size = 0, position = 0, rate = 0;
Sint16 *stream[2];

class olcConsoleGameEngine
{
public:
	olcConsoleGameEngine()
	{
		m_nScreenWidth = 80;
		m_nScreenHeight = 30;


		memset(m_keyNewState, 0, 512 * sizeof(bool));
		memset(m_keyOldState, 0, 512 * sizeof(bool));
		memset(m_keys._state, 0, 512 * sizeof(sKeyState));
		m_mousePosX = 0;
		m_mousePosY = 0;

		m_sAppName = L"Default";
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{
		m_nScreenWidth = width;
		m_nScreenHeight = height;
		m_nFontWidth = fontw;
		m_nFontHeight = fonth;




		// Allocate memory for screen buffer
		m_bufScreen[0] = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
		m_bufScreen[1] = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
		// NOTE(MrBadNewS): set buffers to zero, old way
		memset(m_bufScreen[0], 0, m_nScreenWidth*m_nScreenHeight * sizeof(CHAR_INFO));
		memset(m_bufScreen[1], 0, m_nScreenWidth*m_nScreenHeight * sizeof(CHAR_INFO));
		m_nCurrentBuffer = 0;

		return 1;
	}

	virtual void Draw(int x, int y, wchar_t c = PIXEL_SOLID, short col = 0x000F)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x].glyph = c;
			m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x].colour = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, wchar_t c = PIXEL_SOLID, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawString(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x + i].glyph = c[i];
			m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x + i].colour = col;
		}
	}

	void DrawStringAlpha(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			if (c[i] != L' ')
			{
				m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x + i].glyph = c[i];
				m_bufScreen[m_nCurrentBuffer][y * m_nScreenWidth + x + i].colour = col;
			}
		}
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= m_nScreenWidth) x = m_nScreenWidth;
		if (y < 0) y = 0;
		if (y >= m_nScreenHeight) y = m_nScreenHeight;
	}

	void DrawLine(int x1, int y1, int x2, int y2, wchar_t c = PIXEL_SOLID, short col = 0x000F)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = abs(dx);
		dy1 = abs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			Draw(x, y, c, col);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						y = y + 1;
					else
						y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			Draw(x, y, c, col);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						x = x + 1;
					else
						x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
	}

	void DrawCircle(int xc, int yc, int r, wchar_t c = PIXEL_SOLID, short col = 0x000F)
	{
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		while (y >= x) // only formulate 1/8 of circle
		{
			Draw(xc - x, yc - y, c, col);//upper left left
			Draw(xc - y, yc - x, c, col);//upper upper left
			Draw(xc + y, yc - x, c, col);//upper upper right
			Draw(xc + x, yc - y, c, col);//upper right right
			Draw(xc - x, yc + y, c, col);//lower left left
			Draw(xc - y, yc + x, c, col);//lower lower left
			Draw(xc + y, yc + x, c, col);//lower lower right
			Draw(xc + x, yc + y, c, col);//lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void FillCircle(int xc, int yc, int r, wchar_t c = PIXEL_SOLID, short col = 0x000F)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	};

	void DrawSprite(int x, int y, olcSprite *sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (sprite->GetGlyph(i + ox, j + oy) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
			}
		}
	}

	void DrawWireFrameModel(const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
		}
	}

	~olcConsoleGameEngine()
	{

	}

public:
	void Start()
	{
		m_bAtomActive = true;

		// Start the thread
		//std::thread t = std::thread(&olcConsoleGameEngine::GameThread, this);
		GameThread();
		// Wait for thread to be exited
		//t.join();
	}

	int ScreenWidth()
	{
		return m_nScreenWidth;
	}

	int ScreenHeight()
	{
		return m_nScreenHeight;
	}

private:
	void EnableSound() {
		m_bEnableSound = true;
	}

	void GameThread()
	{
		// Start SDL
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

		char bufAppName[256];
#ifdef _WIN32
		size_t t;
		wcstombs_s(&t, bufAppName, m_sAppName.c_str(), 256);
#else
		wcstombs(bufAppName, m_sAppName.c_str(), 256);
#endif
		// Create Window
		m_window = SDL_CreateWindow(
			bufAppName,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			m_nScreenWidth * m_nFontWidth,
			m_nScreenHeight * m_nFontHeight,
			SDL_WINDOW_SHOWN
		);

		// Create renderer
		m_render = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

		// Now hear this, because its annoying!
		// On windows, SDL does not create a double buffer by default, which
		// permits incremental rendering, a frankly necessary feature which
		// is used extensively by tile renderers (such as this system) to
		// get the most performance. On My Linux VM its fine too. However
		// on Linux with HW, it caused a non-clear bug, as my incremental
		// framing method was rendering differences to successive buffers
		// in SDL causing all sorts of on screen artefacts. SDL - You're shit.
		m_screen = SDL_CreateTexture(m_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_nScreenWidth * m_nFontWidth, m_nScreenHeight * m_nFontHeight);

		// Load Font File
		LoadFontFile("./olcfont_consolas.bmp");

		// Create user resources as part of this thread
		if (!OnUserCreate())
			m_bAtomActive = false;

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (m_bAtomActive)
		{
			// Run as fast as possible
			while (m_bAtomActive)
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float fElapsedTime = elapsedTime.count();

				SDL_Event e;
				while (SDL_PollEvent(&e))
				{
					switch (e.type)
					{
					case SDL_QUIT:
						m_bAtomActive = false;
						break;

					case SDL_KEYDOWN:
					{
						int k = e.key.keysym.sym & 0xfff; // SDL keycode
						m_keyNewState[k] = true;
					}
					break;

					case SDL_KEYUP:
					{
						int k = e.key.keysym.sym & 0xfff; // SDL keycode
						m_keyNewState[k] = false;
					}
					break;

					case SDL_MOUSEMOTION:
					{
						m_mousePosX = e.motion.x / m_nFontWidth;
						m_mousePosY = e.motion.y / m_nFontHeight;
					}
					break;

					case SDL_MOUSEBUTTONDOWN:
					{
						switch (e.button.button)
						{
						case SDL_BUTTON_LEFT:
							m_mouseNewState[0] = 1;
							break;
						case SDL_BUTTON_RIGHT:
							m_mouseNewState[1] = 1;
							break;
						case SDL_BUTTON_MIDDLE:
							m_mouseNewState[2] = 1;
							break;
						case SDL_BUTTON_X1:
							m_mouseNewState[3] = 1;
							break;
						case SDL_BUTTON_X2:
							m_mouseNewState[4] = 1;
							break;
						}
					}
					break;

					case SDL_MOUSEBUTTONUP:
					{

						switch (e.button.button)
						{
						case SDL_BUTTON_LEFT:
							m_mouseNewState[0] = 0;
							break;
						case SDL_BUTTON_RIGHT:
							m_mouseNewState[1] = 0;
							break;
						case SDL_BUTTON_MIDDLE:
							m_mouseNewState[2] = 0;
							break;
						case SDL_BUTTON_X1:
							m_mouseNewState[3] = 0;
							break;
						case SDL_BUTTON_X2:
							m_mouseNewState[4] = 0;
							break;
						}
					}
					break;

					}
				}

				// Handle Keyboard Input
				for (int k = 0; k < 512; k++)
				{
					m_keys._state[k].bPressed = false;
					m_keys._state[k].bReleased = false;

					if (m_keyNewState[k] != m_keyOldState[k])
					{
						if (m_keyNewState[k] == true)
						{
							m_keys._state[k].bPressed = !m_keys._state[k].bHeld;
							m_keys._state[k].bHeld = true;
						}
						else
						{
							m_keys._state[k].bReleased = true;
							m_keys._state[k].bHeld = false;
						}
					}

					m_keyOldState[k] = m_keyNewState[k];
				}

				for (int m = 0; m < 5; m++)
				{
					m_mouse[m].bPressed = false;
					m_mouse[m].bReleased = false;

					if (m_mouseNewState[m] != m_mouseOldState[m])
					{
						if (m_mouseNewState[m])
						{
							m_mouse[m].bPressed = true;
							m_mouse[m].bHeld = true;
						}
						else
						{
							m_mouse[m].bReleased = true;
							m_mouse[m].bHeld = false;
						}
					}

					m_mouseOldState[m] = m_mouseNewState[m];
				}

	// Handle Frame Update
				if (!OnUserUpdate(fElapsedTime))
					m_bAtomActive = false;

				// Update Title & Present Screen Buffer
				char s[256];
				snprintf(s, 256, "OneLoneCoder.com - Console Game Engine (SDL) - %s - FPS: %3.2f", bufAppName, 1.0f / fElapsedTime);
				SDL_SetWindowTitle(m_window, s);

				// Render differences
				CHAR_INFO *buff_old, *buff_new;
				buff_new = m_bufScreen[m_nCurrentBuffer];
				buff_old = m_bufScreen[(m_nCurrentBuffer + 1) % 2];

				SDL_SetRenderTarget(m_render, m_screen);

				for (int x = 0; x < m_nScreenWidth; x++)
				{
					for (int y = 0; y < m_nScreenHeight; y++)
					{
						int i = y * m_nScreenWidth + x;
						if (buff_new[i].colour != buff_old[i].colour || buff_new[i].glyph != buff_old[i].glyph)
						{
							// Draw Cell
							int cx = (buff_new[i].glyph - 0) % 64;
							int cy = (buff_new[i].glyph - 0) / 64;

							// Draw Foreground
							int cidx = (buff_new[i].colour & 0x00F0) >> 4;
							//SDL_Rect src_bg = { (cidx+45) * 8, 88, 8, 8 };
							SDL_Rect src_bg = { (cidx + 0) * 16, 0, 16, 16 };
							SDL_Rect dst = { x * m_nFontWidth, y * m_nFontHeight, m_nFontWidth,	m_nFontHeight };
							SDL_SetTextureColorMod(m_fontFile, 255, 255, 255);
							SDL_SetTextureAlphaMod(m_fontFile, 255);
							SDL_RenderCopy(m_render, m_fontFile, &src_bg, &dst);

							// set colour of font
							SDL_Color fg_col = colour_lookup[buff_new[i].colour & 0x000F];
							SDL_SetTextureColorMod(m_fontFile, fg_col.r, fg_col.g, fg_col.b);
							SDL_SetTextureAlphaMod(m_fontFile, fg_col.a);

							// Draw glyph
							SDL_Rect src_fg = { cx * (16), cy * (16), 16, 16 };
							SDL_RenderCopy(m_render, m_fontFile, &src_fg, &dst);
						}
					}
				}

				// Present buffers
				SDL_SetRenderTarget(m_render, nullptr);
				SDL_RenderCopy(m_render, m_screen, nullptr, nullptr);
				SDL_RenderPresent(m_render);


				// Flip buffers
				m_nCurrentBuffer = (m_nCurrentBuffer + 1) % 2;
			}

			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
				SDL_DestroyTexture(m_fontFile);
				SDL_DestroyRenderer(m_render);
				SDL_DestroyWindow(m_window);
				SDL_Quit();

				delete[] m_bufScreen[0];
				delete[] m_bufScreen[1];
				m_cvGameFinished.notify_one();
			}
			else
			{
				// User denied destroy for some reason, so continue running
				m_bAtomActive = true;
			}
		}
	}

public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;

	// Optional for clean up 
	virtual bool OnUserDestroy()
	{
		return true;
	}

protected:

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	};

	struct sKeyStateWrap
	{
		sKeyState _state[512];

	public:
		sKeyState & operator[] (int nKeyID)
		{
			SDL_Keycode scode = (SDLK_UNKNOWN & 0xffff);

			if (nKeyID > KEY_OFFSET)
			{
				int scode = (SDL_Keycode)nKeyID - KEY_OFFSET;
				return _state[scode];
			}

			switch (nKeyID)
			{
			case L'A': scode = (SDLK_a & 0xffff); break;
			case L'B': scode = (SDLK_b & 0xffff); break;
			case L'C': scode = (SDLK_c & 0xffff); break;
			case L'D': scode = (SDLK_d & 0xffff); break;
			case L'E': scode = (SDLK_e & 0xffff); break;
			case L'F': scode = (SDLK_f & 0xffff); break;
			case L'G': scode = (SDLK_g & 0xffff); break;
			case L'H': scode = (SDLK_h & 0xffff); break;
			case L'I': scode = (SDLK_i & 0xffff); break;
			case L'J': scode = (SDLK_j & 0xffff); break;
			case L'K': scode = (SDLK_k & 0xffff); break;
			case L'L': scode = (SDLK_l & 0xffff); break;
			case L'M': scode = (SDLK_m & 0xffff); break;
			case L'N': scode = (SDLK_n & 0xffff); break;
			case L'O': scode = (SDLK_o & 0xffff); break;
			case L'P': scode = (SDLK_p & 0xffff); break;
			case L'Q': scode = (SDLK_q & 0xffff); break;
			case L'R': scode = (SDLK_r & 0xffff); break;
			case L'S': scode = (SDLK_s & 0xffff); break;
			case L'T': scode = (SDLK_t & 0xffff); break;
			case L'U': scode = (SDLK_u & 0xffff); break;
			case L'V': scode = (SDLK_v & 0xffff); break;
			case L'W': scode = (SDLK_w & 0xffff); break;
			case L'X': scode = (SDLK_x & 0xffff); break;
			case L'Y': scode = (SDLK_y & 0xffff); break;
			case L'Z': scode = (SDLK_z & 0xffff); break;
			case L'0': scode = (SDLK_0 & 0xffff); break;
			case L'1': scode = (SDLK_1 & 0xffff); break;
			case L'2': scode = (SDLK_2 & 0xffff); break;
			case L'3': scode = (SDLK_3 & 0xffff); break;
			case L'4': scode = (SDLK_4 & 0xffff); break;
			case L'5': scode = (SDLK_5 & 0xffff); break;
			case L'6': scode = (SDLK_6 & 0xffff); break;
			case L'7': scode = (SDLK_7 & 0xffff); break;
			case L'8': scode = (SDLK_8 & 0xffff); break;
			case L'9': scode = (SDLK_9 & 0xffff); break;

				/*case L'\'': scode = (SDLK_APOSTROPHE & 0xffff); break;
				case L'\\': scode = (SDLK_BACKSLASH & 0xffff); break;
				case L',': scode = (SDLK_COMMA & 0xffff); break;
				case L'=': scode = (SDLK_EQUALS & 0xffff); break;
				case L'[': scode = (SDLK_LEFTBRACKET & 0xffff); break;
				case L']': scode = (SDLK_RIGHTBRACKET & 0xffff); break;
				case L'-': scode = (SDLK_MINUS & 0xffff); break;
				case L'.': scode = (SDLK_PERIOD & 0xffff); break;
				case L';': scode = (SDLK_SEMICOLON & 0xffff); break;
				case L'/': scode = (SDLK_SLASH & 0xffff); break;*/

			default: scode = (SDL_Keycode)nKeyID;
			}

			return _state[scode];
		}

	} m_keys;

	sKeyState m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

public:
	sKeyState GetKey(int nKeyID)
	{
		return m_keys[nKeyID];
	}


	int GetMouseX() { return m_mousePosX; }
	int GetMouseY() { return m_mousePosY; }
	sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }


protected:
	int Error(const wchar_t *msg)
	{
		wchar_t buf[256];
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	void LoadFontFile(const std::string& fname)
	{
		// Load image.
		SDL_Surface* temp = SDL_LoadBMP(fname.c_str());

		if (temp == nullptr) {
			std::wcout << L"Please download the necessary bmp file too!\n";
			throw 1;
		}

		// set color key to 255,0,255; this basically makes
		// it transparent.
		SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255, 0, 255));

		// Convert to texture.
		m_fontFile = SDL_CreateTextureFromSurface(m_render, temp);

		// Cleanup and return.
		SDL_FreeSurface(temp);
	}

	//static BOOL CloseHandler(DWORD evt)
	//{
	//	// Note this gets called in a seperate OS thread, so it must
	//	// only exit when the game has finished cleaning up, or else
	//	// the process will be killed before OnUserDestroy() has finished
	//	if (evt == CTRL_CLOSE_EVENT)
	//	{
	//		m_bAtomActive = false;

	//		// Wait for thread to be exited
	//		unique_lock<mutex> ul(m_muxGame);
	//		m_cvGameFinished.wait(ul);
	//	}
	//	return true;
	//}


	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		DrawLine(x1, y1, x2, y2, c, col);
		DrawLine(x2, y2, x3, y3, c, col);
		DrawLine(x3, y3, x1, y1, c, col);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										 // Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}



protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	int m_nFontWidth;
	int m_nFontHeight;
	CHAR_INFO *m_bufScreen[2];
	int m_nCurrentBuffer = 0;
	std::wstring m_sAppName;
	bool m_keyOldState[512] = { 0 };
	bool m_keyNewState[512] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;
	bool m_bEnableSound = false;
	static std::atomic<bool> m_bAtomActive;
	static std::condition_variable m_cvGameFinished;
	static std::mutex m_muxGame;

private:
	SDL_Window* m_window;
	SDL_Renderer* m_render;
	SDL_Texture *m_screen;
	SDL_Texture* m_fontFile;

protected: // Audio Engine =====================================================================

	class olcAudioSample
	{
	public:
		olcAudioSample()
		{

		}

		olcAudioSample(std::string sWavFile, olcConsoleGameEngine &cge)
		{
			uint8_t *wavData;
			SDL_AudioSpec fileSpec;
			uint32_t streamLen = 0;
			if (!SDL_LoadWAV(sWavFile.c_str(), &fileSpec, (uint8_t **)&wavData, &streamLen)) {
				std::cout << "Couldn't load audio file!\n" << SDL_GetError() << '\n';
				bSampleValid = false;
				return;
			}

			SDL_AudioCVT cvt;
			if (!SDL_BuildAudioCVT(&cvt, fileSpec.format, fileSpec.channels, fileSpec.freq,
				cge.sampleSpec.format, cge.sampleSpec.channels, cge.sampleSpec.freq)) {
				std::cout << "Failed to build cvt!\n" << SDL_GetError() << '\n';
				bSampleValid = false;
				return;
			}
			cvt.buf = (uint8_t *)malloc(streamLen * cvt.len_mult);
			cvt.len = streamLen;
			memcpy(cvt.buf, wavData, streamLen);
			free(cvt.buf);
			SDL_FreeWAV((uint8_t *)wavData);
			if (SDL_ConvertAudio(&cvt) == -1) {
				std::cout << "Failed to convert audio!\n" << SDL_GetError() << '\n';
				bSampleValid = false;
				return;
			}
			fSample = (float *)cvt.buf;
			nSamples = cvt.len_cvt / sizeof(float) / cge.spec.channels;
			bSampleValid = true;
		}

		~olcAudioSample() {
			SDL_FreeWAV((uint8_t *)fSample);
		}

		float *fSample;
		uint32_t nSamples = 0;
		bool bSampleValid = false;
	};

	// This vector holds all loaded sound samples in memory
	std::vector<olcAudioSample> vecAudioSamples;

	// This structure represents a sound that is currently playing. It only
	// holds the sound ID and where this instance of it is up to for its
	// current playback
	struct sCurrentlyPlayingSample
	{
		int nAudioSampleID = 0;
		long nSamplePosition = 0;
		bool bFinished = false;
		bool bLoop = false;
	};
	std::list<sCurrentlyPlayingSample> listActiveSamples;

	// Load a 16-bit WAVE file @ 44100Hz ONLY into memory. A sample ID
	// number is returned if successful, otherwise -1
	unsigned int LoadAudioSample(std::string sWavFile)
	{
		if (!m_bEnableSound)
			return -1;

		olcAudioSample a(sWavFile, *this);
		if (a.bSampleValid)
		{
			vecAudioSamples.push_back(a);
			return vecAudioSamples.size();
		}
		else
			return -1;
	}

	// Add sample 'id' to the mixers sounds to play list
	void PlaySample(int id, bool bLoop = false)
	{
		sCurrentlyPlayingSample a;
		a.nAudioSampleID = id;
		a.nSamplePosition = 0;
		a.bFinished = false;
		a.bLoop = bLoop;
		listActiveSamples.push_back(a);
	}

	void StopSample(int id)
	{

	}

	// The audio system uses by default a specific wave format
	bool CreateAudio(unsigned int nSampleRate = 44100, unsigned int nChannels = 1,
		unsigned int nBlocks = 8, unsigned int nBlockSamples = 512)
	{
		SDL_AudioSpec wanted;
		SDL_zero(wanted);
		wanted.channels = nChannels;
		wanted.format = AUDIO_S16;
		wanted.freq = nSampleRate;
		wanted.samples = nBlockSamples;
		wanted.userdata = this;
		wanted.callback = forwardCallback;
		SDL_zero(sampleSpec);
		sampleSpec.channels = nChannels;
		sampleSpec.format = AUDIO_F32;
		sampleSpec.freq = nSampleRate;
		sampleSpec.userdata = this;

		deviceID = SDL_OpenAudioDevice(NULL, 0, &wanted, &spec, 0);

		if (deviceID == 0) {
			std::cout << "Failed to open audio device!\n" << SDL_GetError() << '\n';
			return false;
		}

		SDL_PauseAudioDevice(deviceID, 0);

		return true;
	}

	// Stop and clean up audio system
	bool DestroyAudio()
	{
		SDL_CloseAudioDevice(deviceID);
		return false;
	}

	static void forwardCallback(void *userdata, uint8_t *byteStream, int len) {
		static_cast<olcConsoleGameEngine *>(userdata)->AudioThread(userdata, byteStream, len);
	}

	// Audio thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
	void AudioThread(void *userdata, uint8_t *byteStream, int len)
	{
		m_fGlobalTime = 0.0f;
		float fTimeStep = 1.0f / (float)spec.freq;

		// Goofy hack to get maximum integer for a type at run-time
		short nMaxSample = (short)pow(2, (sizeof(short) * 8) - 1) - 1;
		float fMaxSample = (float)nMaxSample;
		short nPreviousSample = 0;

		// Prepare block for processing
		memset(byteStream, 0, len);

		int16_t *buf = (int16_t *)byteStream;

		auto clip = [](float fSample, float fMax)
		{
			if (fSample >= 0.0)
				return fmin(fSample, fMax);
			else
				return fmax(fSample, -fMax);
		};

		uint32_t i = 0;
		for (unsigned int n = 0; n < len / sizeof(int16_t); n += spec.channels)
		{
			// User Process
			for (unsigned int c = 0; c < spec.channels; c++)
			{
				int16_t sample = (int16_t)(clip(GetMixerOutput(c, fTimeStep * i, fTimeStep), 1.0) * fMaxSample);
				buf[i] = sample;
				i++;
			}

			m_fGlobalTime = m_fGlobalTime + fTimeStep;
		}
	}

	// Overridden by user if they want to generate sound in real-time
	virtual float onUserSoundSample(int nChannel, float fGlobalTime, float fTimeStep)
	{
		return 0.0f;
	}

	// Overriden by user if they want to manipulate the sound before it is played
	virtual float onUserSoundFilter(int nChannel, float fGlobalTime, float fSample)
	{
		return fSample;
	}

	// The Sound Mixer - If the user wants to play many sounds simultaneously, and
	// perhaps the same sound overlapping itself, then you need a mixer, which
	// takes input from all sound sources for that audio frame. This mixer maintains
	// a list of sound locations for all concurrently playing audio samples. Instead
	// of duplicating audio data, we simply store the fact that a sound sample is in
	// use and an offset into its sample data. As time progresses we update this offset
	// until it is beyound the length of the sound sample it is attached to. At this
	// point we remove the playing souind from the list.
	//
	// Additionally, the users application may want to generate sound instead of just
	// playing audio clips (think a synthesizer for example) in which case we also
	// provide an "onUser..." event to allow the user to return a sound for that point
	// in time.
	//
	// Finally, before the sound is issued to the operating system for performing, the
	// user gets one final chance to "filter" the sound, perhaps changing the volume
	// or adding funky effects
	float GetMixerOutput(int nChannel, float fGlobalTime, float fTimeStep)
	{
		// Accumulate sample for this channel
		float fMixerSample = 0.0f;

		for (auto &s : listActiveSamples)
		{
			// Calculate sample position
			if (nChannel == 0)
				s.nSamplePosition += (long)((float)spec.freq * fTimeStep);

			// If sample position is valid add to the mix
			if (s.nSamplePosition < vecAudioSamples[s.nAudioSampleID - 1].nSamples)
				fMixerSample += vecAudioSamples[s.nAudioSampleID - 1].fSample[(s.nSamplePosition * spec.channels) + nChannel];
			else
				s.bFinished = true; // Else sound has completed
		}

		// If sounds have completed then remove them
		listActiveSamples.remove_if([](const sCurrentlyPlayingSample &s) {return s.bFinished; });

		// The users application might be generating sound, so grab that if it exists
		fMixerSample += onUserSoundSample(nChannel, fGlobalTime, fTimeStep);

		// Return the sample via an optional user override to filter the sound
		return onUserSoundFilter(nChannel, fGlobalTime, fMixerSample);
		return fMixerSample;
	}

	SDL_AudioDeviceID deviceID;
	SDL_AudioSpec spec, sampleSpec;

	std::atomic<float> m_fGlobalTime{ 0.0f };
};

std::atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
std::condition_variable olcConsoleGameEngine::m_cvGameFinished;
std::mutex olcConsoleGameEngine::m_muxGame;
