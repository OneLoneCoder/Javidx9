#include <iostream>
#include <string>
using namespace std;

#include "olcConsoleGameEngineOOP.h"

#include "RPG_Assets.h"
#include "RPG_Maps.h"
#include "RPG_Dynamics.h"
#include "RPG_Commands.h"

#define X(n) m_script.AddCommand(new cCommand_ ## n)

class OneLoneCoder_RPG : public olcConsoleGameEngineOOP
{
public:
	OneLoneCoder_RPG()
	{
		m_sAppName = L"Top Down Role Playing Game";
	}

private:
	cMap *m_pCurrentMap = nullptr;

	cDynamic *m_pPlayer = nullptr;

	vector<cDynamic*> m_vecDynamics;

	cScriptProcessor m_script;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	olcSprite *spriteTiles = nullptr;
	olcSprite *spriteMan = nullptr;
	olcSprite *m_sprFont = nullptr;


	void DrawBigText(string sText, int x, int y)
	{
		int i = 0;
		for (auto c : sText)
		{
			int sx = ((c - 32) % 16) * 8;
			int sy = ((c - 32) / 16) * 8;
			DrawPartialSprite(x + i * 8, y, m_sprFont, sx, sy, 8, 8);
			i++;
		}

	}

protected:
	virtual bool OnUserCreate()
	{
		cCommand::g_engine = this;
		RPG_Assets::get().LoadSprites();

		m_sprFont = RPG_Assets::get().GetSprite("font");

		m_pCurrentMap = new cMap_Village1();

		m_pPlayer = new cDynamic_Creature("player", RPG_Assets::get().GetSprite("player"));
		m_pPlayer->px = 5.0f;
		m_pPlayer->py = 5.0f;

		cDynamic* ob1 = new cDynamic_Creature("skelly1", RPG_Assets::get().GetSprite("skelly"));
		ob1->px = 12.0f;
		ob1->py = 12.0f;

		cDynamic* ob2 = new cDynamic_Creature("skelly2", RPG_Assets::get().GetSprite("skelly"));
		ob2->px = 5.0f;
		ob2->py = 8.0f;

		m_vecDynamics.push_back(m_pPlayer);
		m_vecDynamics.push_back(ob1);
		m_vecDynamics.push_back(ob2);
	

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{

		// Update script
		m_script.ProcessCommands(fElapsedTime);

		if (m_script.bUserControlEnabled)
		{

			m_pPlayer->vx = 0.0f;
			m_pPlayer->vy = 0.0f;

			// Handle Input
			if (IsFocused())
			{
				if (GetKey(VK_UP).bHeld)
				{
					m_pPlayer->vy = -4.0f;
				}

				if (GetKey(VK_DOWN).bHeld)
				{
					m_pPlayer->vy = 4.0f;
				}

				if (GetKey(VK_LEFT).bHeld)
				{
					m_pPlayer->vx = -4.0f;
				}

				if (GetKey(VK_RIGHT).bHeld)
				{
					m_pPlayer->vx = 4.0f;
				}

				if (GetKey(L'Z').bReleased)
				{
					X(MoveTo(m_pPlayer, 10, 10, 3.0f));
					X(MoveTo(m_pPlayer, 15, 10, 3.0f));
					X(MoveTo(m_vecDynamics[1], 15, 12, 2.0f));
					X(ShowDialog({ "Grrrrr!" }));
					X(ShowDialog({ "I think OOP", "is really useful!" }));
					X(MoveTo(m_pPlayer, 15, 15, 3.0f));
					X(MoveTo(m_pPlayer, 10, 10, 3.0f));
				}
			}
		}
		else
		{
			// Scripting system is in control
			if (m_bShowDialog)
			{
				if (GetKey(VK_SPACE).bReleased)
				{
					m_bShowDialog = false;
					m_script.CompleteCommand();
				}
			}
		}

		for (auto &object : m_vecDynamics)
		{
			float fNewObjectPosX = object->px + object->vx * fElapsedTime;
			float fNewObjectPosY = object->py + object->vy * fElapsedTime;

			// Collision
			if (object->vx <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, object->py + 0.9f))
				{
					fNewObjectPosX = (int)fNewObjectPosX + 1;
					object->vx = 0;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + 1.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 1.0f, object->py + 0.9f))
				{
					fNewObjectPosX = (int)fNewObjectPosX;
					object->vx = 0;

				}
			}

