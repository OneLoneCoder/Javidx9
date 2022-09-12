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

#include "RPG_Dynamics.h"

#include "RPG_Engine.h"

RPG_Engine* cDynamic::g_engine = nullptr;

cDynamic::cDynamic(string n)
{
	sName = n;
	px = 0.0f;
	py = 0.0f;
	vx = 0.0f;
	vy = 0.0f;
	bSolidVsMap = true;
	bSolidVsDyn = true;
	bFriendly = true;
	bRedundant = false;
	bIsAttackable = false;
	bIsProjectile = false;
}

cDynamic::~cDynamic()
{

}


/////////////////////////////////////////////////////////////////////

cDynamic_Creature::cDynamic_Creature(string name, olcSprite *sprite) : cDynamic(name)
{
	m_pSprite = sprite;
	nHealth = 10;
	nHealthMax = 10;
	m_nFacingDirection = SOUTH;
	m_nGraphicState = STANDING;
	m_nGraphicCounter = 0;
	m_fTimer = 0.0f;
	bIsAttackable = true;
}

void cDynamic_Creature::Update(float fElapsedTime, cDynamic* player)
{
	if (m_fKnockBackTimer > 0.0f)
	{
		vx = m_fKnockBackDX * 10.0f;
		vy = m_fKnockBackDY * 10.0f;
		bIsAttackable = false;
		m_fKnockBackTimer -= fElapsedTime;
		if (m_fKnockBackTimer <= 0.0f)
		{
			m_fStateTick = 0.0f;
			bControllable = true;
			bIsAttackable = true;
		}

	}
	else
	{
		bSolidVsDyn = true;
		m_fTimer += fElapsedTime;
		if (m_fTimer >= 0.2f)
		{
			m_fTimer -= 0.2f;
			m_nGraphicCounter++;
			m_nGraphicCounter %= 2;
		}

		if (fabs(vx) > 0 || fabs(vy) > 0)
			m_nGraphicState = WALKING;
		else
			m_nGraphicState = STANDING;

		if (nHealth <= 0)
			m_nGraphicState = DEAD;

		if (vx < 0.0f) m_nFacingDirection = WEST;
		if (vx > 0.0f) m_nFacingDirection = EAST;
		if (vy < -0.0f) m_nFacingDirection = NORTH;
		if (vy > 0.0f) m_nFacingDirection = SOUTH;

		Behaviour(fElapsedTime, player);
	}
}

void cDynamic_Creature::KnockBack(float dx, float dy, float dist)
{
	m_fKnockBackDX = dx;
	m_fKnockBackDY = dy;
	m_fKnockBackTimer = dist;
	bSolidVsDyn = false;
	bControllable = false;
	bIsAttackable = false;
}


void cDynamic_Creature::DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy)
{
	int nSheetOffsetX = 0;
	int nSheetOffsetY = 0;

	switch (m_nGraphicState)
	{
	case STANDING:
		nSheetOffsetX = m_nFacingDirection * 16;
		break;

	case WALKING:
		nSheetOffsetX = m_nFacingDirection * 16;
		nSheetOffsetY = m_nGraphicCounter * 16;
		break;

	case CELEBRATING:
		nSheetOffsetX = 4 * 16;
		break;

	case DEAD:
		nSheetOffsetX = 4 * 16;
		nSheetOffsetY = 1 * 16;
		break;

	}

	gfx->DrawPartialSprite((px - ox) * 16.0f, (py - oy)*16.0f, m_pSprite, nSheetOffsetX, nSheetOffsetY, 16, 16);
}

void cDynamic_Creature::Behaviour(float fElapsedTime, cDynamic* player)
{
	// No default behaviour
}




cDynamic_Creature_Witty::cDynamic_Creature_Witty() : cDynamic_Creature("witty", RPG_Assets::get().GetSprite("player"))
{
	bFriendly = true;
	nHealth = 9;
	nHealthMax = 10;
	m_fStateTick = 2.0f;
	pEquipedWeapon = (cWeapon*)RPG_Assets::get().GetItem("Basic Sword");
}

