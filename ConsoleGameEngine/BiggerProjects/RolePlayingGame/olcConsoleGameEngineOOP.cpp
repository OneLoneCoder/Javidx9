/*
	+-------------------------------------------------------------+
	|              OneLoneCoder RPG Game Engine                   |
	|     "The Legend of WittyBit, Fantasy Quest VI" - javidx9    |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~

	This is the code I created as part of my
	"Code-It-Yourself! Role Playing Game" series on YouTube. This is
	NOT a game. The project will compile and demonstrate several
	systems developed as part of that series. My original intention
	was to develop a small yet complete RPG, alas real life got in
	the way. After several months, I've decided to just open the source
	"as is", so it will contain bugs, be confusing and all round not
	up to the usual "quality" I strive for.

	Part 1: https://youtu.be/xXXt3htgDok
	Part 2: https://youtu.be/AWY_ITpldRk
	Part 3: https://youtu.be/UcNSb-m4YQU
	Part 4: https://youtu.be/AnyoUfeNZ1Y

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018, 2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2018, 2019
*/

#include "olcConsoleGameEngineOOP.h"

olcConsoleGameEngineOOP::olcConsoleGameEngineOOP()
{
	m_nScreenWidth = 80;
	m_nScreenHeight = 30;

	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	memset(m_keyNewState, 0, 256 * sizeof(short));
	memset(m_keyOldState, 0, 256 * sizeof(short));
	memset(m_keys, 0, 256 * sizeof(sKeyState));
	m_mousePosX = 0;
	m_mousePosY = 0;

	m_sAppName = L"Default";
}

olcConsoleGameEngineOOP::~olcConsoleGameEngineOOP()
{
	SetConsoleActiveScreenBuffer(m_hOriginalConsole);
	delete[] m_bufScreen;
}

int olcConsoleGameEngineOOP::ConstructConsole(int width, int height, int fontw, int fonth)
{
	if (m_hConsole == INVALID_HANDLE_VALUE)
		return Error(L"Bad Handle");

	m_nScreenWidth = width;
	m_nScreenHeight = height;

	// Update 13/09/2017 - It seems that the console behaves differently on some systems
	// and I'm unsure why this is. It could be to do with windows default settings, or
	// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
	// by way of useful information, and so the resulting sequence is the reult of experiment
	// that seems to work in multiple cases.
	//
	// The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
	// fail depending on the state of the current console properties, i.e. you can't set
	// the buffer size until you set the screen size, but you can't change the screen size
	// until the buffer size is correct. This coupled with a precise ordering of calls
	// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

	// Change console visual size to a minimum so ScreenBuffer can shrink
	// below the actual visual size
	m_rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
	if (!SetConsoleScreenBufferSize(m_hConsole, coord))
		Error(L"SetConsoleScreenBufferSize");

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(m_hConsole))
		return Error(L"SetConsoleActiveScreenBuffer");

	// Set the font size now that the screen buffer has been assigned to the console
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	//wcscpy_s(cfi.FaceName, L"Lucida Console");
	//wcscpy_s(cfi.FaceName, L"Liberation Mono");
	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
		return Error(L"SetCurrentConsoleFontEx");

	// Get screen buffer info and check the maximum allowed window size. Return
	// error if exceeded, so user knows their dimensions/fontsize are too large
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
		return Error(L"GetConsoleScreenBufferInfo");
	if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		return Error(L"Screen Height / Font Height Too Big");
	if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
		return Error(L"Screen Width / Font Width Too Big");

	// Set Physical Console Window Size
	m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
	if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
		return Error(L"SetConsoleWindowInfo");

	// Set flags to allow mouse input		
	if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return Error(L"SetConsoleMode");

	// Allocate memory for screen buffer
	m_bufScreen = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
	memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

	return 1;
}

void olcConsoleGameEngineOOP::Draw(int x, int y, wchar_t c, short col)
{
	if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
	{
		m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
		m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
	}
}

void olcConsoleGameEngineOOP::Fill(int x1, int y1, int x2, int y2, wchar_t c, short col)
{
	Clip(x1, y1);
	Clip(x2, y2);
	for (int x = x1; x < x2; x++)
		for (int y = y1; y < y2; y++)
			Draw(x, y, c, col);
}

void olcConsoleGameEngineOOP::DrawString(int x, int y, wstring c, short col)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
		m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
	}
}

void olcConsoleGameEngineOOP::DrawStringAlpha(int x, int y, wstring c, short col)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		if (c[i] != L' ')
		{
			m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
		}
	}
}

void olcConsoleGameEngineOOP::Clip(int &x, int &y)
{
	if (x < 0) x = 0;
	if (x >= m_nScreenWidth) x = m_nScreenWidth;
	if (y < 0) y = 0;
	if (y >= m_nScreenHeight) y = m_nScreenHeight;
}

