/*
	Aimin Cannons At Worms (Part 1 & 2)
	"C'mon Dad, keep going!" - javidx9

	Video: https://youtu.be/D2a5fHX-Qrs

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2025 OneLoneCoder.com

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

	Video:
	~~~~~~
	https://youtu.be/4hzqmEqHSPY

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	X:			https://www.x.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2025
*/

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

class Projectiles : public olc::PixelGameEngine
{
public:
	Projectiles()
	{
		sAppName = "Projectiles";
	}

	olc::vf2d vSource = { 160.0f, 300.0f };
	olc::vf2d vTarget = { 480.0f, 300.0f };

	float fMuzzleVelocity = 40.0f;
	float fGravity = 4.0f;
	float fWind = 1.0f;

public:
	bool OnUserCreate() override
	{


		return true;
	}

	std::optional<std::pair<float, float>> CalculateTrajectory(
		const olc::vf2d& source,
		const olc::vf2d& target,
		const olc::vf2d& force,
		const float muzzle
	)
	{
		float angleForce = atan2(force.y, force.x) - (3.14159f / 2.0f);
		olc::vf2d D = (target - source).polar();
		D.y -= angleForce;
		D = D.cart();
		float u = muzzle;
		float G = force.mag();

		float a = (0.5f * G * (D.x / u) * (D.x / u));
		float b = D.x;
		float c = (0.5f * G * (D.x / u) * (D.x / u)) - D.y;

		float d = b * b - 4.0f * a * c;

		if (d < 0)
		{
			return std::nullopt;
		}

		float angle1 = atan((-b + sqrt(d)) / (2.0f * a));
		float angle2 = atan((-b - sqrt(d)) / (2.0f * a));

		angle1 += angleForce;
		angle2 += angleForce;

		float dir = (D.x >= 0 ? 0.0f : 3.14159f);
		return std::pair{ dir + angle1, dir + angle2 };
	}


	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetMouse(0).bHeld)
		{
			vSource = GetMousePos();
		}

		if (GetMouse(1).bHeld)
		{
			vTarget = GetMousePos();
		}

		if (GetMouseWheel() > 0)
			fMuzzleVelocity += 1.0f;
		if (GetMouseWheel() < 0)
			fMuzzleVelocity -= 1.0f;

		if (GetKey(olc::Key::LEFT).bHeld)
			fWind -= 1.0f * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld)
			fWind += 1.0f * fElapsedTime;
		if (GetKey(olc::Key::SPACE).bPressed)
			fWind = 0.0f;

		Clear(olc::VERY_DARK_MAGENTA);

		// Draw Source
		FillCircle(vSource, 10, olc::YELLOW);

		// Draw Target
		FillCircle(vTarget, 10, olc::CYAN);

		// Draw Line
		olc::vf2d vLine = (vTarget - vSource).norm();
		DrawLine(vSource - vLine * 2000.0f, vSource + vLine * 2000.0f, olc::MAGENTA, 0xf0f0f0f0);

		olc::vf2d vForce = { fWind, fGravity };
		olc::vf2d vMidPoint = (vSource + vTarget) * 0.5f;
		olc::vf2d vForceNorm = vForce.norm();
		DrawLine(vMidPoint - vForceNorm * 2000.0f, vMidPoint + vForceNorm * 2000.0f, olc::CYAN, 0xf0f0f0f0);



		auto angles = CalculateTrajectory(vSource, vTarget, vForce, fMuzzleVelocity);

		if (!angles.has_value())
			return true;


		olc::vf2d p1_vel = olc::vf2d(fMuzzleVelocity, angles->first).cart();
		olc::vf2d p1_pos = vSource;
		olc::vf2d p2_vel = olc::vf2d(fMuzzleVelocity, angles->second).cart();
		olc::vf2d p2_pos = vSource;

		


		float dt = 0.1f;
		for (int n = 0; n < 1000; n++)
		{
			p1_pos = p1_vel * (n * dt) + 0.5f * vForce * (n * dt) * (n * dt);
			p2_pos = p2_vel * (n * dt) + 0.5f * vForce * (n * dt) * (n * dt);

			Draw(p1_pos + vSource, olc::GREEN);
			Draw(p2_pos + vSource, olc::WHITE);
		}


		return true;
	}
};

int main()
{
	Projectiles demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}