void cDynamic_Creature_Witty::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	pEquipedWeapon->OnUse(this);
}




cDynamic_Creature_Skelly::cDynamic_Creature_Skelly() : cDynamic_Creature("Skelly", RPG_Assets::get().GetSprite("skelly"))
{
	bFriendly = false;
	nHealth = 10;
	nHealthMax = 10;
	m_fStateTick = 2.0f;

	pEquipedWeapon = (cWeapon*)RPG_Assets::get().GetItem("Basic Sword");
}


void cDynamic_Creature_Skelly::Behaviour(float fElapsedTime, cDynamic* player)
{
	if (nHealth <= 0)
	{
		vx = 0;
		vy = 0;
		bSolidVsDyn = false;
		bIsAttackable = false;
		return;
	}

	// Check if player is nearby
	float fTargetX = player->px - px;
	float fTargetY = player->py - py;
	float fDistance = sqrtf(fTargetX*fTargetX + fTargetY * fTargetY);

	m_fStateTick -= fElapsedTime;

	if (m_fStateTick <= 0.0f)
	{
		if (fDistance < 6.0f)
		{
			vx = (fTargetX / fDistance) * 2.0f;
			vy = (fTargetY / fDistance) * 2.0f;

			if (fDistance < 1.5f)
				PerformAttack();
		}
		else
		{
			vx = 0;
			vy = 0;
		}

		m_fStateTick += 1.0f;
	}
}

void cDynamic_Creature_Skelly::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	pEquipedWeapon->OnUse(this);
}







cDynamic_Teleport::cDynamic_Teleport(float x, float y, string mapName, float tx, float ty) : cDynamic("Teleport")
{
	px = x;
	py = y;
	fMapPosX = tx;
	fMapPosY = ty;
	sMapName = mapName;
	bSolidVsDyn = false;
	bSolidVsMap = false;
}

void cDynamic_Teleport::DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy)
{
	// Does Nothing
	gfx->DrawCircle(((px + 0.5f) - ox) * 16.0f, ((py + 0.5f) - oy) * 16.0f, 0.5f * 16.0f); // For debugging
}

void cDynamic_Teleport::Update(float fElapsedTime, cDynamic* player)
{
	// Does Nothing
}


cDynamic_Item::cDynamic_Item(float x, float y, cItem* i) : cDynamic("pickup")
{
	px = x;
	py = y;
	bSolidVsDyn = false;
	bSolidVsMap = false;
	bFriendly = true;
	bCollected = false;
	item = i;
}

void cDynamic_Item::DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy)
{
	if (bCollected)
		return;

	gfx->DrawPartialSprite((px - ox) * 16.0f, (py - oy)*16.0f, item->pSprite, 0, 0, 16, 16);
}

void cDynamic_Item::OnInteract(cDynamic* player)
{
	if (bCollected)
		return;

	if (item->OnInteract(player))
	{
		// Add item to inventory
		g_engine->GiveItem(item);
	}

	bCollected = true;
}

cDynamic_Projectile::cDynamic_Projectile(float ox, float oy, bool bFriend, float velx, float vely, float duration, olcSprite *sprite, float tx, float ty) : cDynamic("projectile")
{
	pSprite = sprite;
	fSpriteX = tx;
	fSpriteY = ty;
	fDuration = duration;
	px = ox;
	py = oy;
	vx = velx;
	vy = vely;
	bSolidVsDyn = false;
	bSolidVsMap = true;
	bIsProjectile = true;
	bIsAttackable = false;
	bFriendly = bFriend;
}

void cDynamic_Projectile::DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy)
{
	gfx->DrawPartialSprite((px - ox) * 16, (py - oy) * 16, pSprite, fSpriteX * 16, fSpriteY * 16, 16, 16);

}

void cDynamic_Projectile::Update(float fElapsedTime, cDynamic* player)
{
	fDuration -= fElapsedTime;
	if (fDuration <= 0.0f)
		bRedundant = true;
}
