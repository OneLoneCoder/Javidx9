/*
OneLoneCoder.com - What Is Perlin Noise?
"Mountains, Clouds, Worms Landscapes?" - @Javidx9

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
Noise is random - a bad thing when trying to procedurally generate content. Perlin
noise adds coherence at varying spatial scales which create natural looking noise
arrays. Perlin noise can be further processed into all sorts of assets, such as
mountains, maps, rooms, textures, data sets.


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/6-0UaeJBumA

Last Updated: 29/10/2017
*/

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class OneLoneCoder_PerlinNoiseDemo : public olcConsoleGameEngine
{
public:
	OneLoneCoder_PerlinNoiseDemo()
	{
		m_sAppName = L"Perlin Noise";
	}

private:
	// 2D noise variables
	int nOutputWidth = 256;
	int nOutputHeight = 256;
	float *fNoiseSeed2D = nullptr;
	float *fPerlinNoise2D = nullptr;

	// 1D noise variables
	float *fNoiseSeed1D = nullptr;
	float *fPerlinNoise1D = nullptr;
	int nOutputSize = 256;


	int nOctaveCount = 1;
	float fScalingBias = 2.0f;
	int nMode = 1;
	

	virtual bool OnUserCreate()
	{
		nOutputWidth = ScreenWidth();
		nOutputHeight = ScreenHeight();
	
		fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
		fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];
		for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

		nOutputSize = ScreenWidth();
		fNoiseSeed1D = new float[nOutputSize];
		fPerlinNoise1D = new float[nOutputSize];
		for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

		if (m_keys[VK_SPACE].bReleased)
			nOctaveCount++;

		if (m_keys[L'1'].bReleased)
			nMode = 1;

		if (m_keys[L'2'].bReleased)
			nMode = 2;

		if (m_keys[L'3'].bReleased)
			nMode = 3;

		if (m_keys[L'Q'].bReleased)
			fScalingBias += 0.2f;

		if (m_keys[L'A'].bReleased)
			fScalingBias -= 0.2f;

		if (fScalingBias < 0.2f)
			fScalingBias = 0.2f;

		if (nOctaveCount == 9)
			nOctaveCount = 1;

		if (nMode == 1) // 1D Noise
		{
			if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

			if (m_keys[L'X'].bReleased) // Noise Between -1 and +1
				for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;

			PerlinNoise1D(nOutputSize, fNoiseSeed1D, nOctaveCount, fScalingBias, fPerlinNoise1D);

			for (int x = 0; x < nOutputSize; x++)
			{
				int y = -(fPerlinNoise1D[x] * (float)ScreenHeight() / 2.0f) + (float)ScreenHeight() / 2.0f;
				if (y < ScreenHeight() / 2)
					for (int f = y; f < ScreenHeight() / 2; f++)
						Draw(x, f, PIXEL_SOLID, FG_GREEN);
				else
					for (int f = ScreenHeight() / 2; f <= y; f++)
						Draw(x, f, PIXEL_SOLID, FG_RED);
			}
		}

		if (nMode == 2) // 2D Noise
		{
			if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
			{
				for (int y = 0; y < nOutputHeight; y++)
				{
					short bg_col, fg_col;
					wchar_t sym;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 12.0f);
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

					Draw(x, y, sym, fg_col | bg_col);
				}
			}
		}

		if (nMode == 3) // 2D Noise - colourised
		{
			if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
			{
				for (int y = 0; y < nOutputHeight; y++)
				{
					short bg_col, fg_col;
					wchar_t sym;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 16.0f);
					switch (pixel_bw)
					{
					case 0: bg_col = BG_DARK_BLUE; fg_col = FG_DARK_BLUE; sym = PIXEL_SOLID; break;

					case 1: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_QUARTER; break;
					case 2: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_HALF; break;
					case 3: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_THREEQUARTERS; break;
					case 4: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_SOLID; break;

					case 5: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_QUARTER; break;
					case 6: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_HALF; break;
					case 7: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_THREEQUARTERS; break;
					case 8: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_SOLID; break;

					case 9:  bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
					case 10: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
					case 11: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 12: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

					case 13: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 14: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
					case 15: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 16: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
					}

					Draw(x, y, sym, fg_col | bg_col);
				}
			}
		}

		return true;
	}


	void PerlinNoise1D(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nCount; x++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nCount >> o;
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;

				float fBlend = (float)(x - nSample1) / (float)nPitch;

				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

				fScaleAcc += fScale;
				fNoise += fSample * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[x] = fNoise / fScaleAcc;
		}
	}

	void PerlinNoise2D(int nWidth, int nHeight, float *fSeed, int nOctaves, float fBias, float *fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nWidth; x++)
			for (int y = 0; y < nHeight; y++)
			{				
				float fNoise = 0.0f;
				float fScaleAcc = 0.0f;
				float fScale = 1.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;
					
					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;					
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
					float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}

				// Scale to seed range
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}
	
	}


};


int main()
{
	OneLoneCoder_PerlinNoiseDemo game;
	game.ConstructConsole(256, 256, 3, 3);
	game.Start();
	return 0;
}
