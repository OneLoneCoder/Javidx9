/*
OneLoneCoder.com - Ludum Dare 42 - Running Out Of Space
"What's all this about Fish?" - @Javidx9

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
Ludum Dare 42 - A quick and lazy speedcode following the theme of "Running Out Of Space"

OBJECTIVE: Stay Alive! Use Arrow Keys to move player. Press space to deploy bombs
to blow up encroaching walls. Collect bombs when they appear. Stay away from the
blasts, and dont get trapped in the walls!


Video
~~~~~
https://www.twitch.tv/videos/295730019

Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV


Last Updated: 11/08/2018
*/


// For Windows 7
//#include "olcConsoleGameEngineGL.h"

// For Windows
#include "olcConsoleGameEngine.h"

// For Linux
//#include "olcConsoleGameEngineSDL.h"

#include <algorithm>
#include <string>
using namespace std;


class LudumDare42 : public olcConsoleGameEngine
{
public:
	LudumDare42()
	{
		m_sAppName = L"Ludum Dare 42 - Running Out Of Space";
	}


private:
	int nWorldWidth = 60;
	int nWorldHeight = 60;

	int *world = nullptr;

	int nBlocksPlaced = 0;

	float fBlockTimeElapsed = 0.0f;
	float fBlockTime = 0.02f;

	int nPlayerX;
	int nPlayerY;

	

	float fSurvivalTime = 0.0f;

	enum
	{
		EMPTY,
		BORDER, 
		BLOCK,
	};


	struct sParticle
	{
		float px, py;
		float vx, vy;
		float lifetime;
		bool dead = false;
	};

	struct sBomb
	{
		int px, py;
		float fuse;
	};


	list<sParticle> listParticles;
	list<sBomb> listBombs;

	int  nBombPickupX = 20;
	int  nBombPickupY = 20;
	bool bBombsVisible = true;
	int  nBoomsticks = 1;

	int nPlayerHealth = 100;
	float fHighScore = 0.0f;

	bool bReset = true;

public:
	bool OnUserCreate() override
	{
		world = new int[nWorldWidth * nWorldHeight]{ 0 };
		bReset = true;	
		return true;
	}


	void Boom(int xc, int yc, int r)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			if (ny <= 0 || ny >= nWorldHeight) return;
			sx = max(sx, 0);
			ex = min(ex, nWorldWidth);
			for (int i = sx; i <= ex; i++)
			{

				if (world[ny * nWorldWidth + i] != BORDER)
				{
					world[ny * nWorldWidth + i] = 0;
					nBlocksPlaced--;
				}
			}
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

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (bReset)
		{
			for (int x = 0; x<nWorldWidth; x++)
				for (int y = 0; y < nWorldHeight; y++)
				{
					if (x == 0 || y == 0 || x == nWorldWidth - 1 || y == nWorldHeight - 1)
						world[y*nWorldWidth + x] = BORDER;
					else
						world[y*nWorldWidth + x] = EMPTY;
				}

			nPlayerX = nWorldWidth / 2;
			nPlayerY = nWorldHeight / 2;
			nPlayerHealth = 100;
			fSurvivalTime = 0;
			nBombPickupX = 20;
			nBombPickupY = 20;
			bBombsVisible = true;
			nBoomsticks = 1;
			listBombs.clear();
			listParticles.clear();
			bReset = false;
		}


		if (nPlayerHealth <= 0)
		{
			// Show Game Over

			// Clear Screen
			Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
			DrawString(10, 10, L"Oh Dear! You ran out of space...");
			DrawString(10, 12, L"Your Score: " + to_wstring((int)fSurvivalTime));
			DrawString(10, 14, L"High Score: " + to_wstring((int)fHighScore));
			DrawString(10, 20, L"OneLoneCoder - www.youtube.com/javidx9");
			DrawString(10, 22, L"Ludum Dare 42 - 'Running Out Of Space'");
			DrawString(10, 30, L"Press 'R' to restart");

			if (GetKey(L'R').bReleased)
				bReset = true;

			return true;
		}
		
		
		
		// Place Block, only on borders, or attached to 
		// neighbouring blocks

		auto Get = [&](int x, int y)
		{
			return world[y*nWorldWidth + x];
		};

		auto Set = [&](int x, int y, int i)
		{
			world[y*nWorldWidth + x] = i;
		};

		fSurvivalTime += fElapsedTime;

