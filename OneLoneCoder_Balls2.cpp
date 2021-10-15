/*
OneLoneCoder.com - Programming Balls! #2 Circle Vs Edge Collisions
"...totally overkill for pong..." - @Javidx9

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
Collision detection engines can get quite complicated. This program shows the interactions
between circular objects of different sizes and masses. Use Left mouse button to select
and drag a ball to examin static collisions, and use Right mouse button to apply velocity
to the balls as if using a pool/snooker/billiards cue.

Author
~~~~~~
Twitter:	@javidx9
Blog:		www.onelonecoder.com
Twitch:		https://www.twitch.tv/javidx9
Discord:	https://discord.gg/WhwHUMV

Video:
~~~~~~
Part #1 https://youtu.be/LPzyNOHY3A4
Part #2 https://youtu.be/ebq7L2Wtbl4

Last Updated: 18/02/2017
*/

#include <iostream>
#include <string>
using namespace std;

#include "olcConsoleGameEngine.h"

struct sBall
{
	float px, py;
	float vx, vy;
	float ax, ay;
	float ox, oy;
	float radius;
	float mass;
	float friction;
	int score;
	int id;
	float fSimTimeRemaining;
};

struct sLineSegment
{
	float sx, sy;
	float ex, ey;
	float radius;
};



class CirclePhysics : public olcConsoleGameEngine
{
public:
	CirclePhysics()
	{
		m_sAppName = L"Circles V Edges";
	}

private:
	vector<sBall> vecBalls;
	vector<sLineSegment> vecLines;
	vector<pair<float, float>> modelCircle;
	sBall* pSelectedBall = nullptr;
	olcSprite *spriteBalls = nullptr;
	sLineSegment* pSelectedLine = nullptr;
	bool bSelectedLineStart = false;

