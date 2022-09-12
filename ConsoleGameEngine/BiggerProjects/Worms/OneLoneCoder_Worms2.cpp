/*
OneLoneCoder.com - Code-It-Yourself! Worms Part #2
"owch...." - @Javidx9

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
Worms is a classic game where several teams of worms use a variety of weaponry
to elimiate each other from a randomly generated terrain.

This code is the second part of a series that show how to make your own Worms game
from scratch in C++! 

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
Part #1 https://youtu.be/EHlaJvQpW3U
Part #2 https://youtu.be/pV2qYJjCdxM

Last Updated: 03/12/2017
*/
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class cPhysicsObject
{
public:
	float px = 0.0f; // Position
	float py = 0.0f;
	float vx = 0.0f; // Velocity
	float vy = 0.0f;
	float ax = 0.0f; // Acceleration
	float ay = 0.0f;

	float radius = 4.0f;	// Bounding circle for collision
	bool bStable = false;	// Has object stopped moving
	float fFriction = 0.8f;	// Actually, a dampening factor is a more accurate name

	int nBounceBeforeDeath = -1;	// How many time object can bounce before death
									// -1 = infinite
	bool bDead = false;		// Flag to indicate object should be removed

	cPhysicsObject(float x = 0.0f, float y = 0.0f)
	{
		px = x;
		py = y;
	}

	// Make class abstract
	virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY) = 0;
	virtual int BounceDeathAction() = 0;
};


class cDummy : public cPhysicsObject // Does nothing, shows a marker that helps with physics debug and test
{
public:
	cDummy(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y)
	{	}

	virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY)
	{
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_WHITE);
	}

	virtual int BounceDeathAction()
	{
		return 0; // Nothing, just fade
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDummy()
{
	// Defines a circle with a line fom center to edge
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	for (int i = 0; i < 10; i++)
		vecModel.push_back({ cosf(i / 9.0f * 2.0f * 3.14159f) , sinf(i / 9.0f * 2.0f * 3.14159f) });
	return vecModel;
}
vector<pair<float, float>> cDummy::vecModel = DefineDummy();


class cDebris : public cPhysicsObject // a small rock that bounces
{
public:
	cDebris(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y)
	{
		// Set velocity to random direction and size for "boom" effect
		vx = 10.0f * cosf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
		vy = 10.0f * sinf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
		radius = 1.0f;
		fFriction = 0.8f;
		nBounceBeforeDeath = 5; // After 5 bounces, dispose
	}

	virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY)
	{
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_DARK_GREEN);
	}

	virtual int BounceDeathAction()
	{
		return 0; // Nothing, just fade
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDebris()
{
	// A small unit rectangle
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ 1.0f, 0.0f });
	vecModel.push_back({ 1.0f, 1.0f });
	vecModel.push_back({ 0.0f, 1.0f });
	return vecModel;
}
vector<pair<float, float>> cDebris::vecModel = DefineDebris();


class cMissile : public cPhysicsObject // A projectile weapon
{
public:
	cMissile(float x = 0.0f, float y = 0.0f, float _vx = 0.0f, float _vy = 0.0f) : cPhysicsObject(x, y)
	{
		radius = 2.5f;
		fFriction = 0.5f;
		vx = _vx;
		vy = _vy;
		bDead = false;
		nBounceBeforeDeath = 1;
	}

	virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY)
	{
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_YELLOW);
	}

	virtual int BounceDeathAction()
	{
		return 20; // Explode Big
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineMissile()
{
	// Defines a rocket like shape
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ 1.0f, 1.0f });
	vecModel.push_back({ 2.0f, 1.0f });
	vecModel.push_back({ 2.5f, 0.0f });
	vecModel.push_back({ 2.0f, -1.0f });
	vecModel.push_back({ 1.0f, -1.0f });
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ -1.0f, -1.0f });
	vecModel.push_back({ -2.5f, -1.0f });
	vecModel.push_back({ -2.0f, 0.0f });
	vecModel.push_back({ -2.5f, 1.0f });
	vecModel.push_back({ -1.0f, 1.0f });
	
	// Scale points to make shape unit sized
	for (auto &v : vecModel)
	{
		v.first /= 2.5f; v.second /= 2.5f;
	}
	return vecModel;
}
vector<pair<float, float>> cMissile::vecModel = DefineMissile();


