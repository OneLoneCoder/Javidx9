/*
	+-------------------------------------------------------------+
	|              OneLoneCoder RPG Game Engine                   |
	|     "The Legend of WittyBit, Fantasy Quest VI" - javidx9    |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~

	This is the code I created as part of my
	"Code-It-Yourself! Role Playing Game" series on YouTube. This is
	NOT a game. The project will compile and demonstrate several
	systems developed as part of that series. My original intention
	was to develop a small yet complete RPG, alas real life got in
	the way. After several months, I've decided to just open the source
	"as is", so it will contain bugs, be confusing and all round not
	up to the usual "quality" I strive for.

	Part 1: https://youtu.be/xXXt3htgDok
	Part 2: https://youtu.be/AWY_ITpldRk
	Part 3: https://youtu.be/UcNSb-m4YQU
	Part 4: https://youtu.be/AnyoUfeNZ1Y

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018, 2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2018, 2019
*/

#include "RPG_Engine.h"

#define X(n) m_script.AddCommand(new cCommand_ ## n)

RPG_Engine::RPG_Engine()
{
	m_sAppName = L"Top Down Role Playing Game";
}



bool RPG_Engine::OnUserCreate()
{
	cCommand::g_engine = this;
	cMap::g_script = &m_script;

	cQuest::g_script = &m_script;
	cQuest::g_engine = this;

	cDynamic::g_engine = this;

	cItem::g_engine = this;

	RPG_Assets::get().LoadSprites();
	RPG_Assets::get().LoadMaps();
	RPG_Assets::get().LoadItems();

	m_sprFont = RPG_Assets::get().GetSprite("font");

	m_listQuests.push_front(new cQuest_MainQuest());

	m_pPlayer = new cDynamic_Creature_Witty();

	m_listItems.push_back(RPG_Assets::get().GetItem("Basic Sword"));


	ChangeMap("coder town", 5, 5);



	


	return true;
}

bool RPG_Engine::OnUserUpdate(float fElapsedTime)
{
	switch (m_nGameMode)
	{
	//case MODE_TITLE:
		//return UpdateTitleScreen(fElapsedTime);
	case MODE_LOCAL_MAP:
		return UpdateLocalMap(fElapsedTime);
	//case MODE_WORLD_MAP:
	//	return UpdateWorldMap(fElapsedTime);
	case MODE_INVENTORY:
		return UpdateInventory(fElapsedTime);
	//case MODE_SHOP:
		//return UpdateShop(fElapsedTime);
	}

	return true;
}

