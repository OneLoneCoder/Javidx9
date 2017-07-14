/*
OneLoneCoder.com - What are Cellular Automata? John Conway's Game Of Life
"Get lost..." - @Javidx9

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
Sometimes, its worth reflecting on the fact that from simple things can emerge
significant beauty. This is an implementaion of Game Of Life that runs in the console

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/E7CxMHsYzSs

Last Updated: 14/07/2017
*/

#include <iostream>
using namespace std;

#include "olcConsoleGameEngine.h"

class OneLoneCoder_GameOfLife : public olcConsoleGameEngine
{
public:
	OneLoneCoder_GameOfLife()
	{
		m_sAppName = L"Game Of Life";
	}

private:
	int *m_output;
	int *m_state;



protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		m_output = new int[ScreenWidth() * ScreenHeight()];
		m_state = new int[ScreenWidth() * ScreenHeight()];

		memset(m_output, 0, ScreenWidth() * ScreenHeight() * sizeof(int));
		memset(m_state, 0, ScreenWidth() * ScreenHeight() * sizeof(int));

		for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
			m_state[i] = rand() % 2;

		auto set = [&](int x, int y, wstring s)
		{
			int p = 0;
			for (auto c : s)
			{
				m_state[y*ScreenWidth() + x + p] = c == L'#' ? 1 : 0;
				p++;
			}
		};

		/*
		// R-Pentomino
		set(80, 50, L"  ## ");
		set(80, 51, L" ##  ");
		set(80, 52, L"  #  ");

		// Gosper Glider Gun
		set(60, 45, L"........................#............");
		set(60, 46, L"......................#.#............");
		set(60, 47, L"............##......##............##.");
		set(60, 48, L"...........#...#....##............##.");
		set(60, 49, L"##........#.....#...##...............");
		set(60, 50, L"##........#...#.##....#.#............");
		set(60, 51, L"..........#.....#.......#............");
		set(60, 52, L"...........#...#.....................");
		set(60, 53, L"............##.......................");

		// Infinite Growth
		set(20, 50, L"########.#####...###......#######.#####");
		*/

		return true;
	}

	

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//this_thread::sleep_for(50ms);

		//if (!m_keys[VK_SPACE].bReleased)
		//	return true;
		
		auto cell = [&](int x, int y)
		{
			return m_output[y * ScreenWidth() + x];
		};

		// Store output state
		for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
			m_output[i] = m_state[i];
				
		for (int x = 1; x < ScreenWidth() - 1; x++)
			for (int y = 1; y < ScreenHeight() - 1; y++)
			{
				// The secret of artificial life =================================================
				int nNeighbours =	cell(x - 1, y - 1) + cell(x - 0, y - 1) + cell(x + 1, y - 1) +
									cell(x - 1, y + 0) +          0         + cell(x + 1, y + 0) +
									cell(x - 1, y +	1) + cell(x + 0, y + 1) + cell(x + 1, y + 1);

				if (cell(x, y) == 1)
					m_state[y*ScreenWidth() + x] = nNeighbours == 2 || nNeighbours == 3;
				else
					m_state[y*ScreenWidth() + x] = nNeighbours == 3;
				// ===============================================================================


				if (cell(x, y) == 1)
					Draw(x, y, PIXEL_SOLID, FG_WHITE);
				else
					Draw(x, y, PIXEL_SOLID, FG_BLACK);
			}

		return true;
	}
};


int main()
{
	// Seed random number generator
	srand(clock());

	// Use olcConsoleGameEngine derived app
	OneLoneCoder_GameOfLife game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}