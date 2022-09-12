/*
OneLoneCoder.com - Command Line Video
"Yuck..." - @Javidx9

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
I was interested in trying to recreate video that is displayed using font
characters at the command line. Mixing the colours is quite challenging
given the constraints of the palette and characters.

You'll need the ESCAPI libary binaries!
https://github.com/jarikomppa/escapi


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/pk1Y_26j1Y4

Last Updated: 02/10/2017
*/

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"
#include "escapi.h"

class OneLoneCoder_Video : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Video()
	{
		m_sAppName = L"Video";
	}

private:
	int nCameras = 0;
	SimpleCapParams capture;

protected:
	virtual bool OnUserCreate()
	{
		nCameras = setupESCAPI();

		if (nCameras == 0)
			return false;

		capture.mWidth = ScreenWidth();
		capture.mHeight = ScreenHeight();
		capture.mTargetBuf = new int[ScreenWidth() * ScreenHeight()];

		if (initCapture(0, &capture) == 0)
			return false;

		return true;
	}

	void ClassifyPixel_Grey(float r, float g, float b, wchar_t &sym, short &fg_col, short &bg_col)
	{
		float luminance = 0.2987f * r + 0.5870f * g + 0.1140f * b;
		int pixel_bw = (int)(luminance * 13.0f);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Taken from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
	typedef struct
	{
		float r;       // a fraction between 0 and 1
		float g;       // a fraction between 0 and 1
		float b;       // a fraction between 0 and 1
	} rgb;

	typedef struct
	{
		float h;       // angle in degrees
		float s;       // a fraction between 0 and 1
		float v;       // a fraction between 0 and 1
	} hsv;

	hsv rgb2hsv(rgb in)
	{
		hsv         out;
		float      min, max, delta;

		min = in.r < in.g ? in.r : in.g;
		min = min  < in.b ? min : in.b;

		max = in.r > in.g ? in.r : in.g;
		max = max  > in.b ? max : in.b;

		out.v = max;                                // v
		delta = max - min;
		if (delta < 0.00001f)
		{
			out.s = 0;
			out.h = 0; // undefined, maybe nan?
			return out;
		}
		if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
			out.s = (delta / max);                  // s
		}
		else {
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			out.s = 0.0;
			out.h = NAN;                            // its now undefined
			return out;
		}
		if (in.r >= max)                           // > is bogus, just keeps compilor happy
			out.h = (in.g - in.b) / delta;        // between yellow & magenta
		else
			if (in.g >= max)
				out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
			else
				out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

		out.h *= 60.0;                              // degrees

		if (out.h < 0.0)
			out.h += 360.0;

		return out;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ClassifyPixel_HSL(float r, float g, float b, wchar_t &sym, short &fg_col, short &bg_col)
	{
		hsv col = rgb2hsv({ r, g, b });

		const struct { wchar_t c; short fg; short bg; } hues[] =
		{
			{ PIXEL_SOLID,				FG_RED | BG_RED },
			{ PIXEL_QUARTER,			FG_YELLOW | BG_RED },
			{ PIXEL_HALF,				FG_YELLOW | BG_RED },
			{ PIXEL_THREEQUARTERS,		FG_YELLOW | BG_RED },

			{ PIXEL_SOLID,				FG_GREEN | BG_YELLOW },
			{ PIXEL_QUARTER,			FG_GREEN | BG_YELLOW },
			{ PIXEL_HALF,				FG_GREEN | BG_YELLOW },
			{ PIXEL_THREEQUARTERS,		FG_GREEN | BG_YELLOW },

			{ PIXEL_SOLID,				FG_CYAN | BG_GREEN },
			{ PIXEL_QUARTER,			FG_CYAN | BG_GREEN },
			{ PIXEL_HALF,				FG_CYAN | BG_GREEN },
			{ PIXEL_THREEQUARTERS,		FG_CYAN | BG_GREEN },

			{ PIXEL_SOLID,				FG_BLUE | BG_CYAN },
			{ PIXEL_QUARTER,			FG_BLUE | BG_CYAN },
			{ PIXEL_HALF,				FG_BLUE | BG_CYAN },
			{ PIXEL_THREEQUARTERS,		FG_BLUE | BG_CYAN },

			{ PIXEL_SOLID,				FG_MAGENTA | BG_BLUE },
			{ PIXEL_QUARTER,			FG_MAGENTA | BG_BLUE },
			{ PIXEL_HALF,				FG_MAGENTA | BG_BLUE },
			{ PIXEL_THREEQUARTERS,		FG_MAGENTA | BG_BLUE },

			{ PIXEL_SOLID,				FG_RED | BG_MAGENTA },
			{ PIXEL_QUARTER,			FG_RED | BG_MAGENTA },
			{ PIXEL_HALF,				FG_RED | BG_MAGENTA },
			{ PIXEL_THREEQUARTERS,		FG_RED | BG_MAGENTA },

		};

		int index = (int)((col.h / 360.0f) * 24.0f);

		if (col.s > 0.2f)
		{
			sym = hues[index].c;
			fg_col = hues[index].fg;
			bg_col = hues[index].bg;
		}
		else
			ClassifyPixel_Grey(r, g, b, sym, fg_col, bg_col);
	}

	void ClassifyPixel_OLC(float r, float g, float b, wchar_t &sym, short &fg_col, short &bg_col)
	{
		// Is pixel coloured (i.e. RGB values exhibit significant variance)
		float fMean = (r + g + b) / 3.0f;
		float fRVar = (r - fMean)*(r - fMean);
		float fGVar = (g - fMean)*(g - fMean);
		float fBVar = (b - fMean)*(b - fMean);
		float fVariance = fRVar + fGVar + fBVar;

		if (fVariance < 0.0001f)
		{
			ClassifyPixel_Grey(r, g, b, sym, fg_col, bg_col);
		}
		else
		{
			// Pixel has colour so get dominant colour
			float y = min(r, g);// (r * g);
			float c = min(g, b);// (g * b);
			float m = min(b, r);// (b * r);

			float fMean2 = (y + c + m) / 3.0f;
			float fYVar = (y - fMean2)*(y - fMean2);
			float fCVar = (c - fMean2)*(c - fMean2);
			float fMVar = (m - fMean2)*(m - fMean2);

			float fMaxPrimaryVar = max(fRVar, fGVar);
			fMaxPrimaryVar = max(fMaxPrimaryVar, fBVar);

			float fMaxSecondaryVar = max(fCVar, fYVar);
			fMaxSecondaryVar = max(fMaxSecondaryVar, fMVar);

			float fShading = 0.5f;

			auto compare = [&](float fV1, float fV2, float fC1, float fC2, short FG_LIGHT, short FG_DARK, short BG_LIGHT, short BG_DARK)
			{
				if (fV1 >= fV2)
				{
					// Primary Is Dominant, Use in foreground
					fg_col = fC1 > 0.5f ? FG_LIGHT : FG_DARK;

					if (fV2 < 0.0001f)
					{
						// Secondary is not variant, Use greyscale in background
						if (fC2 >= 0.00f && fC2 < 0.25f) bg_col = BG_BLACK;
						if (fC2 >= 0.25f && fC2 < 0.50f) bg_col = BG_DARK_GREY;
						if (fC2 >= 0.50f && fC2 < 0.75f) bg_col = BG_GREY;
						if (fC2 >= 0.75f && fC2 <= 1.00f) bg_col = BG_WHITE;
					}
					else
					{
						// Secondary is varient, Use in background
						bg_col = fC2 > 0.5f ? BG_LIGHT : BG_DARK;
					}

					// Shade dominant over background (100% -> 0%)
					fShading = ((fC1 - fC2) / 2.0f) + 0.5f;
				}
				
				if (fShading >= 0.00f && fShading < 0.20f) sym = L' ';
				if (fShading >= 0.20f && fShading < 0.40f) sym = PIXEL_QUARTER;
				if (fShading >= 0.40f && fShading < 0.60f) sym = PIXEL_HALF;
				if (fShading >= 0.60f && fShading < 0.80f) sym = PIXEL_THREEQUARTERS;
				if (fShading >= 0.80f) sym = PIXEL_SOLID;
			};

			if (fRVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
				compare(fRVar, fYVar, r, y, FG_RED, FG_DARK_RED, BG_YELLOW, BG_DARK_YELLOW);
			
			if (fRVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
				compare(fRVar, fMVar, r, m, FG_RED, FG_DARK_RED, BG_MAGENTA, BG_DARK_MAGENTA);

			if (fRVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
				compare(fRVar, fCVar, r, c, FG_RED, FG_DARK_RED, BG_CYAN, BG_DARK_CYAN);

			if (fGVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
				compare(fGVar, fYVar, g, y, FG_GREEN, FG_DARK_GREEN, BG_YELLOW, BG_DARK_YELLOW);

			if (fGVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
				compare(fGVar, fCVar, g, c, FG_GREEN, FG_DARK_GREEN, BG_CYAN, BG_DARK_CYAN);

			if (fGVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
				compare(fGVar, fMVar, g, m, FG_GREEN, FG_DARK_GREEN, BG_MAGENTA, BG_DARK_MAGENTA);

			if (fBVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
				compare(fBVar, fMVar, b, m, FG_BLUE, FG_DARK_BLUE, BG_MAGENTA, BG_DARK_MAGENTA);

			if (fBVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
				compare(fBVar, fCVar, b, c, FG_BLUE, FG_DARK_BLUE, BG_CYAN, BG_DARK_CYAN);

			if (fBVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
				compare(fBVar, fYVar, b, y, FG_BLUE, FG_DARK_BLUE, BG_YELLOW, BG_DARK_YELLOW);
		}

	}



	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Capture webcam image
		doCapture(0); while (isCaptureDone(0) == 0) {}

		for (int x = 0; x < capture.mWidth; x++)
			for (int y = 0; y < capture.mHeight; y++)
			{
				// Get pixel
				union RGBint
				{
					int rgb;
					unsigned char c[4];
				};

				RGBint col;
				int id = y * capture.mWidth + x;

				col.rgb = capture.mTargetBuf[id];
				int r = col.c[2];
				int g = col.c[1];
				int b = col.c[0];

				float fR = (float)r / 255.0f;
				float fG = (float)g / 255.0f;
				float fB = (float)b / 255.0f;

				// Convert into character/colour combo
				wchar_t sym;
				short bg_col = 0;
				short fg_col = 0;

				//ClassifyPixel_Grey(fR, fG, fB, sym, fg_col, bg_col);
				//ClassifyPixel_HSL(fR, fG, fB, sym, fg_col, bg_col);
				ClassifyPixel_OLC(fR, fG, fB, sym, fg_col, bg_col);

				// Draw pixel
				Draw(x, y, sym, bg_col | fg_col);
				

			}


		return true;
	}

};

int main()
{
	OneLoneCoder_Video game;
	game.ConstructConsole(320, 240, 4, 4);
	game.Start();
	return 0;
}
