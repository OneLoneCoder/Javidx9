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

#include "RPG_Commands.h"
#include "RPG_Engine.h"
#include "RPG_Quests.h"

RPG_Engine* cCommand::g_engine = nullptr;

cScriptProcessor::cScriptProcessor()
{
	bUserControlEnabled = true;
}

void cScriptProcessor::AddCommand(cCommand *cmd)
{
	m_listCommands.push_back(cmd);
}

void cScriptProcessor::ProcessCommands(float fElapsedTime)
{
	// If command are available, halt user control
	bUserControlEnabled = m_listCommands.empty();

	if (!m_listCommands.empty())
	{
		// A command is available
		if (!m_listCommands.front()->bCompleted)
		{
			// Command has not been started
			if (!m_listCommands.front()->bStarted)
			{
				m_listCommands.front()->Start();
				m_listCommands.front()->bStarted = true;
			}
			else // Command has been started so process it
				m_listCommands.front()->Update(fElapsedTime);
		}
		else
		{
			// Command has been completed
			delete m_listCommands.front();
			m_listCommands.pop_front();
		}
	}
}

// Marks currently active command as complete, from external source
void cScriptProcessor::CompleteCommand()
{
	if (!m_listCommands.empty())
	{
		m_listCommands.front()->bCompleted = true;
	}
}




cCommand_MoveTo::cCommand_MoveTo(cDynamic* object, float x, float y, float duration)
{
	m_fTargetPosX = x;
	m_fTargetPosY = y;
	m_fTimeSoFar = 0.0f;
	m_fDuration = max(duration, 0.001f);
	m_pObject = object;
}

void cCommand_MoveTo::Start()
{
	m_fStartPosX = m_pObject->px;
	m_fStartPosY = m_pObject->py;
}

void cCommand_MoveTo::Update(float fElapsedTime)
{
	m_fTimeSoFar += fElapsedTime;
	float t = m_fTimeSoFar / m_fDuration;
	if (t > 1.0f) t = 1.0f;


	m_pObject->px = (m_fTargetPosX - m_fStartPosX) * t + m_fStartPosX;
	m_pObject->py = (m_fTargetPosY - m_fStartPosY) * t + m_fStartPosY;
	m_pObject->vx = (m_fTargetPosX - m_fStartPosX) / m_fDuration;
	m_pObject->vy = (m_fTargetPosY - m_fStartPosY) / m_fDuration;

	if (m_fTimeSoFar >= m_fDuration)
	{
		// Object has reached destination, so stop
		m_pObject->px = m_fTargetPosX;
		m_pObject->py = m_fTargetPosY;
		m_pObject->vx = 0.0f;
		m_pObject->vy = 0.0f;
		bCompleted = true;
	}
}


cCommand_ShowDialog::cCommand_ShowDialog(vector<string> line)
{
	vecLines = line;
}

void cCommand_ShowDialog::Start()
{
	g_engine->ShowDialog(vecLines);
}


cCommand_ChangeMap::cCommand_ChangeMap(string mapName, float mapPosX, float mapPosY)
{
	m_sMapName = mapName;
	m_fMapPosX = mapPosX;
	m_fMapPosY = mapPosY;
}


void cCommand_ChangeMap::Start()
{
	g_engine->ChangeMap(m_sMapName, m_fMapPosX, m_fMapPosY);
	bCompleted = true;
}

cCommand_AddQuest::cCommand_AddQuest(cQuest* quest)
{
	m_quest = quest;
}

void cCommand_AddQuest::Start()
{
	g_engine->AddQuest(m_quest);
	bCompleted = true;
}