	void AddBall(float x, float y, float r = 5.0f, int s = 0)
	{
		sBall b;
		b.px = x; b.py = y;
		b.vx = 0; b.vy = 0;
		b.ax = 0; b.ay = 0;
		b.ox = 0; b.oy = 0;
		b.radius = r;
		b.mass = r * 10.0f;
		b.friction = 0.0f;
		b.score = s;
		b.fSimTimeRemaining = 0.0f;
		b.id = vecBalls.size();
		vecBalls.emplace_back(b);
	}

public:
	bool OnUserCreate()
	{
		float fBallRadius = 4.0f;
		for (int i = 0; i <100; i++)
			AddBall(((float)rand()/(float)RAND_MAX) * ScreenWidth(), ((float)rand() / (float)RAND_MAX) * ScreenHeight(), fBallRadius);

		AddBall(28.0f, 33.0, fBallRadius * 3);
		AddBall(28.0f, 35.0, fBallRadius * 2); 
		
		float fLineRadius = 4.0f;
		vecLines.push_back({ 12.0f, 4.0f, 64.0f, 4.0f, fLineRadius });
		vecLines.push_back({ 76.0f, 4.0f, 132.0f, 4.0f, fLineRadius });
		vecLines.push_back({ 12.0f, 68.0f, 64.0f, 68.0f, fLineRadius });
		vecLines.push_back({ 76.0f, 68.0f, 132.0f, 68.0f, fLineRadius });
		vecLines.push_back({ 4.0f, 12.0f, 4.0f, 60.0f, fLineRadius });
		vecLines.push_back({ 140.0f, 12.0f, 140.0f, 60.0f, fLineRadius });
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		auto DoCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2)
		{
			return fabs((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) <= ((r1 + r2) * (r1 + r2));
		};

		auto IsPointInCircle = [](float x1, float y1, float r1, float px, float py)
		{
			return fabs((x1 - px)*(x1 - px) + (y1 - py)*(y1 - py)) < (r1 * r1);
		};

		if (m_mouse[0].bPressed)
		{
			// Check for selected ball
			pSelectedBall = nullptr;
			for (auto &ball : vecBalls)
			{
				if (IsPointInCircle(ball.px, ball.py, ball.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedBall = &ball;
					break;
				}
			}

			// Check for selected line segment end
			pSelectedLine = nullptr;
			for (auto &line : vecLines)
			{
				if (IsPointInCircle(line.sx, line.sy, line.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedLine = &line;
					bSelectedLineStart = true;
					break;
				}

				if (IsPointInCircle(line.ex, line.ey, line.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedLine = &line;
					bSelectedLineStart = false;
					break;
				}
			}
		}

		if (m_mouse[0].bHeld)
		{
			if (pSelectedLine != nullptr)
			{			
				if (bSelectedLineStart)
				{					
					pSelectedLine->sx = GetMouseX();
					pSelectedLine->sy = GetMouseY();
				}
				else
				{
					pSelectedLine->ex = GetMouseX();
					pSelectedLine->ey = GetMouseY();
				}
			}
		}

		if (m_mouse[0].bReleased)
		{
			if (pSelectedBall != nullptr)
			{
				// Apply velocity
				pSelectedBall->vx = 5.0f * ((pSelectedBall->px) - m_mousePosX);
				pSelectedBall->vy = 5.0f * ((pSelectedBall->py) - m_mousePosY);
			}

			pSelectedBall = nullptr;
			pSelectedLine = nullptr;
		}

		if (m_mouse[1].bHeld)
		{
			for (auto &ball : vecBalls)
			{
				ball.vx += (m_mousePosX - ball.px) * 0.01f;
				ball.vy += (m_mousePosY - ball.py) * 0.01f;
			}
		}

		

		vector<pair<sBall*, sBall*>> vecCollidingPairs;
		vector<sBall*> vecFakeBalls;

		// Threshold indicating stability of object
		float fStable = 0.05f;
		
		// Multiple simulation updates with small time steps permit more accurate physics
		// and realistic results at the expense of CPU time of course
		int nSimulationUpdates = 4;

		// Multiple collision trees require more steps to resolve. Normally we would
		// continue simulation until the object has no simulation time left for this
		// epoch, however this is risky as the system may never find stability, so we
		// can clamp it here
		int nMaxSimulationSteps = 15;

		// Break up the frame elapsed time into smaller deltas for each simulation update
		float fSimElapsedTime = fElapsedTime / (float)nSimulationUpdates;

		// Main simulation loop
		for (int i = 0; i < nSimulationUpdates; i++)
		{
			// Set all balls time to maximum for this epoch
			for (auto &ball : vecBalls)
				ball.fSimTimeRemaining = fSimElapsedTime;

			// Erode simulation time on a per objec tbasis, depending upon what happens
			// to it during its journey through this epoch
			for (int j = 0; j < nMaxSimulationSteps; j++)
			{
				// Update Ball Positions
				for (auto &ball : vecBalls)
				{
					if (ball.fSimTimeRemaining > 0.0f)
					{
						ball.ox = ball.px;								// Store original position this epoch
						ball.oy = ball.py;

						ball.ax = -ball.vx * 0.8f;						// Apply drag and gravity
						ball.ay = -ball.vy * 0.8f + 100.0f;

						ball.vx += ball.ax * ball.fSimTimeRemaining;	// Update Velocity
						ball.vy += ball.ay * ball.fSimTimeRemaining;

						ball.px += ball.vx * ball.fSimTimeRemaining;	// Update position
						ball.py += ball.vy * ball.fSimTimeRemaining;

						// Crudely wrap balls to screen - note this cause issues when collisions occur on screen boundaries
						if (ball.px < 0) ball.px += (float)ScreenWidth();
						if (ball.px >= ScreenWidth()) ball.px -= (float)ScreenWidth();
						if (ball.py < 0) ball.py += (float)ScreenHeight();
						if (ball.py >= ScreenHeight()) ball.py -= (float)ScreenHeight();

						// Stop ball when velocity is neglible
						if (fabs(ball.vx*ball.vx + ball.vy*ball.vy) < fStable)
						{
							ball.vx = 0;
							ball.vy = 0;
						}
					}
				}


				// Work out static collisions with walls and displace balls so no overlaps
				for (auto &ball : vecBalls)
				{
					float fDeltaTime = ball.fSimTimeRemaining;

					// Against Edges
					for (auto &edge : vecLines)
					{
						// Check that line formed by velocity vector, intersects with line segment
						float fLineX1 = edge.ex - edge.sx;
						float fLineY1 = edge.ey - edge.sy;

						float fLineX2 = ball.px - edge.sx;
						float fLineY2 = ball.py - edge.sy;

						float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

						// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
						// how much of the segment is in the "shadow" of the object vector. The min and max clamp
						// this to lie between 0 and the line segment length, which is then normalised. We can
						// use this to calculate the closest point on the line segment
						float t = max(0, min(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

						// Which we do here
						float fClosestPointX = edge.sx + t * fLineX1;
						float fClosestPointY = edge.sy + t * fLineY1;

						// And once we know the closest point, we can check if the ball has collided with the segment in the
						// same way we check if two balls have collided
						float fDistance = sqrtf((ball.px - fClosestPointX)*(ball.px - fClosestPointX) + (ball.py - fClosestPointY)*(ball.py - fClosestPointY));

						if (fDistance <= (ball.radius + edge.radius))
						{
							// Collision has occurred - treat collision point as a ball that cannot move. To make this
							// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
							// so it behaves like a solid object when the momentum calculations are performed
							sBall *fakeball = new sBall();
							fakeball->radius = edge.radius;
							fakeball->mass = ball.mass * 0.8f;
							fakeball->px = fClosestPointX;
							fakeball->py = fClosestPointY;
							fakeball->vx = -ball.vx;	// We will use these later to allow the lines to impart energy into ball
							fakeball->vy = -ball.vy;	// if the lines are moving, i.e. like pinball flippers
							
							// Store Fake Ball
							vecFakeBalls.push_back(fakeball);
							
							// Add collision to vector of collisions for dynamic resolution
							vecCollidingPairs.push_back({ &ball, fakeball });

							// Calculate displacement required
							float fOverlap = 1.0f * (fDistance - ball.radius - fakeball->radius);

							// Displace Current Ball away from collision
							ball.px -= fOverlap * (ball.px - fakeball->px) / fDistance;
							ball.py -= fOverlap * (ball.py - fakeball->py) / fDistance;
						}
					}

					// Against other balls
					for (auto &target : vecBalls)
					{
						if (ball.id != target.id) // Do not check against self
						{
							if (DoCirclesOverlap(ball.px, ball.py, ball.radius, target.px, target.py, target.radius))
							{
								// Collision has occured
								vecCollidingPairs.push_back({ &ball, &target });

								// Distance between ball centers
								float fDistance = sqrtf((ball.px - target.px)*(ball.px - target.px) + (ball.py - target.py)*(ball.py - target.py));

								// Calculate displacement required
								float fOverlap = 0.5f * (fDistance - ball.radius - target.radius);

								// Displace Current Ball away from collision
								ball.px -= fOverlap * (ball.px - target.px) / fDistance;
								ball.py -= fOverlap * (ball.py - target.py) / fDistance;

								// Displace Target Ball away from collision - Note, this should affect the timing of the target ball
								// and it does, but this is absorbed by the target ball calculating its own time delta later on
								target.px += fOverlap * (ball.px - target.px) / fDistance;
								target.py += fOverlap * (ball.py - target.py) / fDistance;
							}
						}
					}

					// Time displacement - we knew the velocity of the ball, so we can estimate the distance it should have covered
					// however due to collisions it could not do the full distance, so we look at the actual distance to the collision
					// point and calculate how much time that journey would have taken using the speed of the object. Therefore
					// we can now work out how much time remains in that timestep.
					float fIntendedSpeed	= sqrtf(ball.vx * ball.vx + ball.vy * ball.vy);
					float fIntendedDistance = fIntendedSpeed * ball.fSimTimeRemaining;
					float fActualDistance	= sqrtf((ball.px - ball.ox)*(ball.px - ball.ox) + (ball.py - ball.oy)*(ball.py - ball.oy));
					float fActualTime = fActualDistance / fIntendedSpeed;

					// After static resolution, there may be some time still left for this epoch, so allow simulation to continue
					ball.fSimTimeRemaining = ball.fSimTimeRemaining - fActualTime;
				}

				// Now work out dynamic collisions
				float fEfficiency = 1.00f;
				for (auto c : vecCollidingPairs)
				{
					sBall *b1 = c.first, *b2 = c.second;

					// Distance between balls
					float fDistance = sqrtf((b1->px - b2->px)*(b1->px - b2->px) + (b1->py - b2->py)*(b1->py - b2->py));

					// Normal
					float nx = (b2->px - b1->px) / fDistance;
					float ny = (b2->py - b1->py) / fDistance;

					// Tangent
					float tx = -ny;
					float ty = nx;

					// Dot Product Tangent
					float dpTan1 = b1->vx * tx + b1->vy * ty;
					float dpTan2 = b2->vx * tx + b2->vy * ty;

					// Dot Product Normal
					float dpNorm1 = b1->vx * nx + b1->vy * ny;
					float dpNorm2 = b2->vx * nx + b2->vy * ny;

					// Conservation of momentum in 1D
					float m1 = fEfficiency * (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
					float m2 = fEfficiency * (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

					// Update ball velocities
					b1->vx = tx * dpTan1 + nx * m1;
					b1->vy = ty * dpTan1 + ny * m1;
					b2->vx = tx * dpTan2 + nx * m2;
					b2->vy = ty * dpTan2 + ny * m2;
				}

				// Remove collisions
				vecCollidingPairs.clear();

				// Remove fake balls
				for (auto &b : vecFakeBalls) delete b;
				vecFakeBalls.clear();
			}
		}

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');

		// Draw Lines
		for (auto line : vecLines)
		{
			FillCircle(line.sx, line.sy, line.radius, PIXEL_HALF, FG_WHITE);
			FillCircle(line.ex, line.ey, line.radius, PIXEL_HALF, FG_WHITE);

			float nx = -(line.ey - line.sy);
			float ny = (line.ex - line.sx);
			float d = sqrt(nx*nx + ny * ny);
			nx /= d;
			ny /= d;

			DrawLine((line.sx + nx * line.radius), (line.sy + ny * line.radius), (line.ex + nx * line.radius), (line.ey + ny * line.radius));
			DrawLine((line.sx - nx * line.radius), (line.sy - ny * line.radius), (line.ex - nx * line.radius), (line.ey - ny * line.radius));
		}

		// Draw Balls
		for (auto ball : vecBalls)
			FillCircle(ball.px, ball.py, ball.radius, PIXEL_SOLID, FG_RED);

		// Draw Cue
		if (pSelectedBall != nullptr)
			DrawLine(pSelectedBall->px, pSelectedBall->py, m_mousePosX, m_mousePosY, PIXEL_SOLID, FG_BLUE);

		return true;
	}

};


int main()
{
	CirclePhysics game;
	if (game.ConstructConsole(320, 240, 4, 4))
		game.Start();
	else
		wcout << L"Could not construct console" << endl;

	return 0;
};

