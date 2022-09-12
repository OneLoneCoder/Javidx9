/*
OneLoneCoder.com - Augmenting Reality #1 - Optical Flow
"My arms are tired now." - @Javidx9

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
Optical flow is the determination of motion in a video stream at the pixel level.
Each pixel is associated with a motion vector that is used to create a map of
velocity vectors which are then used to interact with a virtual object superimposed
on the video stream.

You will need to have watched my webcam video for this one to make sense!
https://youtu.be/pk1Y_26j1Y4

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/aNtzgoEGC1Y

Last Updated: 15/11/2017
*/
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"
#include "escapi.h"

class OneLoneCoder_AROpticFlow : public olcConsoleGameEngine
{
public:
	OneLoneCoder_AROpticFlow()
	{
		m_sAppName = L"Augmented Reality Part #1 - Optic Flow";
	}

private:
	union RGBint
	{
		int rgb;
		unsigned char c[4];
	};

	int nCameras = 0;
	SimpleCapParams capture;

	// 2D Maps for image processing
	float *fOldCamera = nullptr;		// Previous raw frame from camera
	float *fNewCamera = nullptr;		// Recent raw frame from camera
	float *fFilteredCamera = nullptr;	// low-pass filtered image
	float *fOldFilteredCamera = nullptr;	// low-pass filtered image
	float *fOldMotionImage = nullptr;	// previous motion image
	float *fMotionImage = nullptr;		// recent motion image
	float *fFlowX = nullptr;			// x-component of flow field vector
	float *fFlowY = nullptr;			// y-component of flow field vector

	// Object Physics Variables
	float fBallX = 0.0f;				// Ball position 2D
	float fBallY = 0.0f;
	float fBallVX = 0.0f;				// Ball Velocity 2D
	float fBallVY = 0.0f;

protected:
	virtual bool OnUserCreate()
	{
		// Initialise webcam to console dimensions
		nCameras = setupESCAPI();
		if (nCameras == 0)	return false;
		capture.mWidth = ScreenWidth();
		capture.mHeight = ScreenHeight();
		capture.mTargetBuf = new int[ScreenWidth() * ScreenHeight()];
		if (initCapture(0, &capture) == 0)	return false;

		// Allocate memory for images
		fOldCamera		= new float[ScreenWidth() * ScreenHeight()];
		fNewCamera		= new float[ScreenWidth() * ScreenHeight()];
		fFilteredCamera = new float[ScreenWidth() * ScreenHeight()];
		fOldFilteredCamera = new float[ScreenWidth() * ScreenHeight()];
		fFlowX			= new float[ScreenWidth() * ScreenHeight()];
		fFlowY			= new float[ScreenWidth() * ScreenHeight()];
		fOldMotionImage = new float[ScreenWidth() * ScreenHeight()];
		fMotionImage	= new float[ScreenWidth() * ScreenHeight()];

		// Initialise images to 0
		memset(fOldCamera,		0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fNewCamera,		0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fFilteredCamera, 0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fOldFilteredCamera, 0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fFlowX,			0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fFlowY,			0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fOldMotionImage, 0, sizeof(float) * ScreenWidth() * ScreenHeight());
		memset(fMotionImage,	0, sizeof(float) * ScreenWidth() * ScreenHeight());
		
		// Set ball position to middle of frame
		fBallX = ScreenWidth() / 2.0f;
		fBallY = ScreenHeight() / 2.0f;
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Lambda function to draw "image" in greyscale
		auto draw_image = [&](float *image)
		{
			for (int x = 0; x < capture.mWidth; x++)
			{
				for (int y = 0; y < capture.mHeight; y++)
				{
					wchar_t sym = 0;
					short bg_col = 0;
					short fg_col = 0;
					int pixel_bw = (int)(image[y*ScreenWidth() + x] * 13.0f);
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
					Draw(x, y, sym, bg_col | fg_col);
				}
			}
		};

		// Lambda function to read from a 2D array without error
		auto get_pixel = [&](float* image, int x, int y)
		{
			if (x >= 0 && x < ScreenWidth() && y >= 0 && y < ScreenHeight())
				return image[y*ScreenWidth() + x];
			else
				return 0.0f;
		};

		// === Capture & Filter New Input Image ==========================================
		
		// Get Image from webcam
		doCapture(0); while (isCaptureDone(0) == 0) {}

		// Do Temporal Filtering per pixel
		for (int y = 0; y < capture.mHeight; y++)
			for (int x = 0; x < capture.mWidth; x++)
			{
				RGBint col;
				int id = y * capture.mWidth + x;
				col.rgb = capture.mTargetBuf[id];
				int r = col.c[2], g = col.c[1], b = col.c[0];					
				float fR = (float)r / 255.0f;
				float fG = (float)g / 255.0f;
				float fB = (float)b / 255.0f;
				
				// Store previous camera frame for temporal processing
				fOldCamera[y*ScreenWidth() + x] = fNewCamera[y*ScreenWidth() + x];

				// Store previous camera frame for temporal processing
				fOldFilteredCamera[y*ScreenWidth() + x] = fFilteredCamera[y*ScreenWidth() + x];

				// Store previous motion only frame
				fOldMotionImage[y*ScreenWidth() + x] = fMotionImage[y*ScreenWidth() + x];

				// Calculate luminance (greyscale equivalent) of pixel
				float fLuminance = 0.2987f * fR + 0.5870f * fG + 0.1140f * fB;
				fNewCamera[y*ScreenWidth() + x] = fLuminance;
								
				// Low-Pass filter camera image, to remove pixel jitter
				fFilteredCamera[y*ScreenWidth() + x] += (fNewCamera[y*ScreenWidth() + x] - fFilteredCamera[y*ScreenWidth() + x]) * 0.8f;

				// Create motion image as difference between two successive camera frames
				float fDiff = fabs(get_pixel(fFilteredCamera, x, y) - get_pixel(fOldFilteredCamera, x, y));

				// Threshold motion image to remove filter out camera noise
				fMotionImage[y*ScreenWidth() + x] = (fDiff >= 0.05f) ? fDiff : 0.0f;
			}
		
		// === Calculate Optic Flow Vector Map ==========================================

		// Brute Force Local Spatial Pattern Matching
		int nPatchSize = 9;
		int nSearchSize = 7;	

		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
			{
				// Initialise serach variables
				float fPatchDifferenceMax = INFINITY;
				float fPatchDifferenceX = 0.0f;
				float fPatchDifferenceY = 0.0f;
				fFlowX[y*ScreenWidth() + x] = 0.0f;
				fFlowY[y*ScreenWidth() + x] = 0.0f;

				// Search over a given rectangular area for a "patch" of old image
				// that "resembles" a patch of the new image.
				for (int sx = 0; sx < nSearchSize; sx++)
				{
					for (int sy = 0; sy < nSearchSize; sy++)
					{
						// Search vector is centre of patch test
						int nSearchVectorX = x + (sx - nSearchSize / 2);
						int nSearchVectorY = y + (sy - nSearchSize / 2);

						float fAccumulatedDifference = 0.0f;

						// For each pixel in search patch, accumulate difference with base patch						
						for (int px = 0; px < nPatchSize; px++)
							for (int py = 0; py < nPatchSize; py++)
							{
								// Work out search patch offset indices
								int nPatchPixelX = nSearchVectorX + (px - nPatchSize / 2);
								int nPatchPixelY = nSearchVectorY + (py - nPatchSize / 2);

								// Work out base patch indices
								int nBasePixelX = x + (px - nPatchSize / 2);
								int nBasePixelY = y + (py - nPatchSize / 2);

								// Get adjacent values for each patch
								float fPatchPixel = get_pixel(fNewCamera, nPatchPixelX, nPatchPixelY);
								float fBasePixel = get_pixel(fOldCamera, nBasePixelX, nBasePixelY);

								// Accumulate difference
								fAccumulatedDifference += fabs(fPatchPixel - fBasePixel);
							}

						// Record the vector offset for the search patch that is the
						// least different to the base patch
						if (fAccumulatedDifference <= fPatchDifferenceMax)
						{
							fPatchDifferenceMax = fAccumulatedDifference;
							fFlowX[y*ScreenWidth() + x] = (float)(nSearchVectorX - x);
							fFlowY[y*ScreenWidth() + x] = (float)(nSearchVectorY - y);
						}
					}
				}				
			}
		}