class cWorm : public cPhysicsObject // A unit, or worm
{
public:
	cWorm(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y)
	{
		radius = 3.5f;
		fFriction = 0.2f;
		bDead = false;
		nBounceBeforeDeath = -1;

		// load sprite data from sprite file
		if (sprWorm == nullptr)
			sprWorm = new olcSprite(L"../spriteeditor/worms.spr");
	}

	virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY)
	{
		engine->DrawPartialSprite(px - fOffsetX - radius, py - fOffsetY - radius, sprWorm, 0, 0, 8, 8);
	}

	virtual int BounceDeathAction()
	{
		return 0; // Nothing
	}

public:
	float fShootAngle = 0.0f;

private:
	static olcSprite *sprWorm;
};

olcSprite* cWorm::sprWorm = nullptr;




// Main Game Engine Class
class OneLoneCoder_Worms : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Worms()
	{
		m_sAppName = L"Worms";
	}

private:

	// Terrain size
	int nMapWidth = 1024;
	int nMapHeight = 512;
	unsigned char *map = nullptr;

	// Camera coordinates
	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;
	float fCameraPosXTarget = 0.0f;
	float fCameraPosYTarget = 0.0f;

	enum GAME_STATE
	{
		GS_RESET = 0,
		GS_GENERATE_TERRAIN = 1,
		GS_GENERATING_TERRAIN,
		GS_ALLOCATE_UNITS,
		GS_ALLOCATING_UNITS,
		GS_START_PLAY,
		GS_CAMERA_MODE
	} nGameState, nNextState;

	bool bGameIsStable = false;
	bool bPlayerHasControl = false;
	bool bPlayerActionComplete = false;


	// list of things that exist in game world
	list<unique_ptr<cPhysicsObject>> listObjects;

	cPhysicsObject* pObjectUnderControl = nullptr;
	cPhysicsObject* pCameraTrackingObject = nullptr;

	bool bEnergising = false;
	float fEnergyLevel = 0.0f;
	bool bFireWeapon = false;

	virtual bool OnUserCreate()
	{
		// Create Map
		map = new unsigned char[nMapWidth * nMapHeight];
		memset(map, 0, nMapWidth*nMapHeight * sizeof(unsigned char));
		//CreateMap();		

		nGameState = GS_RESET;
		nNextState = GS_RESET;


		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Press 'M' key to regenerate map
		//if (m_keys[L'M'].bReleased)
		//	CreateMap();

		// Left click to cause small explosion
		if (m_mouse[0].bReleased)
			Boom(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY, 10.0f);

		// Right click to drop missile
		if (m_mouse[1].bReleased)
			listObjects.push_back(unique_ptr<cMissile>(new cMissile(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY)));

		// Middle click to spawn worm/unit
		if (m_mouse[2].bReleased)
		{
			cWorm* worm = new cWorm(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
			pObjectUnderControl = worm;
			pCameraTrackingObject = worm;
			listObjects.push_back(unique_ptr<cWorm>(worm));
		}

		// Mouse Edge Map Scroll
		float fMapScrollSpeed = 400.0f;
		if (m_mousePosX < 5) fCameraPosX -= fMapScrollSpeed * fElapsedTime;
		if (m_mousePosX > ScreenWidth() - 5) fCameraPosX += fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY < 5) fCameraPosY -= fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY > ScreenHeight() - 5) fCameraPosY += fMapScrollSpeed * fElapsedTime;

		// Control Supervisor
		switch (nGameState)
		{
		case GS_RESET: // Set game variables to know state
		{
			bGameIsStable = false;
			bPlayerActionComplete = false;
			bPlayerHasControl = false;
			nNextState = GS_GENERATE_TERRAIN;
		}
		break;

		case GS_GENERATE_TERRAIN: // Create a new terrain
		{
			bPlayerHasControl = false;
			CreateMap();
			nNextState = GS_GENERATING_TERRAIN;
		}
		break;

		case GS_GENERATING_TERRAIN: // Does nothing, for now ;)
		{
			bPlayerHasControl = false;
			nNextState = GS_ALLOCATE_UNITS;
		}
		break;

		case GS_ALLOCATE_UNITS: // Add a unit to the top of the screen
		{
			bPlayerHasControl = false;
			cWorm *worm = new cWorm(32.0f, 1.0f);
			listObjects.push_back(unique_ptr<cWorm>(worm));
			pObjectUnderControl = worm;
			pCameraTrackingObject = pObjectUnderControl;
			nNextState = GS_ALLOCATING_UNITS;
		}
		break;

		case GS_ALLOCATING_UNITS: // Stay in this state whilst units are deploying
		{
			bPlayerHasControl = false;

			if (bGameIsStable) // Can only leave state once game is stable
			{
				bPlayerActionComplete = false;
				nNextState = GS_START_PLAY;
			}
		}
		break;

		case GS_START_PLAY: // Player is in control of unit
		{
			bPlayerHasControl = true;
			if (bPlayerActionComplete) // Can only leave state when the player action has completed
				nNextState = GS_CAMERA_MODE;
		}
		break;

		case GS_CAMERA_MODE: // Camera is tracking on-screen action
		{
			bPlayerHasControl = false;
			bPlayerActionComplete = false;

			if (bGameIsStable) // Can only leave state when action has finished, and engine is stable
			{
				pCameraTrackingObject = pObjectUnderControl;
				nNextState = GS_START_PLAY;
			}

		}
		break;
		}


		// Handle User Input
		if (bPlayerHasControl)
		{
			if (pObjectUnderControl != nullptr)
			{
				if (pObjectUnderControl->bStable)
				{
					if (m_keys[L'Z'].bPressed) // Jump in direction of cursor
					{
						float a = ((cWorm*)pObjectUnderControl)->fShootAngle;
						pObjectUnderControl->vx = 4.0f * cosf(a);
						pObjectUnderControl->vy = 8.0f * sinf(a);
						pObjectUnderControl->bStable = false;
					}

					if (m_keys[L'A'].bHeld) // Rotate cursor CCW
					{
						cWorm* worm = (cWorm*)pObjectUnderControl;
						worm->fShootAngle -= 1.0f * fElapsedTime;
						if (worm->fShootAngle < -3.14159f) worm->fShootAngle += 3.14159f * 2.0f;
					}

					if (m_keys[L'S'].bHeld) // Rotate cursor CW
					{
						cWorm* worm = (cWorm*)pObjectUnderControl;
						worm->fShootAngle += 1.0f * fElapsedTime;
						if (worm->fShootAngle > 3.14159f) worm->fShootAngle -= 3.14159f * 2.0f;
					}

					if (m_keys[VK_SPACE].bPressed) // Start to charge weapon
					{
						bEnergising = true;
						bFireWeapon = false;
						fEnergyLevel = 0.0f;
					}

					if (m_keys[VK_SPACE].bHeld) // Weapon is charging
					{
						if (bEnergising)
						{
							fEnergyLevel += 0.75f * fElapsedTime;
							if (fEnergyLevel >= 1.0f) // If it maxes out, Fire!
							{
								fEnergyLevel = 1.0f;
								bFireWeapon = true;
							}
						}
					}

					if (m_keys[VK_SPACE].bReleased) // If it is released before maxing out, Fire!
					{
						if (bEnergising)
						{
							bFireWeapon = true;
						}

						bEnergising = false;
					}
				}

				if (bFireWeapon)
				{
					cWorm* worm = (cWorm*)pObjectUnderControl;

					// Get Weapon Origin
					float ox = worm->px;
					float oy = worm->py;

					// Get Weapon Direction
					float dx = cosf(worm->fShootAngle);
					float dy = sinf(worm->fShootAngle);

					// Create Weapon Object
					cMissile *m = new cMissile(ox, oy, dx * 40.0f * fEnergyLevel, dy * 40.0f * fEnergyLevel);
					listObjects.push_back(unique_ptr<cMissile>(m));
					pCameraTrackingObject = m;

					// Reset flags involved with firing weapon
					bFireWeapon = false;
					fEnergyLevel = 0.0f;
					bEnergising = false;

					// Indicate the player has completed their action for this unit
					bPlayerActionComplete = true;
				}
			}
		}

		if (pCameraTrackingObject != nullptr)
		{
			//fCameraPosX = pCameraTrackingObject->px - ScreenWidth() / 2;
			//fCameraPosY = pCameraTrackingObject->py - ScreenHeight() / 2;
			fCameraPosXTarget = pCameraTrackingObject->px - ScreenWidth() / 2;
			fCameraPosYTarget = pCameraTrackingObject->py - ScreenHeight() / 2;
			fCameraPosX += (fCameraPosXTarget - fCameraPosX) * 5.0f * fElapsedTime;
			fCameraPosY += (fCameraPosYTarget - fCameraPosY) * 5.0f * fElapsedTime;
		}



		// Clamp map boundaries
		if (fCameraPosX < 0) fCameraPosX = 0;
		if (fCameraPosX >= nMapWidth - ScreenWidth()) fCameraPosX = nMapWidth - ScreenWidth();
		if (fCameraPosY < 0) fCameraPosY = 0;
		if (fCameraPosY >= nMapHeight - ScreenHeight()) fCameraPosY = nMapHeight - ScreenHeight();

		// Do 10 physics iterations per frame - this allows smaller physics steps
		// giving rise to more accurate and controllable calculations
		for (int z = 0; z < 10; z++)
		{
			// Update physics of all physical objects
			for (auto &p : listObjects)
			{
				// Apply Gravity
				p->ay += 2.0f;

				// Update Velocity
				p->vx += p->ax * fElapsedTime;
				p->vy += p->ay * fElapsedTime;

				// Update Position
				float fPotentialX = p->px + p->vx * fElapsedTime;
				float fPotentialY = p->py + p->vy * fElapsedTime;

				// Reset Acceleration
				p->ax = 0.0f;
				p->ay = 0.0f;
				p->bStable = false;

				// Collision Check With Map
				float fAngle = atan2f(p->vy, p->vx);
				float fResponseX = 0;
				float fResponseY = 0;
				bool bCollision = false;

				// Iterate through semicircle of objects radius rotated to direction of travel
				for (float r = fAngle - 3.14159f / 2.0f; r < fAngle + 3.14159f / 2.0f; r += 3.14159f / 8.0f)
				{
					// Calculate test point on circumference of circle
					float fTestPosX = (p->radius) * cosf(r) + fPotentialX;
					float fTestPosY = (p->radius) * sinf(r) + fPotentialY;

					// Constrain to test within map boundary
					if (fTestPosX >= nMapWidth) fTestPosX = nMapWidth - 1;
					if (fTestPosY >= nMapHeight) fTestPosY = nMapHeight - 1;
					if (fTestPosX < 0) fTestPosX = 0;
					if (fTestPosY < 0) fTestPosY = 0;

					// Test if any points on semicircle intersect with terrain
					if (map[(int)fTestPosY * nMapWidth + (int)fTestPosX] != 0)
					{
						// Accumulate collision points to give an escape response vector
						// Effectively, normal to the areas of contact
						fResponseX += fPotentialX - fTestPosX;
						fResponseY += fPotentialY - fTestPosY;
						bCollision = true;
					}
				}

				// Calculate magnitudes of response and velocity vectors
				float fMagVelocity = sqrtf(p->vx*p->vx + p->vy*p->vy);
				float fMagResponse = sqrtf(fResponseX*fResponseX + fResponseY*fResponseY);

				// Collision occurred
				if (bCollision)
				{
					// Force object to be stable, this stops the object penetrating the terrain
					p->bStable = true;
					
					// Calculate reflection vector of objects velocity vector, using response vector as normal
					float dot = p->vx * (fResponseX / fMagResponse) + p->vy * (fResponseY / fMagResponse);
					
					// Use friction coefficient to dampen response (approximating energy loss)
					p->vx = p->fFriction * (-2.0f * dot * (fResponseX / fMagResponse) + p->vx);
					p->vy = p->fFriction * (-2.0f * dot * (fResponseY / fMagResponse) + p->vy);

					//Some objects will "die" after several bounces
					if (p->nBounceBeforeDeath > 0)
					{
						p->nBounceBeforeDeath--;
						p->bDead = p->nBounceBeforeDeath == 0;

						// If object died, work out what to do next
						if (p->bDead)
						{
							// Action upon object death
							// = 0 Nothing
							// > 0 Explosion 
							int nResponse = p->BounceDeathAction();
							if (nResponse > 0)
							{
								Boom(p->px, p->py, nResponse);

								// Dead objects can no lobger be tracked by the camera
								pCameraTrackingObject = nullptr;
							}
						}
					}

				}
				else
				{
					// No collision so update objects position
					p->px = fPotentialX;
					p->py = fPotentialY;
				}

				// Turn off movement when tiny
				if (fMagVelocity < 0.1f) p->bStable = true;
			}

			// Remove dead objects from the list, so they are not processed further. As the object
			// is a unique pointer, it will go out of scope too, deleting the object automatically. Nice :-)
			listObjects.remove_if([](unique_ptr<cPhysicsObject> &o) {return o->bDead; });
		}

		// Draw Landscape
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				// Offset screen coordinates into world coordinates
				switch (map[(y + (int)fCameraPosY)*nMapWidth + (x + (int)fCameraPosX)])
				{
				case 0:
					Draw(x, y, PIXEL_SOLID, FG_CYAN); // Sky
					break;
				case 1:
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN); // Land
					break;
				}
			}

		// Draw Objects
		for (auto &p : listObjects)
		{
			p->Draw(this, fCameraPosX, fCameraPosY);
			cWorm* worm = (cWorm*)pObjectUnderControl;

			if (p.get() == worm)
			{
				float cx = worm->px + 8.0f * cosf(worm->fShootAngle) - fCameraPosX;
				float cy = worm->py + 8.0f * sinf(worm->fShootAngle) - fCameraPosY;

				// Draw "+" symbol
				Draw(cx, cy, PIXEL_SOLID, FG_BLACK);
				Draw(cx + 1, cy, PIXEL_SOLID, FG_BLACK);
				Draw(cx - 1, cy, PIXEL_SOLID, FG_BLACK);
				Draw(cx, cy + 1, PIXEL_SOLID, FG_BLACK);
				Draw(cx, cy - 1, PIXEL_SOLID, FG_BLACK);

				// Draws an Energy Bar, indicating how much energy should the weapon be
				// fired with
				for (int i = 0; i < 11 * fEnergyLevel; i++)
				{
					Draw(worm->px - 5 + i - fCameraPosX, worm->py - 12 - fCameraPosY, PIXEL_SOLID, FG_GREEN);
					Draw(worm->px - 5 + i - fCameraPosX, worm->py - 11 - fCameraPosY, PIXEL_SOLID, FG_RED);
				}
			}
		}

		// Check For game state stability
		bGameIsStable = true;
		for (auto &p : listObjects)
			if (!p->bStable)
			{
				bGameIsStable = false;
				break;
			}

		// DEBUG Feature: Indicate Game Stability
		if (bGameIsStable)
			Fill(2, 2, 6, 6, PIXEL_SOLID, FG_RED);


		// Update State Machine
		nGameState = nNextState;
		return true;
	}

	// Explosion Function
	void Boom(float fWorldX, float fWorldY, float fRadius)
	{
		auto CircleBresenham = [&](int xc, int yc, int r)
		{
			// Taken from wikipedia
			int x = 0;
			int y = r;
			int p = 3 - 2 * r;
			if (!r) return;

			auto drawline = [&](int sx, int ex, int ny)
			{
				for (int i = sx; i < ex; i++)
					if (ny >= 0 && ny < nMapHeight && i >= 0 && i < nMapWidth)
						map[ny*nMapWidth + i] = 0;
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

		// Erase Terrain to form crater
		CircleBresenham(fWorldX, fWorldY, fRadius);

		// Shockwave other entities in range
		for (auto &p : listObjects)
		{
			// Work out distance between explosion origin and object
			float dx = p->px - fWorldX;
			float dy = p->py - fWorldY;
			float fDist = sqrt(dx*dx + dy*dy);
			if (fDist < 0.0001f) fDist = 0.0001f;

			// If within blast radius
			if (fDist < fRadius)
			{
				// Set velocity proportional and away from boom origin
				p->vx = (dx / fDist) * fRadius;
				p->vy = (dy / fDist) * fRadius;
				p->bStable = false;
			}
		}

		// Launch debris proportional to blast size
		for (int i = 0; i < (int)fRadius; i++)
			listObjects.push_back(unique_ptr<cDebris>(new cDebris(fWorldX, fWorldY)));
	}



	void CreateMap()
	{
		// Used 1D Perlin Noise
		float *fSurface = new float[nMapWidth];
		float *fNoiseSeed = new float[nMapWidth];

		// Populate with noise
		for (int i = 0; i < nMapWidth; i++)
			fNoiseSeed[i] = (float)rand() / (float)RAND_MAX;

		// Clamp noise to half way up screen
		fNoiseSeed[0] = 0.5f;

		// Generate 1D map
		PerlinNoise1D(nMapWidth, fNoiseSeed, 8, 2.0f, fSurface);

		// Fill 2D map based on adjacent 1D map
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y >= fSurface[x] * nMapHeight)
					map[y * nMapWidth + x] = 1;
				else
					map[y * nMapWidth + x] = 0;
			}

		// Clean up!
		delete[] fSurface;
		delete[] fNoiseSeed;
	}

	// Taken from Perlin Noise Video https://youtu.be/6-0UaeJBumA
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
};


int main()
{
	OneLoneCoder_Worms game;
	game.ConstructConsole(256, 160, 6, 6);
	game.Start();
	return 0;
}