		fBlockTimeElapsed += fElapsedTime;
		if (fBlockTimeElapsed >= fBlockTime)
		{
			fBlockTimeElapsed -= fBlockTime;
			// TODO: perfect !
			// May come back to this later to make it optimal
			if (nBlocksPlaced < (nWorldWidth * nWorldHeight) - 2 * (nWorldWidth - 2) - 2 * (nWorldHeight - 2) - 4) // hmmm maybe? :D
			{
				bool bBlockPlaced = false;
				while (!bBlockPlaced)
				{
					int x = (rand() % (nWorldWidth - 2) + 1);
					int y = (rand() % (nWorldHeight - 2) + 1);

					// Check if location is suitable, it is
					// if neighbouring cell is not empty

					if (!Get(x, y) &&
						(Get(x - 1, y) || Get(x + 1, y) || Get(x, y - 1) || Get(x, y + 1)))
					{
						Set(x, y, BLOCK);
						bBlockPlaced = true;
						nBlocksPlaced++;
					}
				}
			}

			if (!bBombsVisible && nBoomsticks == 0)
			{
				bool bBlockPlaced = false;
				while (!bBlockPlaced)
				{
					int x = (rand() % (nWorldWidth - 2) + 1);
					int y = (rand() % (nWorldHeight - 2) + 1);
					if (!Get(x, y))
					{
						bBlockPlaced = true;
						nBombPickupX = x;
						nBombPickupY = y;
						bBombsVisible = true;
					}
				}
			}

		
			// Handle Player
			if (IsFocused())
			{
				if (GetKey(VK_UP).bHeld)
				{
					if (!Get(nPlayerX, nPlayerY - 1))
						nPlayerY--;
				}

				if (GetKey(VK_DOWN).bHeld)
				{
					if (!Get(nPlayerX, nPlayerY + 1))
						nPlayerY++;
				}

				if (GetKey(VK_LEFT).bHeld)
				{
					if (!Get(nPlayerX - 1, nPlayerY))
						nPlayerX--;
				}

				if (GetKey(VK_RIGHT).bHeld)
				{
					if (!Get(nPlayerX + 1, nPlayerY))
						nPlayerX++;
				}

				if (bBombsVisible && (pow(nPlayerX-nBombPickupX,2) + pow(nPlayerY-nBombPickupY, 2)) <= 6)
				{
					bBombsVisible = false;
					nBoomsticks += 5;
				}

				// Check if player is trapped
				if (Get(nPlayerX - 1, nPlayerY) && Get(nPlayerX + 1, nPlayerY) &&
					Get(nPlayerX, nPlayerY - 1) && Get(nPlayerX, nPlayerY + 1))
				{
					if (nBoomsticks == 0)
						nPlayerHealth = 0;
				}
			}
		}

		// Update Particles
		for (auto &p : listParticles)
		{
			p.vy += 100.0f * fElapsedTime;

			p.px += p.vx * fElapsedTime;
			p.py += p.vy * fElapsedTime;

			if (p.px < 0 || p.px >= nWorldWidth || p.py < 0 || p.py >= nWorldHeight)
			{
				p.dead = true;
			}
		}

		listParticles.remove_if([&](const sParticle &p) { return p.dead; });


		// Update Bombs
		for (auto &b : listBombs)
		{
			b.fuse -= fElapsedTime;
			if (b.fuse < 0)
			{
				// Boom! if boomsticks are available
				Boom(b.px, b.py, 5);

				// Some sort of visual effect for explosion
				for (int i = 0; i < 20; i++)
				{
					sParticle p;
					p.px = b.px + (2.5f * (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f));
					p.py = b.py + (2.5f * (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f));
					p.vx = 1.1f * 50.0f * (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f);
					p.vy = 1.1f * 50.0f * (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f);
					p.dead = false;

					listParticles.push_back(p);
				}

				// Injure player
				nPlayerHealth -= max(0, 4 * (5 - sqrtf(pow(nPlayerX - b.px, 2) + pow(nPlayerY - b.py, 2))));

			}
		}

		listBombs.remove_if([&](const sBomb &b) { return b.fuse < 0; });

		if (IsFocused() && GetKey(VK_SPACE).bReleased)
		{
			if (nBoomsticks > 0)
			{
				listBombs.push_back({ nPlayerX, nPlayerY, 5.0f });
				nBoomsticks--;
			}
		}

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		// Draw World
		for (int x = 0; x < nWorldWidth; x++)
		{
			for (int y = 0; y < nWorldHeight; y++)
			{
				switch (world[y*nWorldWidth + x])
				{
				case BORDER:
					Draw(x, y, PIXEL_SOLID, FG_WHITE);
					break;

				case BLOCK:
					Draw(x, y, PIXEL_SOLID, FG_RED);
					break;
				}
			}
		}

		// Draw Boom 
		for (auto &p : listParticles)
		{
			Draw(p.px, p.py, PIXEL_QUARTER, FG_YELLOW);
		}

		// Draw Bombs
		for (auto &b : listBombs)
		{
			Draw(b.px - 1, b.py, PIXEL_SOLID, FG_GREEN);
			Draw(b.px + 1, b.py, PIXEL_SOLID, FG_GREEN);
			Draw(b.px, b.py - 1, PIXEL_SOLID, FG_GREEN);
			Draw(b.px, b.py + 1, PIXEL_SOLID, FG_GREEN);
			DrawString(b.px, b.py, to_wstring((int)b.fuse + 1));
		}

		// Draw Bomb Pickup
		if (bBombsVisible)
		{
			Fill(nBombPickupX - 1, nBombPickupY - 1, nBombPickupX + 2, nBombPickupY + 2, PIXEL_SOLID, FG_MAGENTA);
			Draw(nBombPickupX, nBombPickupY, PIXEL_SOLID, FG_GREY);
		}


		fHighScore = max(fHighScore, fSurvivalTime);

		// Draw Player
		Draw(nPlayerX, nPlayerY, L'P', FG_CYAN);

		DrawString(2, ScreenHeight() - 2, L"Survived: " + to_wstring((int)fSurvivalTime));
		DrawString(18, ScreenHeight() - 2, L"Bombs: " + to_wstring(nBoomsticks));
		DrawString(30, ScreenHeight() - 2, L"Health: " + to_wstring(nPlayerHealth));
		DrawString(45, ScreenHeight() - 2, L"High Score: " + to_wstring((int)fHighScore));

		return true;
	}

};




int main()
{
	LudumDare42 demo;
	if (demo.ConstructConsole(60, 63, 14, 14))
		demo.Start();
	return 0;
}

