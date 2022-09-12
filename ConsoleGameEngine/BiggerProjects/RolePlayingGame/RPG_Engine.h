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


#include <iostream>
#include <string>
#include <memory.h>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngineOOP.h"

#include "RPG_Assets.h"
#include "RPG_Maps.h"
#include "RPG_Dynamics.h"
#include "RPG_Commands.h"
#include "RPG_Quests.h"
#include "RPG_Items.h"

#define X(n) m_script.AddCommand(new cCommand_ ## n)

class RPG_Engine : public olcConsoleGameEngineOOP
{
public:
	RPG_Engine();

private:
	cMap *m_pCurrentMap = nullptr;
	cDynamic_Creature *m_pPlayer = nullptr;
	vector<cDynamic*> m_vecDynamics;    // Fixed
	vector<cDynamic*> m_vecProjectiles; // Transient
	cScriptProcessor m_script;

	list<cQuest*> m_listQuests;
	list<cItem*> m_listItems;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	olcSprite *m_sprFont = nullptr;

	enum
	{
		MODE_TITLE,
		MODE_LOCAL_MAP,
		MODE_WORLD_MAP,
		MODE_INVENTORY,
		MODE_SHOP
	};

	int m_nGameMode = MODE_LOCAL_MAP;

	int m_nInvSelectX = 0;
	int m_nInvSelectY = 0;



protected:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	
	//bool UpdateTitleScreen(float fElapsedTime);
	bool UpdateLocalMap(float fElapsedTime);
	//bool UpdateWorldMap(float fElapsedTime);
	bool UpdateInventory(float fElapsedTime);
	//bool UpdateShop(float fElapsedTime);
		
protected:
	vector<string> m_vecDialogToShow;
	bool m_bShowDialog = false;
	float m_fDialogX = 0.0f;
	float m_fDialogY = 0.0f;

public:
	void ShowDialog(vector<string> vecLines);
	void DisplayDialog(vector<string> vecText, int x, int y);
	void DrawBigText(string sText, int x, int y);
	void ChangeMap(string sMapName, float x, float y);
	void AddQuest(cQuest* quest);

	void AddProjectile(cDynamic_Projectile *proj);

	bool GiveItem(cItem *item);
	bool TakeItem(cItem* item);
	bool HasItem(cItem* item);

	void Attack(cDynamic_Creature* aggressor, cWeapon *weapon);
	void Damage(cDynamic_Projectile* projectile, cDynamic_Creature* victim);
};
