/*
	Huuuuge Game of Life using Sparse Encoding
	"It's gonna be a fun one tomorrow boss" - javidx9

	Video: https://youtu.be/OqfHIujOvnE

	Use Left mouse button to stimulate a cell, right mouse button to 
	stimulate many cells, space bar to run simulation and middle mouse
	to pan & zoom. 

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
	https://youtu.be/OqfHIujOvnE

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

#define OLC_PGEX_TRANSFORMEDVIEW
#include <extensions/olcPGEX_TransformedView.h>

#include <unordered_set>

struct HASH_OLC_VI2D {
	std::size_t operator()(const olc::vi2d& v) const
	{
		return int64_t(v.y << sizeof(int32_t) | v.x);
	}
};

class SparseEncodedGOL : public olc::PixelGameEngine
{
public:
	SparseEncodedGOL()
	{
		sAppName = "Huge Game Of Life";
	}

protected:
	std::unordered_set<olc::vi2d, HASH_OLC_VI2D> setActive;
	std::unordered_set<olc::vi2d, HASH_OLC_VI2D> setActiveNext;
	std::unordered_set<olc::vi2d, HASH_OLC_VI2D> setPotential;
	std::unordered_set<olc::vi2d, HASH_OLC_VI2D> setPotentialNext;
	olc::TransformedView tv;

protected:
	bool OnUserCreate() override
	{
		tv.Initialise(GetScreenSize());
		return true;
	}

	int GetCellState(const olc::vi2d& in)
	{
		return setActive.contains(in) ? 1 : 0;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		tv.HandlePanAndZoom();

		if (GetKey(olc::Key::SPACE).bHeld)
		{

			setActive = setActiveNext;
			setActiveNext.clear();
			setActiveNext.reserve(setActive.size());


			setPotential = setPotentialNext;

			// We know all active cells this epoch have potential to stimulate next epoch
			setPotentialNext = setActive;

			for (const auto& c : setPotential)
			{
				// Cell has changed, apply rules

				// The secret of artificial life =================================================
				int nNeighbours =
					GetCellState(olc::vi2d(c.x - 1, c.y - 1)) +
					GetCellState(olc::vi2d(c.x - 0, c.y - 1)) +
					GetCellState(olc::vi2d(c.x + 1, c.y - 1)) +
					GetCellState(olc::vi2d(c.x - 1, c.y + 0)) +
					GetCellState(olc::vi2d(c.x + 1, c.y + 0)) +
					GetCellState(olc::vi2d(c.x - 1, c.y + 1)) +
					GetCellState(olc::vi2d(c.x + 0, c.y + 1)) +
					GetCellState(olc::vi2d(c.x + 1, c.y + 1));


				if (GetCellState(c) == 1)
				{
					// if cell is alive and has 2 or 3 neighbours, cell lives on
					int s = (nNeighbours == 2 || nNeighbours == 3);

					if (s == 0)
					{
						// Kill cell through activity omission		

						// Neighbours are stimulated for computation next epoch												
						for (int y = -1; y <= 1; y++)
							for (int x = -1; x <= 1; x++)
								setPotentialNext.insert(c + olc::vi2d(x, y));
					}
					else
					{
						// No Change - Keep cell alive
						setActiveNext.insert(c);
					}


				}
				else
				{
					int s = (nNeighbours == 3);
					if (s == 1)
					{
						// Spawn cell
						setActiveNext.insert(c);

						// Neighbours are stimulated for computation next epoch												
						for (int y = -1; y <= 1; y++)
							for (int x = -1; x <= 1; x++)
								setPotentialNext.insert(c + olc::vi2d(x, y));
					}
					else
					{
						// No Change - Keep cell dead						
					}
				}
				// ===============================================================================
			}
		}


		if (GetMouse(0).bHeld)
		{
			auto m = tv.ScreenToWorld(GetMousePos());

			setActiveNext.insert(m);
			setActive.insert(m);

			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
					setPotentialNext.insert(m + olc::vi2d(x, y));
		}

		if (GetMouse(1).bHeld)
		{
			auto m = tv.ScreenToWorld(GetMousePos());

			for (int i = -50; i <= 50; i++)
				for (int j = -50; j < 50; j++)
				{
					if (rand() % 2)
					{
						setActiveNext.insert(m + olc::vi2d(i, j));
						setActive.insert(m + olc::vi2d(i, j));

						for (int y = -1; y <= 1; y++)
							for (int x = -1; x <= 1; x++)
								setPotentialNext.insert(m + olc::vi2d(i + x, j + y));
					}
				}
		}


		size_t nDrawCount = 0;
		for (const auto& c : setActive)
		{
			if (tv.IsRectVisible(olc::vi2d(c), olc::vi2d(1, 1)))
			{
				tv.FillRectDecal(olc::vi2d(c), olc::vi2d(1, 1), olc::WHITE);
				nDrawCount++;
			}
		}

		DrawStringDecal({ 2,2 }, "Active: " + std::to_string(setActiveNext.size()) + " / " + std::to_string(setPotentialNext.size()) + " : " + std::to_string(nDrawCount));
		return !GetKey(olc::Key::ESCAPE).bPressed;
	}
};


int main()
{
	SparseEncodedGOL demo;
	if (demo.Construct(1280, 960, 1, 1, false))
		demo.Start();

	return 0;
}
