/*
OneLoneCoder.com - Programming Racing Lines
"Brake! Brake! Hard Left! " - @Javidx9

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
Algorithmically generating a racing line is quite tricky. This simple framework 
allows me to explore different methods. Use mouse to drag points, and A & S keys
to change the number of iterations.

Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV

Video:
~~~~~~
https://youtu.be/FlieT66N9OM

Last Updated: 27/05/2018
*/

#include <iostream>
#include <string>
using namespace std;

#include "olcConsoleGameEngine.h"

// See Programming Splines! Videos
struct sPoint2D
{
	float x;
	float y;
	float length;
};

struct sSpline
{
	vector<sPoint2D> points;
	float fTotalSplineLength = 0.0f;
	bool bIsLooped = true;

	sPoint2D GetSplinePoint(float t)
	{
		int p0, p1, p2, p3;
		if (!bIsLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
		}
		else
		{
			p1 = ((int)t) % points.size();
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
		}

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -ttt + 2.0f*tt - t;
		float q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
		float q3 = -3.0f*ttt + 4.0f*tt + t;
		float q4 = ttt - tt;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}

	sPoint2D GetSplineGradient(float t)
	{
		int p0, p1, p2, p3;
		if (!bIsLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
		}
		else
		{
			p1 = ((int)t) % points.size();
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
		}

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -3.0f * tt + 4.0f*t - 1.0f;
		float q2 = 9.0f*tt - 10.0f*t;
		float q3 = -9.0f*tt + 8.0f*t + 1.0f;
		float q4 = 3.0f*tt - 2.0f*t;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}

	float CalculateSegmentLength(int node)
	{
		float fLength = 0.0f;
		float fStepSize = 0.1;

		sPoint2D old_point, new_point;
		old_point = GetSplinePoint((float)node);

		for (float t = 0; t < 1.0f; t += fStepSize)
		{
			new_point = GetSplinePoint((float)node + t);
			fLength += sqrtf((new_point.x - old_point.x)*(new_point.x - old_point.x)
				+ (new_point.y - old_point.y)*(new_point.y - old_point.y));
			old_point = new_point;
		}

		return fLength;
	}


	float GetNormalisedOffset(float p)
	{
		// Which node is the base?
		int i = 0;
		while (p > points[i].length)
		{
			p -= points[i].length;
			i++;
		}

		// The fractional is the offset 
		return (float)i + (p / points[i].length);
	}


	void UpdateSplineProperties()
	{
		// Use to cache local spline lengths and overall spline length
		fTotalSplineLength = 0.0f;

		if (bIsLooped)
		{
			// Each node has a succeeding length
			for (int i = 0; i < points.size(); i++)
			{
				points[i].length = CalculateSegmentLength(i);
				fTotalSplineLength += points[i].length;
			}
		}
		else
		{
			for (int i = 1; i < points.size() - 2; i++)
			{
				points[i].length = CalculateSegmentLength(i);
				fTotalSplineLength += points[i].length;
			}
		}
	}