bool RPG_Engine::UpdateLocalMap(float fElapsedTime)
{

	// Update script
	m_script.ProcessCommands(fElapsedTime);

	// Erase and delete redundant projectiles	
	m_vecProjectiles.erase(
		remove_if(m_vecProjectiles.begin(), m_vecProjectiles.end(),
			[](const cDynamic* d) {return ((cDynamic_Projectile*)d)->bRedundant; }), m_vecProjectiles.end());

	if (m_script.bUserControlEnabled)
	{

		m_pPlayer->vx = 0.0f;
		m_pPlayer->vy = 0.0f;
		if (!m_bShowDialog && m_pPlayer->bControllable)
		{

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
					m_nGameMode = MODE_INVENTORY;

				if (GetKey(VK_SPACE).bReleased) // Interaction requested
				{
					// Grab a point from the direction the player is facing and check for interactions										
					float fTestX, fTestY;

					if (m_pPlayer->GetFacingDirection() == 0) // South
					{
						fTestX = m_pPlayer->px + 0.5f;
						fTestY = m_pPlayer->py + 1.5f;
					}

					if (m_pPlayer->GetFacingDirection() == 1) // West
					{
						fTestX = m_pPlayer->px - 0.5f;
						fTestY = m_pPlayer->py + 0.5f;
					}

					if (m_pPlayer->GetFacingDirection() == 2) // North
					{
						fTestX = m_pPlayer->px + 0.5f;
						fTestY = m_pPlayer->py - 0.5f;
					}

					if (m_pPlayer->GetFacingDirection() == 3) // East
					{
						fTestX = m_pPlayer->px + 1.5f;
						fTestY = m_pPlayer->py + 0.5f;
					}

					// Check if test point has hit a dynamic object
					bool bHitSomething = false;
					for (auto dyns : m_vecDynamics)
					{
						if (fTestX > dyns->px && fTestX < (dyns->px + 1.0f) && fTestY > dyns->py && fTestY < (dyns->py + 1.0f))
						{
							if (dyns->bFriendly)
							{
								bHitSomething = true;

								// Iterate through quest stack until something responds, the base quests should capture
								// interactions that are not specfied in other quests
								for (auto &quest : m_listQuests)
									if (quest->OnInteraction(m_vecDynamics, dyns, cQuest::TALK))
									{

										break;
									}

								// Some objects just do stuff when you interact with them
								dyns->OnInteract(m_pPlayer);

								// Then check if it is map related
								m_pCurrentMap->OnInteraction(m_vecDynamics, dyns, cMap::TALK);
							}
							else
							{
								// Interaction was with something not friendly - only enemies
								// are not friendly, so perfrom attack
								m_pPlayer->PerformAttack();
							}
						}
					}

					if (!bHitSomething) // Default action is attack
					{
						m_pPlayer->PerformAttack();
					}

				}
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

	bool bWorkingWithProjectiles = false;
	for (auto &source : { &m_vecDynamics, &m_vecProjectiles })
	{
		for (auto &object : *source)
		{
			float fNewObjectPosX = object->px + object->vx * fElapsedTime;
			float fNewObjectPosY = object->py + object->vy * fElapsedTime;

			// Collision
			float fBorder = 0.1f;
			bool bCollisionWithMap = false;

			if (object->vx <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->py + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->py + (1.0f - fBorder)))
				{
					fNewObjectPosX = (int)fNewObjectPosX + 1;
					object->vx = 0;
					bCollisionWithMap = true;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->py + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->py + (1.0f - fBorder)))
				{
					fNewObjectPosX = (int)fNewObjectPosX;
					object->vx = 0;
					bCollisionWithMap = true;

				}
			}

			if (object->vy <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + fBorder) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + fBorder))
				{
					fNewObjectPosY = (int)fNewObjectPosY + 1;
					object->vy = 0;
					bCollisionWithMap = true;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + (1.0f - fBorder)) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + (1.0f - fBorder)))
				{
					fNewObjectPosY = (int)fNewObjectPosY;
					object->vy = 0;
					bCollisionWithMap = true;
				}
			}

			if (object->bIsProjectile && bCollisionWithMap)
			{
				object->bRedundant = true;
			}

			float fDynamicObjectPosX = fNewObjectPosX;
			float fDynamicObjectPosY = fNewObjectPosY;

			// Object V Object collisions
			for (auto &dyn : m_vecDynamics)
			{
				if (dyn != object)
				{
					// If the object is solid then the player must not overlap it
					if (dyn->bSolidVsDyn && object->bSolidVsDyn)
					{
						// Check if bounding rectangles overlap
						if (fDynamicObjectPosX < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px &&
							object->py < (dyn->py + 1.0f) && (object->py + 1.0f) > dyn->py)
						{
							// First Check Horizontally - Check Left
							if (object->vx <= 0)
								fDynamicObjectPosX = dyn->px + 1.0f;
							else
								fDynamicObjectPosX = dyn->px - 1.0f;
						}

						if (fDynamicObjectPosX < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px &&
							fDynamicObjectPosY < (dyn->py + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->py)
						{

							// First Check Vertically - Check Left
							if (object->vy <= 0)
								fDynamicObjectPosY = dyn->py + 1.0f;
							else
								fDynamicObjectPosY = dyn->py - 1.0f;
						}

					}
					else
					{
						if (object == m_vecDynamics[0])
						{
							// Object is player and can interact with things
							if (fDynamicObjectPosX < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px &&
								object->py < (dyn->py + 1.0f) && (object->py + 1.0f) > dyn->py)
							{
								// First check if object is part of a quest
								for (auto &quest : m_listQuests)
									if (quest->OnInteraction(m_vecDynamics, dyn, cQuest::WALK))
										break;

								// Then check if it is map related
								m_pCurrentMap->OnInteraction(m_vecDynamics, dyn, cMap::WALK);

								// Finally just check the object
								dyn->OnInteract(object);
							}
						}
						else
						{
							if (bWorkingWithProjectiles)
							{
								if (fDynamicObjectPosX < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px &&
									fDynamicObjectPosY < (dyn->py + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->py)
								{
									if (dyn->bFriendly != object->bFriendly)
									{
										// We know object is a projectile, so dyn is something
										// opposite that it has overlapped with											
										if (dyn->bIsAttackable)
										{
											// Dynamic object is a creature
											Damage((cDynamic_Projectile*)object, (cDynamic_Creature*)dyn);
										}
									}
								}
							}


						}
					}
				}
			}


			object->px = fDynamicObjectPosX;
			object->py = fDynamicObjectPosY;
		}

		bWorkingWithProjectiles = true;
	}

	for (auto &source : { &m_vecDynamics, &m_vecProjectiles })
		for (auto &dyns : *source)
			dyns->Update(fElapsedTime, m_pPlayer);

	// Remove quests that have been completed
	auto i = remove_if(m_listQuests.begin(), m_listQuests.end(), [](const cQuest* d) {return d->bCompleted; });
	if (i != m_listQuests.end())
		m_listQuests.erase(i);


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
	for (auto &source : { &m_vecDynamics, &m_vecProjectiles })
		for (auto &dyns : *source)
			dyns->DrawSelf(this, fOffsetX, fOffsetY);

	m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY);

	string sHealth = "HP: " + to_string(m_pPlayer->nHealth) + "/" + to_string(m_pPlayer->nHealthMax);
	DisplayDialog({ sHealth }, 160, 10);


	// Draw any dialog being displayed
	if (m_bShowDialog)
		DisplayDialog(m_vecDialogToShow, 20, 20);


	return true;
}