			if (object->vy <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.9f, fNewObjectPosY))
				{
					fNewObjectPosY = (int)fNewObjectPosY + 1;
					object->vy = 0;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY + 1.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.9f, fNewObjectPosY + 1.0f))
				{
					fNewObjectPosY = (int)fNewObjectPosY;
					object->vy = 0;
				}
			}

			object->px = fNewObjectPosX;
			object->py = fNewObjectPosY;

			object->Update(fElapsedTime);
		}


		fCameraPosX = m_pPlayer->px;
		fCameraPosY = m_pPlayer->py;

		// Draw Level
		int nTileWidth = 16;
		int nTileHeight = 16;
		int nVisibleTilesX = ScreenWidth() / nTileWidth;
		int nVisibleTilesY = ScreenHeight() / nTileHeight;

		// Calculate Top-Leftmost visible tile
		float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
		float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;

		// Clamp camera to game boundaries
		if (fOffsetX < 0) fOffsetX = 0;
		if (fOffsetY < 0) fOffsetY = 0;
		if (fOffsetX > m_pCurrentMap->nWidth - nVisibleTilesX) fOffsetX = m_pCurrentMap->nWidth - nVisibleTilesX;
		if (fOffsetY > m_pCurrentMap->nHeight - nVisibleTilesY) fOffsetY = m_pCurrentMap->nHeight - nVisibleTilesY;

		// Get offsets for smooth movement
		float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
		float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;

		// Draw visible tile map
		for (int x = -1; x < nVisibleTilesX + 1; x++)
		{
			for (int y = -1; y < nVisibleTilesY + 1; y++)
			{
				int idx = m_pCurrentMap->GetIndex(x + fOffsetX, y + fOffsetY);
				int sx = idx % 10;
				int sy = idx / 10;

				DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, m_pCurrentMap->pSprite, sx * nTileWidth, sy * nTileHeight, nTileWidth, nTileHeight);
			}
		}

		// Draw Object
		for (auto &object : m_vecDynamics)
			object->DrawSelf(this, fOffsetX, fOffsetY);
		
		m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY);

		// Draw any dialog being displayed
		if (m_bShowDialog)
			DisplayDialog(m_vecDialogToShow, 20, 20);


		return true;
	}

	vector<string> m_vecDialogToShow;
	bool m_bShowDialog = false;
	float m_fDialogX = 0.0f;
	float m_fDialogY = 0.0f;
public:

	void ShowDialog(vector<string> vecLines)
	{
		m_vecDialogToShow = vecLines;
		m_bShowDialog = true;
	}

	void DisplayDialog(vector<string> vecText, int x, int y)
	{
		int nMaxLineLength = 0;
		int nLines = vecText.size();

		for (auto l : vecText)	if (l.size() > nMaxLineLength) nMaxLineLength = l.size();

		// Draw Box
		Fill(x - 1, y - 1, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1, PIXEL_SOLID, FG_DARK_BLUE);
		DrawLine(x - 2, y - 2, x - 2, y + nLines * 8 + 1);
		DrawLine(x + nMaxLineLength * 8 + 1, y - 2, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1);
		DrawLine(x - 2, y - 2, x + nMaxLineLength * 8 + 1, y - 2);
		DrawLine(x - 2, y + nLines * 8 + 1, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1);

		for (int l = 0; l<vecText.size(); l++)
			DrawBigText(vecText[l], x, y + l * 8);
	}
};
