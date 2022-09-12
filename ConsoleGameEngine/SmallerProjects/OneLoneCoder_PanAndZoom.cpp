/*
OneLoneCoder.com - Programming Panning & Zooming
"Left a bit, bit more, there..." - @Javidx9

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
Used in absolutely everything now, panning and zooming is an important
tool to know how to implement. Even though it appears simple, getting
your head around different spatial transforms can be challenging!

Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV

Video:
~~~~~~
https://youtu.be/ZQ8qtAizis4

Last Updated: 10/06/2018
*/

#include "olcConsoleGameEngine.h"
#include <string>

class OneLoneCoder_PanAndZoom : public olcConsoleGameEngine
{
public:
	OneLoneCoder_PanAndZoom()
	{
		m_sAppName = L"Pan And Zoom";
	}

private:

	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	float fStartPanX = 0.0f;
	float fStartPanY = 0.0f;

	float fSelectedCellX = 0.0f;
	float fSelectedCellY = 0.0f;


protected:

	virtual bool OnUserCreate()
	{		
		// Initialise offset so 0,0 in world space is middle of the screen
		fOffsetX = -ScreenWidth() / 2;
		fOffsetY = -ScreenHeight() / 2;
		return true;
	}

	// Convert coordinates from World Space --> Screen Space
	void WorldToScreen(float fWorldX, float fWorldY, int &nScreenX, int &nScreenY)
	{
		nScreenX = (int)((fWorldX - fOffsetX) * fScaleX);
		nScreenY = (int)((fWorldY - fOffsetY) * fScaleY);
	}

	// Convert coordinates from Screen Space --> World Space
	void ScreenToWorld(int nScreenX, int nScreenY, float &fWorldX, float &fWorldY)
	{
		fWorldX = ((float)nScreenX / fScaleX) + fOffsetX;
		fWorldY = ((float)nScreenY / fScaleY) + fOffsetY;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Just grab a copy of mouse coordinates for convenience
		float fMouseX = (float)GetMouseX();
		float fMouseY = (float)GetMouseY();

		// For panning, we need to capture the screen location when the user starts
		// to pan...
		if (GetMouse(2).bPressed)
		{
			fStartPanX = fMouseX;
			fStartPanY = fMouseY;
		}

		// ...as the mouse moves, the screen location changes. Convert this screen
		// coordinate change into world coordinates to implement the pan. Simples.
		if (GetMouse(2).bHeld)
		{
			fOffsetX -= (fMouseX - fStartPanX) / fScaleX;
			fOffsetY -= (fMouseY - fStartPanY) / fScaleY;

			// Start "new" pan for next epoch
			fStartPanX = fMouseX;
			fStartPanY = fMouseY;
		}

		// For zoom, we need to extract the location of the cursor before and after the
		// scale is changed. Here we get the cursor and translate into world space...
		float fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom);


		// ...change the scale as required...
		if (GetKey(L'Q').bHeld)
		{
			fScaleX *= 1.001f;
			fScaleY *= 1.001f;
		}

		if (GetKey(L'A').bHeld)
		{
			fScaleX *= 0.999f;
			fScaleY *= 0.999f;
		}

		// ...now get the location of the cursor in world space again - It will have changed
		// because the scale has changed, but we can offset our world now to fix the zoom
		// location in screen space, because we know how much it changed laterally between
		// the two spatial scales. Neat huh? ;-)
		float fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom);		
		fOffsetX += (fMouseWorldX_BeforeZoom - fMouseWorldX_AfterZoom);
		fOffsetY += (fMouseWorldY_BeforeZoom - fMouseWorldY_AfterZoom);

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


		// Clip
		float fWorldLeft, fWorldTop, fWorldRight, fWorldBottom;
		ScreenToWorld(0, 0, fWorldLeft, fWorldTop);
		ScreenToWorld(ScreenWidth(), ScreenHeight(), fWorldRight, fWorldBottom);

		auto function = [](float x)
		{
			return sinf(x);
		};


		// Draw Main Axes a 10x10 Unit Grid
		// Draw 10 horizontal lines
		int nLinesDrawn = 0;
		for (float y = 0.0f; y <= 10.0f; y++)
		{
			if (y >= fWorldTop && y <= fWorldBottom)
			{
				float sx = 0.0f, sy = y;
				float ex = 10.0f, ey = y;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
				nLinesDrawn++;
			}
		}

		// Draw 10 vertical lines
		for (float x = 0.0f; x <= 10.0f; x++)
		{
			if (x >= fWorldLeft && x <= fWorldRight)
			{
				float sx = x, sy = 0.0f;
				float ex = x, ey = 10.0f;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
				nLinesDrawn++;
			}
		}

		// Draw selected cell

		// We can easily determine where the mouse is in world space. In fact we already
		// have this frame so just reuse the values
		if (GetMouse(1).bReleased)
		{
			fSelectedCellX = (int)fMouseWorldX_AfterZoom;
			fSelectedCellY = (int)fMouseWorldY_AfterZoom;
		}

		// Draw selected cell by filling with red circle. Convert cell coords
		// into screen space, also scale the radius
		int cx, cy, cr;
		WorldToScreen(fSelectedCellX + 0.5f, fSelectedCellY + 0.5f, cx, cy);
		cr = 0.3f * fScaleX;
		FillCircle(cx, cy, cr, PIXEL_SOLID, FG_RED);
		DrawString(2, 2, L"Lines Drawn: " + to_wstring(nLinesDrawn));


		// Draw Chart
		float fWorldPerScreenWidthPixel = (fWorldRight - fWorldLeft) / ScreenWidth();
		float fWorldPerScreenHeightPixel = (fWorldBottom - fWorldTop) / ScreenHeight();
		int px, py, opx = 0, opy = 0;
		WorldToScreen(fWorldLeft-fWorldPerScreenWidthPixel, -function((fWorldLeft - fWorldPerScreenWidthPixel) - 5.0f) + 5.0f, opx, opy);
		for (float x = fWorldLeft; x < fWorldRight; x+=fWorldPerScreenWidthPixel)
		{
			float y = -function(x - 5.0f) + 5.0f;
			WorldToScreen(x, y, px, py);
			DrawLine(opx, opy, px, py, PIXEL_SOLID, FG_GREEN);
			opx = px;
			opy = py;
		}
		
		return true;
	}
};


int main()
{
	OneLoneCoder_PanAndZoom demo;
	demo.ConstructConsole(160, 100, 8, 8);
	demo.Start();
	return 0;
}