void RPG_Engine::ShowDialog(vector<string> vecLines)
{
	m_vecDialogToShow = vecLines;
	m_bShowDialog = true;
}

void RPG_Engine::DisplayDialog(vector<string> vecText, int x, int y)
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

void RPG_Engine::DrawBigText(string sText, int x, int y)
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

void RPG_Engine::ChangeMap(string sMapName, float x, float y)
{
	// Destroy all dynamics
	m_vecDynamics.clear();
	m_vecDynamics.push_back(m_pPlayer);

	// Set current map
	m_pCurrentMap = RPG_Assets::get().GetMap(sMapName);

	// Update player location
	m_pPlayer->px = x;
	m_pPlayer->py = y;

	// Create new dynamics from map
	m_pCurrentMap->PopulateDynamics(m_vecDynamics);

	// Create new dynamics from quests
	for (auto q : m_listQuests)
		q->PopulateDynamics(m_vecDynamics, m_pCurrentMap->sName);
}

void RPG_Engine::AddQuest(cQuest* quest)
{
	m_listQuests.push_front(quest);
}

bool RPG_Engine::GiveItem(cItem* item)
{
	//m_script.AddCommand(new cCommand_ShowDialog({ "You have found a" , item->sName }));
	m_listItems.push_back(item);
	return true;
}

bool RPG_Engine::TakeItem(cItem* item)
{
	if (item != nullptr)
	{
		m_listItems.erase(find(m_listItems.begin(), m_listItems.end(), item));
		return true;
	}
	else
		return false;
}

bool RPG_Engine::HasItem(cItem* item)
{
	if (item != nullptr)
		return find(m_listItems.begin(), m_listItems.end(), item) != m_listItems.end();
	else
		return false;
}

