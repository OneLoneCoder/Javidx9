/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9

Disclaimer
~~~~~~~~~~
I don't care what you use this for. It's intended to be educational, and perhaps
to the oddly minded - a little bit of fun. Please hack this, change it and use it
in any way you see fit. BUT, you acknowledge that I am not responsible for anything
bad that happens as a result of your actions. However, if good stuff happens, I
would appreciate a shout out, or at least give the blog some publicity for me.
Cheers!

Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're 
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/cWc0hgYwZyc

Last Updated: 10/07/2017

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

Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in.

The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.

There may be bugs! 

See my other videos for examples!

*/

#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <atomic>
#include <condition_variable>
using namespace std;

#include <SDL.h>
#include <SDL_ttf.h>
// #include <windows.h>

#define VK_UP SDL_SCANCODE_UP
#define VK_LEFT SDL_SCANCODE_LEFT
#define VK_RIGHT SDL_SCANCODE_RIGHT
#define VK_DOWN SDL_SCANCODE_DOWN

enum COLOUR
{
	FG_BLACK		= 0x0000,
	FG_DARK_BLUE    = 0x0001,	
	FG_DARK_GREEN   = 0x0002,
	FG_DARK_CYAN    = 0x0003,
	FG_DARK_RED     = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW  = 0x0006,
	FG_GREY         = 0x0007,
	FG_BLUE			= 0x0009,
	FG_GREEN		= 0x000A,
	FG_CYAN			= 0x000B,
	FG_RED			= 0x000C,
	FG_MAGENTA		= 0x000D,
	FG_YELLOW		= 0x000E,
	FG_WHITE		= 0x000F,
	BG_BLACK		= 0x0000,
	BG_DARK_BLUE	= 0x0010,
	BG_DARK_GREEN	= 0x0020,
	BG_DARK_CYAN	= 0x0030,
	BG_DARK_RED		= 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW	= 0x0060,
	BG_GREY			= 0x0070,
	BG_BLUE			= 0x0090,
	BG_GREEN		= 0x00A0,
	BG_CYAN			= 0x00B0,
	BG_RED			= 0x00C0,
	BG_MAGENTA		= 0x00D0,
	BG_YELLOW		= 0x00E0,
	BG_WHITE		= 0x00F0,
};

SDL_Color g_Colors[] = { 
	{0,0,0,0},      // 0
	{0,0,127,0},    // 1
	{0,127,0,0},    // 2
	{0,127,127,0},  // 3
	{127,0,0,0},    // 4
	{127,0,127,0},  // 5
	{127,127,0,0},  // 6
	{127,127,127,0},// 7
	{0,0,0,0},
	{0,0,255,0},    // 9
	{0,255,0,0},    // A
	{0,255,255,0},  // B
	{255,0,0,0},    // C
	{255,0,255,0},  // D
	{255,255,0,0},  // E
	{255,255,255,0},// F
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_HALF = 0x2592,
};

struct RenderString
{
	SDL_Texture *texture;
	SDL_Rect distination;
};

class olcConsoleGameEngine
{
public:
	olcConsoleGameEngine()
		: m_window{nullptr}, m_render{nullptr}
	{
        SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		m_nScreenWidth = 80;
		m_nScreenHeight = 30;

		// m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_keyNewState = new short[256];
		m_keyOldState = new short[256];
		memset(m_keyNewState, 0, 256 * sizeof(short));
		memset(m_keyOldState, 0, 256 * sizeof(short));

		memset(m_keys, 0, 256 * sizeof(sKeyState));


		m_sAppName = L"Default";
	}

	int ConstructConsole(int width, int height, int fontw = 12, int fonth = 12)
	{
		m_cellw = fontw;
		m_cellh = fonth;
		m_window = SDL_CreateWindow((const char*)m_sAppName.data(), SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, width*m_cellw, height*m_cellh, 0);
		if(!m_window)
			return Error(L"Unable to create Window");
		m_render = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		if(!m_render)
			return Error(L"Unable to create render");
		string font{"/usr/share/fonts/truetype/freefont/FreeMono.ttf"};
		m_font = TTF_OpenFont(font.data(),fonth);
		if(!m_font)
			return Error(L"Unable to font");
		m_nScreenWidth = width*m_cellw;
		m_nScreenHeight = height*m_cellh;

/*
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, L"Consolas");

		if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
			return Error(L"SetCurrentConsoleFontEx");
		COORD coordLargest = GetLargestConsoleWindowSize(m_hConsole);
		if (m_nScreenHeight > coordLargest.Y)
			return Error(L"Game Height Too Big");
		if (m_nScreenWidth > coordLargest.X)
			return Error(L"Game Width Too Big");

		COORD buffer = { (short)m_nScreenWidth, (short)m_nScreenHeight };
		if (!SetConsoleScreenBufferSize(m_hConsole, buffer))
			Error(L"SetConsoleScreenBufferSize");

		m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
		if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
			Error(L"SetConsoleWindowInfo");

		m_bufScreen = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
*/

		return 1;
	}