	void DrawSelf(olcConsoleGameEngine* gfx, float ox, float oy, wchar_t c = 0x2588, short col = 0x000F)
	{
		if (bIsLooped)
		{
			for (float t = 0; t < (float)points.size() - 0; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				gfx->Draw(pos.x, pos.y, c, col);
			}
		}
		else // Not Looped
		{
			for (float t = 0; t < (float)points.size() - 3; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				gfx->Draw(pos.x, pos.y, c, col);
			}
		}
	}

};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class OneLoneCoder_RacingLine : public olcConsoleGameEngine
{
public:
	OneLoneCoder_RacingLine()
	{
		m_sAppName = L"Racing Line";
	}

private:
	sSpline path, trackLeft, trackRight, racingLine;	// Various splines

	int nNodes = 20;	// Number of nodes in spline
	
	float fDisplacement[20]; // Displacement along spline node normal

	int nIterations = 1; 
	float fMarker = 1.0f;
	int nSelectedNode = -1;

	vector<pair<float, float>> vecModelCar;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		for (int i = 0; i < nNodes; i++)
		{
			//path.points.push_back(
			//	{ 30.0f * sinf((float)i / (float)nNodes * 3.14159f * 2.0f) + ScreenWidth() / 2,
			//	30.0f * cosf((float)i / (float)nNodes * 3.14159f * 2.0f) + ScreenHeight() / 2 });

			// Could use allocation functions for thes now, but just size via
			// append
			trackLeft.points.push_back({ 0.0f, 0.0f });
			trackRight.points.push_back({ 0.0f, 0.0f });
			racingLine.points.push_back({ 0.0f, 0.0f });
		}

		// A hand crafted track
		path.points = { { 81.8f, 196.0f },	{ 108.0f,210.0f },	{ 152.0f,216.0f },
		{ 182.0f,185.6f },	{ 190.0f,159.0f },	{ 198.0f,122.0f },	{ 226.0f,93.0f },
		{ 224.0f,41.0f },	{ 204.0f,15.0f },	{ 158.0f,24.0f },	{ 146.0f,52.0f },
		{ 157.0f,93.0f },	{ 124.0f,129.0f },	{ 83.0f,104.0f },	{ 77.0f,62.0f },
		{ 40.0f,57.0f },	{ 21.0f,83.0f },	{ 33.0f,145.0f },	{ 30.0f,198.0f },
		{ 48.0f,210.0f } };

		vecModelCar = { { 2,0 },{ 0,-1 },{ 0,1 } };

		path.UpdateSplineProperties();
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_DARK_GREEN);

		// Handle iteration count
		if (m_keys[L'A'].bHeld)	nIterations++;
		if (m_keys[L'S'].bHeld)	nIterations--;
		if (nIterations < 0) nIterations = 0;


		// Check if node is selected with mouse
		if (GetMouse(0).bPressed)
		{
			for (int i = 0; i < path.points.size(); i++)
			{
				float d = sqrtf(powf(path.points[i].x - GetMouseX(), 2) + powf(path.points[i].y - GetMouseY(), 2));
				if (d < 5.0f)
				{
					nSelectedNode = i;
					break;
				}
			}
		}

		if (GetMouse(0).bReleased)
			nSelectedNode = -1;

		// Move selected node
		if (GetMouse(0).bHeld && nSelectedNode >= 0)
		{
			path.points[nSelectedNode].x = GetMouseX();
			path.points[nSelectedNode].y = GetMouseY();
			path.UpdateSplineProperties();
		}

		// Move car around racing line
		fMarker += 2.0f * fElapsedTime;
		if (fMarker >= (float)racingLine.fTotalSplineLength)
			fMarker -= (float)racingLine.fTotalSplineLength;

		// Calculate track boundary points
		float fTrackWidth = 10.0f;
		for (int i = 0; i < path.points.size(); i++)
		{
			sPoint2D p1 = path.GetSplinePoint(i);
			sPoint2D g1 = path.GetSplineGradient(i);
			float glen = sqrtf(g1.x*g1.x + g1.y*g1.y);

			trackLeft.points[i].x = p1.x + fTrackWidth * (-g1.y / glen);
			trackLeft.points[i].y = p1.y + fTrackWidth * ( g1.x / glen);
			trackRight.points[i].x = p1.x - fTrackWidth * (-g1.y / glen);
			trackRight.points[i].y = p1.y - fTrackWidth * (g1.x / glen);
		}

		// Draw Track
		float fRes = 0.2f;
		for (float t = 0.0f; t < path.points.size(); t += fRes)
		{
			sPoint2D pl1 = trackLeft.GetSplinePoint(t);
			sPoint2D pr1 = trackRight.GetSplinePoint(t);
			sPoint2D pl2 = trackLeft.GetSplinePoint(t + fRes);
			sPoint2D pr2 = trackRight.GetSplinePoint(t + fRes);

			FillTriangle(pl1.x, pl1.y, pr1.x, pr1.y, pr2.x, pr2.y, PIXEL_SOLID, FG_GREY);
			FillTriangle(pl1.x, pl1.y, pl2.x, pl2.y, pr2.x, pr2.y, PIXEL_SOLID, FG_GREY);
		}

		// Reset racing line
		for (int i = 0; i < racingLine.points.size(); i++)
		{
			racingLine.points[i] = path.points[i];
			fDisplacement[i] = 0;
		}
		racingLine.UpdateSplineProperties();

		for (int n = 0; n < nIterations; n++)
		{
			for (int i = 0; i < racingLine.points.size(); i++)
			{
				// Get locations of neighbour nodes
				sPoint2D pointRight = racingLine.points[(i + 1) % racingLine.points.size()];
				sPoint2D pointLeft = racingLine.points[(i + racingLine.points.size() - 1) % racingLine.points.size()];
				sPoint2D pointMiddle = racingLine.points[i];

				// Create vectors to neighbours
				sPoint2D vectorLeft = { pointLeft.x - pointMiddle.x, pointLeft.y - pointMiddle.y };
				sPoint2D vectorRight = { pointRight.x - pointMiddle.x, pointRight.y - pointMiddle.y };

				// Normalise neighbours
				float lengthLeft = sqrtf(vectorLeft.x*vectorLeft.x + vectorLeft.y*vectorLeft.y);
				sPoint2D leftn = { vectorLeft.x / lengthLeft, vectorLeft.y / lengthLeft };
				float lengthRight = sqrtf(vectorRight.x*vectorRight.x + vectorRight.y*vectorRight.y);
				sPoint2D rightn = { vectorRight.x / lengthRight, vectorRight.y / lengthRight };

				// Add together to create bisector vector
				sPoint2D vectorSum = { rightn.x + leftn.x, rightn.y + leftn.y };
				float len = sqrtf(vectorSum.x*vectorSum.x + vectorSum.y*vectorSum.y);
				vectorSum.x /= len; vectorSum.y /= len;

				// Get point gradient and normalise
				sPoint2D g = path.GetSplineGradient(i);
				float glen = sqrtf(g.x*g.x + g.y*g.y);
				g.x /= glen; g.y /= glen;

				// Project required correction onto point tangent to give displacment
				float dp = -g.y*vectorSum.x + g.x * vectorSum.y;

				// Shortest path
				fDisplacement[i] += (dp * 0.3f);

				// Curvature
				//fDisplacement[(i + 1) % racingLine.points.size()] += dp * -0.2f;
				//fDisplacement[(i - 1 + racingLine.points.size()) % racingLine.points.size()] += dp * -0.2f;

			}

			// Clamp displaced points to track width
			for (int i = 0; i < racingLine.points.size(); i++)
			{
				if (fDisplacement[i] >= fTrackWidth) fDisplacement[i] = fTrackWidth;
				if (fDisplacement[i] <= -fTrackWidth) fDisplacement[i] = -fTrackWidth;

				sPoint2D g = path.GetSplineGradient(i);
				float glen = sqrtf(g.x*g.x + g.y*g.y);
				g.x /= glen; g.y /= glen;

				racingLine.points[i].x = path.points[i].x + -g.y * fDisplacement[i];
				racingLine.points[i].y = path.points[i].y + g.x * fDisplacement[i];
			}
		}

		path.DrawSelf(this, 0, 0);
		//trackLeft.DrawSelf(this, 0, 0);
		//trackRight.DrawSelf(this, 0, 0);

		racingLine.UpdateSplineProperties();
		racingLine.DrawSelf(this, 0, 0, PIXEL_SOLID, FG_BLUE);

		for (auto i : path.points)
			Fill(i.x - 1, i.y - 1, i.x + 2, i.y + 2, PIXEL_SOLID, FG_RED);

		sPoint2D car_p = racingLine.GetSplinePoint(fMarker);
		sPoint2D car_g = racingLine.GetSplineGradient(fMarker);
		DrawWireFrameModel(vecModelCar, car_p.x, car_p.y, atan2f(car_g.y, car_g.x), 4.0f, FG_BLACK);

		return true;
	}
};


int main()
{
	OneLoneCoder_RacingLine demo;
	demo.ConstructConsole(256, 240, 4, 4);
	demo.Start();
	return 0;
}