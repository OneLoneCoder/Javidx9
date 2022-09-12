/*
OneLoneCoder.com - Code-It-Yourself! Asteroids at the command prompt (quick and simple c++)
"This one just kept growing..." - @Javidx9

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
I needed a quick video to keep up with my upload schedule, and thought "oh asteroids
must be simple", and indeed it was, but it was a bit rubbish, and didn't really
offer anything educational, until I made the decision to ditch sprites and use
vector models like the original game. This made the game considerably more sophisticated,
has opened up new possibilities for the game engine, and just goes to show, jus 'cos
it's old, doesn't mean it's irrelevant.

I ended up with a demonstration of physics, wireframe graphics, 2D matrix transformations,
toroidal space mapping and vector manipulation.

Controls
~~~~~~~~
Left and Right arrow keys steer the ship. Up arrow is thrust. There is no reverse thrust.
Space bar fires bullets. Get the highest score by surviving waves of asteroids.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com
YouTube: www.youtube.com/javidx9

Video:
~~~~~~
https://youtu.be/QgDR8LrRZhk

Last Updated: 30/08/2017
*/

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class OneLoneCoder_Asteroids : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Asteroids()
	{
		m_sAppName = L"Asteroids";
	}

private:
	struct sSpaceObject
	{
		int nSize;
		float x;
		float y;
		float dx;
		float dy;
		float angle;
	};

	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;
	sSpaceObject player;
	bool bDead = false;
	int nScore = 0;

	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelAsteroid;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		vecModelShip = 
		{
			{ 0.0f, -5.0f},
			{-2.5f, +2.5f},
			{+2.5f, +2.5f}
		}; // A simple Isoceles Triangle

		// Create a "jagged" circle for the asteroid. It's important it remains
		// mostly circular, as we do a simple collision check against a perfect
		// circle.
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelAsteroid.push_back(make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f), 
												 noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		ResetGame();
		return true;
	}

	void ResetGame()
	{
		// Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		// Put in two asteroids
		vecAsteroids.push_back({ (int)16, 20.0f, 20.0f, 8.0f, -6.0f, 0.0f });
		vecAsteroids.push_back({ (int)16, 100.0f, 20.0f, -5.0f, 3.0f, 0.0f });

		// Reset game
		bDead = false;
		nScore = false;
	}

	// Implements "wrap around" for various in-game sytems
	void WrapCoordinates(float ix, float iy, float &ox, float &oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f)	ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())	ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	// Overriden to handle toroidal drawing routines
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);		
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}

	bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy)) < radius;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bDead)
			ResetGame();

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		// Steer Ship
		if (m_keys[VK_LEFT].bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (m_keys[VK_RIGHT].bHeld)
			player.angle += 5.0f * fElapsedTime;

		// Thrust? Apply ACCELERATION
		if (m_keys[VK_UP].bHeld)
		{
			// ACCELERATION changes VELOCITY (with respect to time)
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		// VELOCITY changes POSITION (with respect to time)
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		// Keep ship in gamespace
		WrapCoordinates(player.x, player.y, player.x, player.y);

		// Check ship collision with asteroids
		for (auto &a : vecAsteroids)
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
				bDead = true; // Uh oh...

		// Fire Bullet in direction of player
		if (m_keys[VK_SPACE].bReleased)
			vecBullets.push_back({ 0, player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 100.0f });

		// Update and draw asteroids
		for (auto &a : vecAsteroids)
		{
			// VELOCITY changes POSITION (with respect to time)
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation :)

			// Asteroid coordinates are kept in game space (toroidal mapping)
			WrapCoordinates(a.x, a.y, a.x, a.y);

			// Draw Asteroids
			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, FG_YELLOW);	
		}

		// Any new asteroids created after collision detection are stored
		// in a temporary vector, so we don't interfere with the asteroids
		// vector iterator in the for(auto)
		vector<sSpaceObject> newAsteroids;

		// Update Bullets
		for (auto &b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			b.angle -= 1.0f * fElapsedTime;

			// Check collision with asteroids
			for (auto &a : vecAsteroids)
			{
				//if (IsPointInsideRectangle(a.x, a.y, a.x + a.nSize, a.y + a.nSize, b.x, b.y))
				if(IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
				{
					// Asteroid Hit - Remove bullet
					// We've already updated the bullets, so force bullet to be offscreen
					// so it is cleaned up by the removal algorithm. 
					b.x = -100;

					// Create child asteroids
					if (a.nSize > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
					}

					// Remove asteroid - Same approach as bullets
					a.x = -100;
					nScore += 100; // Small score increase for hitting asteroid
				}
			}
		}

		// Append new asteroids to existing vector
		for(auto a:newAsteroids)
			vecAsteroids.push_back(a);

		// Clear up dead objects - They are out of game space

		// Remove asteroids that have been blown up
		if (vecAsteroids.size() > 0)
		{
			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}

		if (vecAsteroids.empty()) // If no asteroids, level complete! :) - you win MORE asteroids!
		{
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecAsteroids.clear();
			vecBullets.clear();

			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by th enext asteroid update
			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle - 3.14159f/2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.14159f/2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f*cosf(player.angle), 0.0f });

			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle + 3.14159f/2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f/2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f*cosf(-player.angle), 0.0f });
		}

		// Remove bullets that have gone off screen
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vecBullets.end())
				vecBullets.erase(i);
		}

		// Draw Bullets
		for (auto b : vecBullets)
			Draw(b.x, b.y);

		// Draw Ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		// Draw Score
		DrawString(2, 2, L"SCORE: " + to_wstring(nScore));
		return true;
	}

	void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
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
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second, 
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
		}
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}