		// Modulate Optic Flow Vector Map with motion map, to remove vectors that
		// errornously indicate large local motion
		for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
		{
			fFlowX[i] *= fMotionImage[i] > 0 ? 1.0f : 0.0f;
			fFlowY[i] *= fMotionImage[i] > 0 ? 1.0f : 0.0f;
		}

		// === Update Ball Physics ========================================================

		// Ball velocity is updated by optic flow vector field
		fBallVX += 100.0f * fFlowX[(int)fBallY * ScreenWidth() + (int)fBallX] * fElapsedTime;
		fBallVY += 100.0f * fFlowY[(int)fBallY * ScreenWidth() + (int)fBallX] * fElapsedTime;

		// Ball position is updated by velocity
		fBallX += 1.0f * fBallVX * fElapsedTime;
		fBallY += 1.0f * fBallVY * fElapsedTime;

		// Add "drag" effect to ball velocity
		fBallVX *= 0.85f;
		fBallVY *= 0.85f;

		// Wrap ball around screen
		if (fBallX >= ScreenWidth()) fBallX -= (float)ScreenWidth();
		if (fBallY >= ScreenHeight()) fBallY -= (float)ScreenHeight();
		if (fBallX < 0) fBallX += (float)ScreenWidth();
		if (fBallY < 0) fBallY += (float)ScreenHeight();

		// === Update Screen =================================================================

		// Draw Camera Image
		draw_image(fNewCamera);

		// Draw "Ball"
		Fill(fBallX - 4, fBallY - 4, fBallX + 4, fBallY + 4,  PIXEL_SOLID, FG_RED);
		return true;
	}
};

int main()
{
	OneLoneCoder_AROpticFlow game;
	game.ConstructConsole(80, 60, 16, 16);
	game.Start();
	return 0;
}
