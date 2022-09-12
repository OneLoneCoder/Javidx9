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

#include "RPG_Dynamics.h"
#include "RPG_Commands.h"

class RPG_Engine;

class cQuest
{
public:
	enum NATURE
	{
		TALK = 0,
		ATTACK = 1,
		KILL = 2,
		WALK = 3
	};

public:
	cQuest();

public:
	virtual bool OnInteraction(vector<cDynamic*> &vecDynobs, cDynamic *target, NATURE nature);
	virtual bool PopulateDynamics(vector<cDynamic*> &vecDyns, string sMap);

public:
	string sName;
	bool bCompleted = false;
	static cScriptProcessor* g_script;
	static RPG_Engine* g_engine;
};



class cQuest_MainQuest : public cQuest
{
public:
	bool PopulateDynamics(vector<cDynamic*> &vecDyns, string sMap) override;
	bool OnInteraction(vector<cDynamic*> &vecDynobs, cDynamic *target, NATURE nature) override;

private:
	int m_nPhase = 0;
};


class cQuest_BobsQuest : public cQuest
{
public:
	bool PopulateDynamics(vector<cDynamic*> &vecDyns, string sMap) override;
	bool OnInteraction(vector<cDynamic*> &vecDynobs, cDynamic *target, NATURE nature) override;

private:
	int m_nPhase = 0;
};