bool RPG_Engine::UpdateInventory(float fElapsedTime)
{
	Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
	DrawBigText("INVENTORY", 4, 4);


	int i = 0;
	cItem* highlighted = nullptr;

	// Draw Consumables
	for (auto &item : m_listItems)
	{
		int x = i % 4;
		int y = i / 4;
		i++;

		DrawPartialSprite(8 + x * 20, 20 + y * 20, item->pSprite, 0, 0, 16, 16);

		if (m_nInvSelectX == x && m_nInvSelectY == y)
			highlighted = item;
	}

	// Draw selection reticule
	DrawLine(6 + (m_nInvSelectX) * 20, 18 + (m_nInvSelectY) * 20, 6 + (m_nInvSelectX + 1) * 20, 18 + (m_nInvSelectY) * 20);
	DrawLine(6 + (m_nInvSelectX) * 20, 18 + (m_nInvSelectY + 1) * 20, 6 + (m_nInvSelectX + 1) * 20, 18 + (m_nInvSelectY + 1) * 20);
	DrawLine(6 + (m_nInvSelectX) * 20, 18 + (m_nInvSelectY) * 20, 6 + (m_nInvSelectX) * 20, 18 + (m_nInvSelectY + 1) * 20);
	DrawLine(6 + (m_nInvSelectX + 1) * 20, 18 + (m_nInvSelectY) * 20, 6 + (m_nInvSelectX + 1) * 20, 18 + (m_nInvSelectY + 1) * 20);

	if (GetKey(VK_LEFT).bReleased)	m_nInvSelectX--;
	if (GetKey(VK_RIGHT).bReleased)	m_nInvSelectX++;
	if (GetKey(VK_UP).bReleased)	m_nInvSelectY--;
	if (GetKey(VK_DOWN).bReleased)	m_nInvSelectY++;
	if (m_nInvSelectX < 0) m_nInvSelectX = 3;
	if (m_nInvSelectX >= 4) m_nInvSelectX = 0;
	if (m_nInvSelectY < 0) m_nInvSelectY = 3;
	if (m_nInvSelectY >= 4) m_nInvSelectY = 0;

	if (GetKey(L'Z').bReleased)
		m_nGameMode = MODE_LOCAL_MAP;

	DrawBigText("SELECTED:", 8, 160);

	if (highlighted != nullptr)
	{
		DrawBigText("SELECTED:", 8, 160);
		DrawBigText(highlighted->sName, 8, 170);

		DrawBigText("DESCRIPTION:", 8, 190);
		DrawBigText(highlighted->sDescription, 8, 200);

		if (!highlighted->bKeyItem)
		{
			DrawBigText("(Press SPACE to use)", 80, 160);
		}

		if (GetKey(VK_SPACE).bReleased)
		{
			// Use selected item 
			if (!highlighted->bKeyItem)
			{
				if (highlighted->OnUse(m_pPlayer))
				{
					// Item has signalled it must be consumed, so remove it
					TakeItem(highlighted);
				}
			}
			else
			{
				
			}
		}
	}

	DrawBigText("LOCATION:", 128, 8);
	DrawBigText(m_pCurrentMap->sName, 128, 16);

	DrawBigText("HEALTH: " + to_string(m_pPlayer->nHealth), 128, 32);
	DrawBigText("MAX HEALTH: " + to_string(m_pPlayer->nHealthMax), 128, 40);
	return true;
}

void RPG_Engine::Attack(cDynamic_Creature* aggressor, cWeapon *weapon)
{
	weapon->OnUse(aggressor);
}

void RPG_Engine::AddProjectile(cDynamic_Projectile *proj)
{
	m_vecProjectiles.push_back(proj);
}


void RPG_Engine::Damage(cDynamic_Projectile* projectile, cDynamic_Creature* victim)
{
	if (victim != nullptr)
	{
		// Attack victim with damage
		victim->nHealth -= projectile->nDamage;

		// Knock victim back
		float tx = victim->px - projectile->px;
		float ty = victim->py - projectile->py;
		float d = sqrtf(tx*tx + ty * ty);
		if (d < 1) d = 1.0f;

		// After a hit, they object experiences knock back, where it is temporarily
		// under system control. This delivers two functions, the first being
		// a visual indicator to the player that something has happened, and the second
		// it stops the ability to spam attacks on a single creature
		victim->KnockBack(tx / d, ty / d, 0.2f);

		if (victim != m_pPlayer)
		{
			victim->OnInteract(m_pPlayer);
		}
		else
		{
			// We must ensure the player is never pushed out of bounds by the physics engine. This
			// is a bit of a hack, but it allows knockbacks to occur providing there is an exit
			// point for the player to be knocked back into. If the player is "mobbed" then they
			// become trapped, and must fight their way out
			victim->bSolidVsDyn = true;
		}


		if (projectile->bOneHit)
			projectile->bRedundant = true;
	}
}