/*
OneLoneCoder.com - Code-It-Yourself! Flappy Bird
"You asked for it... wait, no one did, ever..." - @Javidx9

License
~~~~~~~
Copyright (C) 2018  Javidx9
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
Look it's Flappy Bird, OK? Press Space Bar to Flap. Get a high score.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/b6A4XHkTjs8

Last Updated: 05/11/2017
*/

#include <iostream>
#include <string>
using namespace std;

#include "olcConsoleGameEngine.h"


class OneLoneCoder_FlappyBird : public olcConsoleGameEngine
{
public:
	OneLoneCoder_FlappyBird()
	{
		m_sAppName = L"Flappy Bird";
	}

private:

	float fBirdPosition = 0.0f;
	float fBirdVelocity = 0.0f;
	float fBirdAcceleration = 0.0f;
	float fGravity = 100.0f;
	float fLevelPosition = 0.0f;

	float fSectionWidth;
	list<int> listSection;

	bool bHasCollided = false;
	bool bResetGame = false;

	int nAttemptCount = 0;
	int nFlapCount = 0;
	int nMaxFlapCount = 0;


protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		listSection = { 0, 0, 0, 0 };
		bResetGame = true;
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bResetGame)
		{
			bHasCollided = false;
			bResetGame = false;
			listSection = { 0, 0, 0, 0 };
			fBirdAcceleration = 0.0f;
			fBirdVelocity = 0.0f;
			fBirdPosition = ScreenHeight() / 2.0f;
			nFlapCount = 0;
			nAttemptCount++;
		}

		// Game
		if (bHasCollided)
		{
			// Do nothing until user releases space
			if (m_keys[VK_SPACE].bReleased)
				bResetGame = true;
		}
		else
		{

			if (m_keys[VK_SPACE].bPressed && fBirdVelocity >= fGravity / 10.0f)
			{
				fBirdAcceleration = 0.0f;
				fBirdVelocity = -fGravity / 4.0f;
				nFlapCount++;
				if (nFlapCount > nMaxFlapCount)
					nMaxFlapCount = nFlapCount;
			}
			else
				fBirdAcceleration += fGravity * fElapsedTime;

			if (fBirdAcceleration >= fGravity)
				fBirdAcceleration = fGravity;

			fBirdVelocity += fBirdAcceleration * fElapsedTime;
			fBirdPosition += fBirdVelocity * fElapsedTime;
			fLevelPosition += 14.0f * fElapsedTime;

			if (fLevelPosition > fSectionWidth)
			{
				fLevelPosition -= fSectionWidth;
				listSection.pop_front();
				int i = rand() % (ScreenHeight() - 20);
				if (i <= 10) i = 0;
				listSection.push_back(i);
			}

			// Display
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

			// Draw Sections
			int nSection = 0;
			for (auto s : listSection)
			{
				if (s != 0)
				{
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s, nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(), PIXEL_SOLID, FG_GREEN);
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0, nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 15, PIXEL_SOLID, FG_GREEN);
				}
				nSection++;
			}

			int nBirdX = (int)(ScreenWidth() / 3.0f);

			// Collision Detection
			bHasCollided = fBirdPosition < 2 || fBirdPosition > ScreenHeight() - 2 ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ';

			// Draw Bird
			if (fBirdVelocity > 0)
			{
				DrawString(nBirdX, fBirdPosition + 0, L"\\\\\\");
				DrawString(nBirdX, fBirdPosition + 1, L"<\\\\\\=Q");
			}
			else
			{
				DrawString(nBirdX, fBirdPosition + 0, L"<///=Q");
				DrawString(nBirdX, fBirdPosition + 1, L"///");
			}

			DrawString(1, 1, L"Attempt: " + to_wstring(nAttemptCount) + L" Score: " + to_wstring(nFlapCount) + L" High Score: " + to_wstring(nMaxFlapCount));
		}

		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_FlappyBird game;
	game.ConstructConsole(80, 48, 16, 16);
	game.Start();

	return 0;
}
