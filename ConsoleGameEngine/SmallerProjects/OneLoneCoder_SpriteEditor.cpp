/*
OneLoneCoder.com - Sprite Editor
"Stop Crying about Paint ya big baby" - @Javidx9

License
~~~~~~~
Copyright (C) 2018  Javidx9
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
Editing ASCII is not a simple as it should be, especially if you want to
include all the weird characters

Controls
~~~~~~~~


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
Several...

Last Updated:
*/

#include <iostream>
#include <string>
using namespace std;

#include "olcConsoleGameEngine.h"





class OneLoneCoder_SpriteEditor : public olcConsoleGameEngine
{
public:
	OneLoneCoder_SpriteEditor()
	{
		m_sAppName = L"Sprite Editor";
	}

private:
	int nPosX = 0;
	int nPosY = 0;
	int nOffsetX = 0;
	int nOffsetY = 0;
	int nZoom = 4;
	int nCurrentGlyph = PIXEL_SOLID;
	int nCurrentColourFG = FG_RED;
	int nCurrentColourBG = FG_BLACK;

	olcSprite *sprite = nullptr;
	wstring sCurrentSpriteFile;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		sprite = new olcSprite(8, 32);
		sCurrentSpriteFile = L"fps_fireball1.spr";

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{

		// Zooming 
		if (m_keys[VK_PRIOR].bReleased)
			nZoom <<= 1;

		if (m_keys[VK_NEXT].bReleased)
			nZoom >>= 1;

		if (nZoom > 32) nZoom = 32;
		if (nZoom < 2) nZoom = 2;

		// Brushes
		if (m_keys[VK_F1].bReleased) nCurrentGlyph = PIXEL_SOLID;
		if (m_keys[VK_F2].bReleased) nCurrentGlyph = PIXEL_THREEQUARTERS;
		if (m_keys[VK_F3].bReleased) nCurrentGlyph = PIXEL_HALF;
		if (m_keys[VK_F4].bReleased) nCurrentGlyph = PIXEL_QUARTER;

		// Colours
		for (int i = 0; i < 8; i++)
			if (m_keys[L"01234567"[i]].bReleased)
				if (m_keys[VK_SHIFT].bHeld)
					nCurrentColourFG = i + 8;
				else
					nCurrentColourFG = i;


		if (m_keys[VK_F7].bReleased)
			nCurrentColourBG--;

		if (m_keys[VK_F8].bReleased)
			nCurrentColourBG++;

		if (nCurrentColourBG < 0) nCurrentColourBG = 15;
		if (nCurrentColourBG > 15) nCurrentColourBG = 0;

		// Cursing :-)
		if (m_keys[VK_SHIFT].bHeld)
		{
			if (m_keys[VK_UP].bReleased) nOffsetY++;
			if (m_keys[VK_DOWN].bReleased) nOffsetY--;
			if (m_keys[VK_LEFT].bReleased) nOffsetX++;
			if (m_keys[VK_RIGHT].bReleased) nOffsetX--;
		}
		else
		{
			if (m_keys[VK_UP].bReleased) nPosY--;
			if (m_keys[VK_DOWN].bReleased) nPosY++;
			if (m_keys[VK_LEFT].bReleased) nPosX--;
			if (m_keys[VK_RIGHT].bReleased) nPosX++;
		}

		if (sprite != nullptr)
		{
			if (nPosX < 0) nPosX = 0;
			if (nPosX >= sprite->nWidth) nPosX = sprite->nWidth - 1;
			if (nPosY < 0) nPosY = 0;
			if (nPosY >= sprite->nHeight) nPosY = sprite->nHeight - 1;

			if (m_keys[VK_SPACE].bReleased)
			{
				sprite->SetGlyph(nPosX - 0, nPosY - 0, nCurrentGlyph);
				sprite->SetColour(nPosX - 0, nPosY - 0, nCurrentColourFG | (nCurrentColourBG << 4));
			}

			if (m_keys[VK_DELETE].bReleased)
			{
				sprite->SetGlyph(nPosX - 0, nPosY - 0, L' ');
				sprite->SetColour(nPosX - 0, nPosY - 0, 0);
			}

			if (m_keys[VK_F9].bReleased)
			{
				sprite->Load(sCurrentSpriteFile);
			}

			if (m_keys[VK_F10].bReleased)
			{
				sprite->Save(sCurrentSpriteFile);
			}
		}





		// Erase All
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

		// Draw Menu
		DrawString(1, 1, L"F1 = 100%  F2 = 75%  F3 = 50%  F4 = 25%    F9 = Load File  F10 = Save File");
		for (int i = 0; i < 8; i++)
		{
			DrawString(1 + 6 * i, 3, to_wstring(i) + L" = ");
			if (m_keys[VK_SHIFT].bHeld)
				Draw(1 + 6 * i + 4, 3, PIXEL_SOLID, (i + 8));
			else
				Draw(1 + 6 * i + 4, 3, PIXEL_SOLID, (i));
		}

		DrawString(1, 5, L"Current Brush = ");
		Draw(18, 5, nCurrentGlyph, nCurrentColourFG | (nCurrentColourBG << 4));

		// Draw Canvas
		for (int x = 9; x < 138; x++)
		{
			Draw(x, 9);
			Draw(x, 74);
		}

		for (int y = 9; y < 75; y++)
		{
			Draw(9, y);
			Draw(138, y);
		}

		// Draw Visible Sprite
		if (sprite != nullptr)
		{
			int nVisiblePixelsX = 128 / nZoom;
			int nVisiblePixelsY = 64 / nZoom;

			for (int x = 0; x < nVisiblePixelsX; x++)
				for (int y = 0; y < nVisiblePixelsY; y++)
				{
					if (x - nOffsetX < sprite->nWidth && y - nOffsetY < sprite->nHeight && x - nOffsetX >= 0 && y - nOffsetY >= 0)
					{
						// Draw Sprite
						Fill(x * nZoom + 10, y*nZoom + 10, (x + 1)*nZoom + 10, (y + 1)*nZoom + 10, sprite->GetGlyph(x - nOffsetX, y - nOffsetY), sprite->GetColour(x - nOffsetX, y - nOffsetY));


						// Draw Pixel Markers
						if (sprite->GetGlyph(x - nOffsetX, y - nOffsetY) == L' ')
							Draw((x)* nZoom + 10, (y)* nZoom + 10, L'.');
					}

					if (x - nOffsetX == nPosX && y - nOffsetY == nPosY)
						Draw((x)* nZoom + 10, (y)* nZoom + 10, L'O');
				}
		}


		// Draw Actual Sprite
		for (int x = 0; x < sprite->nWidth; x++)
			for (int y = 0; y < sprite->nHeight; y++)
				Draw(x + 10, y + 80, sprite->GetGlyph(x, y), sprite->GetColour(x, y));



		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_SpriteEditor game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}