void olcConsoleGameEngineOOP::DrawLine(int x1, int y1, int x2, int y2, wchar_t c, short col)
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
		for (i = 0; x<xe; i++)
		{
			x = x + 1;
			if (px<0)
				px = px + 2 * dy1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0))
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
		for (i = 0; y<ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0))
					x = x + 1;
				else
					x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			Draw(x, y, c, col);
		}
	}
}

void olcConsoleGameEngineOOP::DrawCircle(int xc, int yc, int r, wchar_t c, short col)
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

void olcConsoleGameEngineOOP::FillCircle(int xc, int yc, int r, wchar_t c, short col)
{
	// Taken from wikipedia
	int x = 0;
	int y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	auto drawline = [&](int sx, int ex, int ny)
	{
		for (int i = sx; i < ex; i++)
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

void olcConsoleGameEngineOOP::DrawSprite(int x, int y, olcSprite *sprite)
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

void olcConsoleGameEngineOOP::DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h)
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

void olcConsoleGameEngineOOP::DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r, float s, short col)
{
	// pair.first = x coordinate
	// pair.second = y coordinate

	// Create translated model vector of coordinate pairs
	vector<pair<float, float>> vecTransformedCoordinates;
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




void olcConsoleGameEngineOOP::Start()
{
	m_bAtomActive = true;

	// Star the thread
	thread t = thread(&olcConsoleGameEngineOOP::GameThread, this);

	// Wait for thread to be exited
	t.join();
}

int olcConsoleGameEngineOOP::ScreenWidth()
{
	return m_nScreenWidth;
}

int olcConsoleGameEngineOOP::ScreenHeight()
{
	return m_nScreenHeight;
}

void olcConsoleGameEngineOOP::GameThread()
{
	// Create user resources as part of this thread
	if (!OnUserCreate())
		m_bAtomActive = false;

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (m_bAtomActive)
	{
		// Run as fast as possible
		while (m_bAtomActive)
		{
			// Handle Timing
			tp2 = chrono::system_clock::now();
			chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			float fElapsedTime = elapsedTime.count();

			// Handle Keyboard Input
			for (int i = 0; i < 256; i++)
			{
				m_keyNewState[i] = GetAsyncKeyState(i);

				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
				{
					if (m_keyNewState[i] & 0x8000)
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

			// Handle Mouse Input - Check for window events
			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
			if (events > 0)
				ReadConsoleInput(m_hConsoleIn, inBuf, events, &events);

			// Handle events - we only care about mouse clicks and movement
			// for now
			for (DWORD i = 0; i < events; i++)
			{
				switch (inBuf[i].EventType)
				{
				case FOCUS_EVENT:
				{
					m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
				}
				break;

				case MOUSE_EVENT:
				{
					switch (inBuf[i].Event.MouseEvent.dwEventFlags)
					{
					case MOUSE_MOVED:
					{
						m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}
					break;

					case 0:
					{
						for (int m = 0; m < 5; m++)
							m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

					}
					break;

					default:
						break;
					}
				}
				break;

				default:
					break;
					// We don't care just at the moment
				}
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
			wchar_t s[256];
			swprintf_s(s, 256, L"OneLoneCoder.com - Console Game Engine - %s - FPS: %3.2f - %d ", m_sAppName.c_str(), 1.0f / fElapsedTime, events);
			SetConsoleTitle(s);
			WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
		}

		if (OnUserDestroy())
		{
			// User has permitted destroy, so exit and clean up

			delete[] m_bufScreen;
			SetConsoleActiveScreenBuffer(m_hOriginalConsole);
			m_cvGameFinished.notify_one();
		}
		else
		{
			// User denied destroy for some reason, so continue running
			m_bAtomActive = true;
		}
	}
}


// Optional for clean up 
bool olcConsoleGameEngineOOP::OnUserDestroy()
{
	return true;
}

int olcConsoleGameEngineOOP::Error(const wchar_t *msg)
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(m_hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
	return 0;
}

BOOL olcConsoleGameEngineOOP::CloseHandler(DWORD evt)
{
	// Note this gets called in a seperate OS thread, so it must
	// only exit when the game has finished cleaning up, or else
	// the process will be killed before OnUserDestroy() has finished
	if (evt == CTRL_CLOSE_EVENT)
	{
		m_bAtomActive = false;

		// Wait for thread to be exited
		unique_lock<mutex> ul(m_muxGame);
		m_cvGameFinished.wait(ul);
	}
	return true;
}

atomic<bool> olcConsoleGameEngineOOP::m_bAtomActive = false;
condition_variable olcConsoleGameEngineOOP::m_cvGameFinished;
mutex olcConsoleGameEngineOOP::m_muxGame;