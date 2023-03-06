/*
	Coding Quickie: Handling Periodic Numbers with Templates
	"Don't need no ceiling" - javidx9
	
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018-2023 OneLoneCoder.com
	
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
	
	Relevant Video: https://youtu.be/gsOtwF2sOLc
	
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com
	
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2023
*/

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>


template<class TL, class TR>
constexpr auto circ_add(
	const TL& lhs, 
	const TR& rhs, 
	const decltype(lhs + rhs) rmin = 0, 
	const decltype(lhs + rhs) rmax = 360)
{
	auto c = lhs + rhs;
	auto range = rmax - rmin;
	while (c >= rmax)  c -= range;
	while (c <  rmin)  c += range;
	return c;
}

template<class TL, class TR>
constexpr auto circ_sub(const TL& lhs, const TR& rhs, const decltype(lhs - rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	auto c = lhs - rhs;
	auto range = rmax - rmin;
	while (c >= rmax)  c -= range;
	while (c < rmin)  c += range;
	return c;
}

template<class TL, class TR>
constexpr auto circ_mul(const TL& lhs, const TR& rhs, const decltype(lhs * rhs) rmin = 0, const decltype(lhs * rhs) rmax = 360)
{
	auto c = lhs * rhs;
	auto range = rmax - rmin;
	while (c >= rmax)  c -= range;
	while (c < rmin)  c += range;
	return c;
}

template<class TL, class TR>
constexpr auto circ_div(const TL& lhs, const TR& rhs, const decltype(lhs / rhs) rmin = 0, const decltype(lhs / rhs) rmax = 360)
{
	auto c = lhs / rhs;
	auto range = rmax - rmin;
	while (c >= rmax)  c -= range;
	while (c < rmin)  c += range;
	return c;
}


template<class TL, class TR>
constexpr auto circ_lerp(const double t, const TL& lhs, const TR& rhs, const decltype(lhs - rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	return circ_add(lhs, circ_sub(rhs, lhs, rmin, rmax) * t, rmin, rmax);
}

template<class TL, class TR>
constexpr auto circ_shortdist(const TL& lhs, const TR& rhs, const decltype(lhs-rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	auto a = circ_sub(rhs, lhs, rmin, rmax);
	auto b = circ_sub(lhs, rhs, rmin, rmax);
	return std::min(a, b);
}

template<class TL, class TR>
constexpr auto circ_longdist(const TL& lhs, const TR& rhs, const decltype(lhs - rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	auto a = circ_sub(rhs, lhs, rmin, rmax);
	auto b = circ_sub(lhs, rhs, rmin, rmax);
	return std::max(a, b);
}

template<class TL, class TR>
constexpr auto circ_shortdiff(const TL& lhs, const TR& rhs, const decltype(lhs - rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	auto a = circ_sub(rhs, lhs, rmin, rmax);
	auto b = circ_sub(lhs, rhs, rmin, rmax);
	if (b > a)
		return a;
	else
		return -b;
}

template<class TL, class TR>
constexpr auto circ_shortlerp(const double t, const TL& lhs, const TR& rhs, const decltype(lhs - rhs) rmin = 0, const decltype(lhs - rhs) rmax = 360)
{
	auto a = circ_sub(rhs, lhs, rmin, rmax);
	auto b = circ_sub(lhs, rhs, rmin, rmax);
	if(a < b)
		return circ_add(lhs, a * t, rmin, rmax);
	else
		return circ_sub(lhs, b * t, rmin, rmax);
}



//int main()
//{
//	constexpr float a = 10;
//	constexpr int b = 120;
//	constexpr float c = circ_sub(a, b, -100, +100);
//	return 0;
//}



class PeriodicNumbers : public olc::PixelGameEngine
{
public:
	PeriodicNumbers()
	{
		sAppName = "Periodic Numbers";
	}

	struct Unit
	{		
		float fHeading;		
		olc::vf2d vPosition;
		olc::vf2d vTarget;
	};

	olc::vf2d vTarget;
	float fTime = 0.0;
	
	olc::Renderable gfxEye;
	std::vector<Unit> vecUnits;
	bool bCircularNumbers = true;

public:
	bool OnUserCreate() override
	{
		int nUnits = 60;
		for (int i = 0; i < nUnits; i++)
			vecUnits.push_back({ 0.0f, olc::vf2d(rand() % ScreenWidth(), rand() % ScreenHeight()) });

		gfxEye.Load("./assets/eye2_tutas_512x512.png", nullptr, true);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_MAGENTA);

		if (GetMouse(0).bHeld)
		{
			vTarget = GetMousePos();
			fTime = 0;
		}

		if (GetKey(olc::Key::SPACE).bPressed)
			bCircularNumbers = !bCircularNumbers;

		fTime += fElapsedTime;

		for (auto& unit : vecUnits)
		{

			float fAngleToNewTarget = atan2(unit.vPosition.y - vTarget.y, unit.vPosition.x - vTarget.x) + 3.14159f;
				

			//float delta = fAngleToNewTarget - unit.fHeading;
			//unit.fHeading += delta * fElapsedTime * 10.0f;
		
			float delta = circ_shortdiff(unit.fHeading, fAngleToNewTarget, 0.0f, 2.0f * 3.14159f);
			unit.fHeading = circ_add(unit.fHeading, delta * fElapsedTime * 10.0f, 0.0f, 2.0f * 3.14159f);
			


			DrawRotatedDecal(unit.vPosition, gfxEye.Decal(), unit.fHeading, { 256, 256 }, {0.039f, 0.039f});
		}

		FillCircle(vTarget, 3, olc::RED);

		return true;
	}
};

int main()
{
	PeriodicNumbers demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}