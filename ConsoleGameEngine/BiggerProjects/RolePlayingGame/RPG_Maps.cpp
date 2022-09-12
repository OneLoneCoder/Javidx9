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

#include "RPG_Maps.h"

#include <fstream>

cScriptProcessor* cMap::g_script = nullptr;

#define X(n) g_script->AddCommand(new cCommand_ ## n)

cMap::cMap()
{
	pSprite = nullptr;
	nWidth = 0;
	nHeight = 0;
	m_solids = nullptr;
	m_indices = nullptr;
}


cMap::~cMap()
{
	delete[] m_solids;
	delete[] m_indices;
}

int cMap::GetIndex(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_indices[y*nWidth + x];
	else
		return 0;
}

bool cMap::GetSolid(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_solids[y*nWidth + x];
	else
		return true;
}

bool cMap::Create(string fileData, olcSprite* sprite, string name)
{
	sName = name;
	pSprite = sprite;
	ifstream data(fileData, ios::in | ios::binary);
	if (data.is_open())
	{
		data >> nWidth >> nHeight;
		m_solids = new bool[nWidth * nHeight];
		m_indices = new int[nWidth * nHeight];
		for (int i = 0; i < nWidth * nHeight; i++)
		{
			data >> m_indices[i];
			data >> m_solids[i];
		}
		return true;
	}

	return false;
}








cMap_Village1::cMap_Village1()
{
	Create("rpgdata/map/village1.lvl", RPG_Assets::get().GetSprite("village"), "coder town");
}

bool cMap_Village1::PopulateDynamics(vector<cDynamic*> &vecDyns)
{
	// Add Teleporters
	vecDyns.push_back(new cDynamic_Teleport(12.0f, 6.0f, "home", 5.0f, 12.0f));

	// Add Items
	vecDyns.push_back(new cDynamic_Item(10, 10, RPG_Assets::get().GetItem("Small Health")));
	vecDyns.push_back(new cDynamic_Item(12, 10, RPG_Assets::get().GetItem("Health Boost")));

	for (int i = 0; i < 3; i++)
	{
		cDynamic* g1 = new cDynamic_Creature_Skelly();
		vecDyns.push_back(g1);
		g1->px = rand() % 10 + 5.0f;
		g1->py = rand() % 10 + 5.0f;
	}

	return true;
}


bool cMap_Village1::OnInteraction(vector<cDynamic*> &vecDynobs, cDynamic *target, NATURE nature)
{
	if (target->sName == "Teleport")
		g_script->AddCommand(new cCommand_ChangeMap(
			((cDynamic_Teleport*)target)->sMapName,
			((cDynamic_Teleport*)target)->fMapPosX,
			((cDynamic_Teleport*)target)->fMapPosY));

	return false;
}












cMap_Home1::cMap_Home1()
{
	Create("rpgdata/map/home.lvl", RPG_Assets::get().GetSprite("hitech"), "home");
}

bool cMap_Home1::PopulateDynamics(vector<cDynamic*> &vecDyns)
{
	// Front door
	vecDyns.push_back(new cDynamic_Teleport(5.0f, 13.0f, "coder town", 12.0f, 7.0f));
	vecDyns.push_back(new cDynamic_Teleport(4.0f, 13.0f, "coder town", 12.0f, 7.0f));

	/*cDynamic_Creature* c1 = new cDynamic_Creature("bob", RPG_Assets::get().GetSprite("skelly"));
	c1->px = 12.0f;
	c1->py = 4.0f;
	vecDyns.push_back(c1);*/

	return true;
}


bool cMap_Home1::OnInteraction(vector<cDynamic*> &vecDynobs, cDynamic *target, NATURE nature)
{
	if (target->sName == "Teleport")
		g_script->AddCommand(new cCommand_ChangeMap(
		((cDynamic_Teleport*)target)->sMapName,
			((cDynamic_Teleport*)target)->fMapPosX,
			((cDynamic_Teleport*)target)->fMapPosY));

	/*if (target->sName == "bob")
	{
		X(ShowDialog({ "Hello!", "I'm Bob!" }));
	}*/

	return false;
}