	void Draw(int x, int y, wchar_t in = 0x2588, short col = 0x000F)
	{
		SDL_Color &c = g_Colors[col];
		SDL_Rect rect {x,y,4,4};
		SDL_SetRenderDrawColor(m_render, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(m_render, &rect);

	}

	void Fill(int x1, int y1, int x2, int y2, wchar_t in = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		SDL_Color c = g_Colors[col];
		SDL_Rect rect {x1,y1,abs(x2-x1),abs(y2-y1)};
		SDL_SetRenderDrawColor(m_render, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(m_render, &rect);
	}

	void DrawString(int x, int y, wstring c, short col = 0x000F)
	{
		string stds{c.begin(), c.end()};
		SDL_Color color = g_Colors[col];
		color.a = 255;
		RenderString text;
		string key = stds + "_COL_" + to_string(col);
		if(stringCache.count(key) == 0) {
			SDL_Surface *surface = TTF_RenderText_Solid(m_font, stds.data(), color);
			text.texture = SDL_CreateTextureFromSurface(m_render, surface);
			text.distination.w = surface->w;
			text.distination.h = surface->h;
            SDL_FreeSurface(surface);
		    stringCache.insert(make_pair(key, text));
		}
		text = stringCache.at(key);
		text.distination.x = x;
		text.distination.y = y*m_cellh;
        m_textBuffer.push_back(text);
	}

	void DrawStringAlpha(int x, int y, wstring c, short col = 0x0000)
	{
		string stds{c.begin(), c.end()};
		SDL_Color color = g_Colors[col];
		RenderString text;
		string key = stds + "_COL_" + to_string(col);
		if(stringCache.count(key) == 0) {
			SDL_Surface *surface = TTF_RenderText_Solid(m_font, stds.data(), color);
			text.texture = SDL_CreateTextureFromSurface(m_render, surface);
			text.distination.w = surface->w;
			text.distination.h = surface->h;
            SDL_FreeSurface(surface);
		    stringCache.insert(make_pair(key, text));
		}
		text = stringCache.at(key);
		text.distination.x = x;
		text.distination.y = y*m_cellh;
        m_textBuffer.push_back(text);
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= m_nScreenWidth) x = m_nScreenWidth;
		if (y < 0) y = 0;
		if (y >= m_nScreenHeight) y = m_nScreenHeight;
	}

	void DrawLine(int x1, int y1, int x2, int y2, wchar_t in = 0x2588, short col = 0x000F)
	{
		SDL_Color &c = g_Colors[col];
		SDL_SetRenderDrawColor(m_render, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(m_render,x1*m_cellw,y1*m_cellh,x2*m_cellw,y2*m_cellh);
	}



	~olcConsoleGameEngine()
	{
		for(const auto &r : stringCache)
		{
			SDL_DestroyTexture(r.second.texture);
		}
		SDL_DestroyRenderer(m_render);
		SDL_DestroyWindow(m_window);
		TTF_Quit();
		SDL_Quit();
	}

public:
	void Start()
	{
		m_bAtomActive = true;

		// Star the thread
		thread t = thread(&olcConsoleGameEngine::GameThread, this);

		// Wait for thread to be exited
		unique_lock<mutex> lock1(m_muxGame, defer_lock);
		m_cvGameFinished.wait(lock1);

		// Tidy up
		t.join();
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
	void GameThread()
	{
		// Create user resources as part of this thread
		if (!OnUserCreate())
			return;

		auto tp1 = chrono::system_clock::now();
		auto tp2 = chrono::system_clock::now();

		// Run as fast as possible
		while (m_bAtomActive)
		{				
			// Handle Timing
			tp2 = chrono::system_clock::now();
			chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			float fElapsedTime = elapsedTime.count();

			// Handle Input
			SDL_Event e;
			while(SDL_PollEvent(&e) != 0 ){
				if(e.type == SDL_QUIT)
					m_bAtomActive = false;
			}
			int numKeys;
			auto keyNewState = SDL_GetKeyboardState(&numKeys);
			for (int i = 0; i < 256 && i < numKeys; i++)
			{
				m_keyNewState[i] = keyNewState[i];
				
				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
				{
					if (m_keyNewState[i] > 0)
					{
						m_keys[i].bPressed = !m_keys[i].bHeld;
						m_keys[i].bHeld = true;
					}
					else
					{
						m_keys[i].bReleased = true;
						m_keys[i].bHeld = false;
					}
				}

				m_keyOldState[i] = m_keyNewState[i];
			}


			// Handle Frame Update
			if (!OnUserUpdate(fElapsedTime))
				m_bAtomActive = false;
			

			// swprintf_s(s, 128, L"OneLoneCoder.com - Console Game Engine - %s - FPS: %3.2f ", m_sAppName.c_str(), 1.0f / fElapsedTime);

			// Update Title & Present Screen Buffer
			string title = "OneLoneCoder.com - Console Game Engine - ";
			string fps = " FPS: " + to_string(1.0f / fElapsedTime);
			string appname(m_sAppName.begin(), m_sAppName.end());
			SDL_SetWindowTitle(m_window,(title + appname + fps).data());

			for(auto && t : m_textBuffer)
				SDL_RenderCopy(m_render, t.texture, NULL, &t.distination);
			
			m_textBuffer.clear();
			SDL_RenderPresent(m_render);
		}

		m_cvGameFinished.notify_one();
	}

public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;	


protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
//	CHAR_INFO *m_bufScreen;
	atomic<bool> m_bAtomActive;
	condition_variable m_cvGameFinished;
	mutex m_muxGame;
	wstring m_sAppName;

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256];


protected:
	int Error(const wchar_t *msg)
	{
		cerr << "ERROR: " << msg << '\n' << SDL_GetError() << '\n';
		return -1;
	}

private:
    SDL_Window *m_window;
    SDL_Renderer *m_render;
	TTF_Font *m_font;
	uint8_t m_cellh;
	uint8_t m_cellw;
	list<RenderString> m_textBuffer;
	map<string, RenderString> stringCache;
/*	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	SMALL_RECT m_rectWindow; */
	short *m_keyOldState;
	short *m_keyNewState;
};