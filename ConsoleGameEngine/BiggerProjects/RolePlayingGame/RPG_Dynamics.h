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

#pragma once
#include "RPG_Assets.h"
#include "olcConsoleGameEngineOOP.h"

class RPG_Engine;
class cItem;
class cWeapon;

class cDynamic
{
public:
	cDynamic(string n);
	~cDynamic();

public:
	float px, py;
	float vx, vy;
	bool bSolidVsMap;
	bool bSolidVsDyn;
	bool bFriendly;
	bool bRedundant;
	bool bIsProjectile;
	bool bIsAttackable;
	string sName;

public:
	virtual void DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy) {}
	virtual void Update(float fElapsedTime, cDynamic* player = nullptr) {}
	virtual void OnInteract(cDynamic* player = nullptr) {}

	static RPG_Engine* g_engine;
};



class cDynamic_Creature : public cDynamic
{
public:
	cDynamic_Creature(string n, olcSprite *sprite);

protected:
	olcSprite *m_pSprite;
	float m_fTimer;
	int m_nGraphicCounter;
	enum { SOUTH = 0, WEST = 1, NORTH = 2, EAST = 3 } m_nFacingDirection;
	enum { STANDING, WALKING, CELEBRATING, DEAD } m_nGraphicState;

public:
	int nHealth;
	int nHealthMax;
	int bControllable = true;

public:
	void DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy) override;
	void Update(float fElapsedTime, cDynamic* player = nullptr) override;
	virtual void Behaviour(float fElapsedTime, cDynamic* player = nullptr);
	int GetFacingDirection() { return m_nFacingDirection; };
	virtual void PerformAttack() {};
	void KnockBack(float dx, float dy, float dist);

	cWeapon *pEquipedWeapon = nullptr;

protected:
	float m_fStateTick;
	float m_fKnockBackTimer = 0.0f;
	float m_fKnockBackDX = 0.0f;
	float m_fKnockBackDY = 0.0f;

};


class cDynamic_Creature_Skelly : public cDynamic_Creature
{
public:
	cDynamic_Creature_Skelly();

	void Behaviour(float fElapsedTime, cDynamic* player = nullptr) override;
	void PerformAttack() override;
};

class cDynamic_Creature_Witty : public cDynamic_Creature
{
public:
	cDynamic_Creature_Witty();

public:
	void PerformAttack() override;
};





class cDynamic_Teleport : public cDynamic
{
public:
	cDynamic_Teleport(float x, float y, string sMapName, float tx, float ty);
	void DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy) override;
	void Update(float fElapsedTime, cDynamic* player = nullptr) override;

public:
	string sMapName;
	float fMapPosX;
	float fMapPosY;
};

class cDynamic_Item : public cDynamic
{
public:
	cDynamic_Item(float x, float y, cItem* item);
	void DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy) override;
	void OnInteract(cDynamic* player = nullptr) override;

public:
	cItem * item;
	bool bCollected = false;
};

class cDynamic_Projectile : public cDynamic
{
public:
	cDynamic_Projectile(float ox, float oy, bool bFriend, float velx, float vely, float duration, olcSprite *sprite, float tx, float ty);
	void DrawSelf(olcConsoleGameEngineOOP *gfx, float ox, float oy) override;
	void Update(float fElapsedTime, cDynamic* player = nullptr) override;

public:
	olcSprite * pSprite = nullptr;
	float fSpriteX;
	float fSpriteY;
	float fDuration;
	bool bOneHit = true;
	int nDamage = 0;
};